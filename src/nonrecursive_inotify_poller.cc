#include "nonrecursive_inotify_poller.h"

#include <sys/inotify.h>

#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstring>

NonrecursiveInotifyPoller::NonrecursiveInotifyPoller()
    : fd_(inotify_init1(IN_NONBLOCK | IN_CLOEXEC)),
      events_dropped_(false),
      read_buffer_length_(0),
      read_buffer_offset_(0) {
  assert(fd_ >= 0 && "Failed to create inotify descriptor");
}

NonrecursiveInotifyPoller::~NonrecursiveInotifyPoller() {
  close(fd_);
}

bool NonrecursiveInotifyPoller::AddWatch(const std::string& path) {
  // Create new inotify watch.
  assert(path[path.size() - 1] == '/' &&
         "Directory names must end with a trailing slash");
  int watch = inotify_add_watch(
      fd_, path.c_str(), IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY |
                             IN_MOVED_FROM | IN_MOVED_TO);
  if (watch < 0)
    return false;

  // Keep track of watch and pathname string, so we can recombine events to
  // full pathname strings.
  directories_.insert({watch, path});
  return true;
}

bool NonrecursiveInotifyPoller::GetNextEvent(InotifyEvent* event) {
  struct inotify_event ev;
  for (;;) {
    // Read new events from the kernel if needed.
    if (read_buffer_offset_ + sizeof(struct inotify_event) >
        read_buffer_length_) {
      ssize_t readlen = read(fd_, read_buffer_, sizeof(read_buffer_));
      if (readlen == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return false;
      assert(readlen >= (ssize_t)sizeof(struct inotify_event) &&
             "inotify read failed");
      read_buffer_offset_ = 0;
      read_buffer_length_ = readlen;
    }

    // Copy next event from read buffer to get the alignment right.
    memcpy(&ev, read_buffer_ + read_buffer_offset_, sizeof(ev));
    read_buffer_offset_ += sizeof(ev);
    assert(read_buffer_offset_ <= read_buffer_length_ &&
           "Attempted to escape read buffer");

    // If the kernel is going to discard this watch, remove it from our
    // bookkeeping as well.
    if ((ev.mask & IN_IGNORED) != 0)
      directories_.erase(ev.wd);

    // Record that we've been dropping events.
    if ((ev.mask & IN_Q_OVERFLOW) != 0)
      events_dropped_ = true;

    if (ev.len > 0) {
      // Extract the event type.
      switch (ev.mask & IN_ALL_EVENTS) {
        case IN_CREATE:
        case IN_MOVED_TO:
          event->type = InotifyEventType::CREATED;
          break;
        case IN_DELETE:
        case IN_MOVED_FROM:
          event->type = InotifyEventType::DELETED;
          break;
        case IN_MODIFY:
          event->type = InotifyEventType::MODIFIED;
          break;
        default:
          assert(0 && "Unknown event type");
      }

      // Extract the pathname string.
      event->path = directories_[ev.wd];
      event->path += std::string(read_buffer_ + read_buffer_offset_);
      if ((ev.mask & IN_ISDIR) != 0)
        event->path += '/';
      read_buffer_offset_ += ev.len;
      assert(read_buffer_offset_ <= read_buffer_length_ &&
             "Attempted to escape read buffer");
      return true;
    }
  }
}
