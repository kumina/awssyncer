#ifndef NONRECURSIVE_INOTIFY_POLLER_H
#define NONRECURSIVE_INOTIFY_POLLER_H

#include "inotify_poller.h"

#include <map>
#include <string>

// Poller of inotify events.
class NonrecursiveInotifyPoller : public InotifyPoller {
 public:
  NonrecursiveInotifyPoller();

  // Attaches a watch to a single directory.
  bool AddWatch(const std::string &path) override;

  // Extracts the next event from the inotify event queue. Returns false
  // if no events are present.
  bool GetNextEvent(InotifyEvent *event) override;

  // Whether or not this queue had to drop events due to a large amount of
  // activity happening on the file system.
  bool EventsDropped() override { return events_dropped_; }

 private:
  int fd_;
  std::map<int, std::string> directories_;
  bool events_dropped_;

  char read_buffer_[65536];
  size_t read_buffer_length_;
  size_t read_buffer_offset_;
};

#endif
