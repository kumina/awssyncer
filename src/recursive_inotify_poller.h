#ifndef RECURSIVE_INOTIFY_POLLER_H
#define RECURSIVE_INOTIFY_POLLER_H

#include "inotify_poller.h"

// Inotify poller that automatically inspects a full directory
// hierarchy.
class RecursiveInotifyPoller : public InotifyPoller {
 public:
  RecursiveInotifyPoller(InotifyPoller* ip) : ip_(ip) {
  }

  // Adds watches for a directory and all of its child directories.
  bool AddWatch(const std::string& path) override;

  // Gets the next event from the inotify poller.
  bool GetNextEvent(InotifyEvent* event) override;

  // Whether or not this queue had to drop events due to a large amount of
  // activity happening on the file system.
  bool EventsDropped() override {
    return ip_->EventsDropped();
  }

 private:
  InotifyPoller* ip_;
};

#endif
