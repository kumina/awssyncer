#include "inotify_poller.h"

#include <sys/inotify.h>

#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstring>

InotifyPoller::InotifyPoller()
    : fd_(inotify_init1(IN_NONBLOCK | IN_CLOEXEC)),
      read_buffer_length_(0),
      read_buffer_offset_(0) {
  assert(fd_ >= 0 && "Failed to create inotify descriptor");
}

bool InotifyPoller::AddWatch(const std::string &path) {
  // Create new inotify watch.
  int watch = inotify_add_watch(
      fd_, path.c_str(),
      IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
  if (watch < 0)
    return false;

  // Keep track of watch and pathname string, so we can recombine events to
  // full pathname strings.
  directories_.insert({watch, path});
  return true;
}

void InotifyPoller::RemoveWatch(const std::string &path) {
  // TODO(ed): This would have been easier if we could use Boost's Bimap.
  for (auto it = directories_.cbegin(); it != directories_.cend();) {
    if (it->second == path) {
      // Found existing watch that uses this pathname. Remove it.
      inotify_rm_watch(fd_, it->first);
      directories_.erase(it++);
    } else {
      ++it;
    }
  }
}

bool InotifyPoller::GetNextEvent(InotifyEvent *event) {
  // Read new events from the kernel if needed.
  if (read_buffer_length_ - read_buffer_offset_ <
      sizeof(struct inotify_event)) {
    ssize_t readlen = read(fd_,  read_buffer_, sizeof(read_buffer_));
    if (readlen == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
      return false;
    assert(readlen >= (ssize_t)sizeof(struct inotify_event) && "inotify read failed");
    read_buffer_offset_ = 0;
    read_buffer_length_ = readlen;
  }

  // Copy next event from read buffer to get the alignment right.
  struct inotify_event ev;
  memcpy(&ev, read_buffer_, sizeof(ev));
  read_buffer_offset_ += sizeof(ev);
  assert(read_buffer_offset_ <= read_buffer_length_ &&
         "Attempted to escape read buffer");

  // Extract scalar data from the event.
  event->is_directory = (ev.mask & IN_ISDIR) != 0;
  event->queue_overflow = (ev.mask & IN_Q_OVERFLOW) != 0;

  // Extract the pathname string.
  event->path = directories_[ev.wd];
  event->path += '/';
  event->path += std::string(read_buffer_ + read_buffer_offset_, ev.len - 1);
  read_buffer_offset_ += ev.len;
  assert(read_buffer_offset_ <= read_buffer_length_ &&
         "Attempted to escape read buffer");

  return true;
}
