#ifndef RECURSIVE_INOTIFY_POLLER_H
#define RECURSIVE_INOTIFY_POLLER_H

#include "inotify_poller.h"

// Inotify poller that automatically inspects a full directory
// hierarchy.
class RecursiveInotifyPoller {
 public:
  // Adds watches for a directory and all of its child directories.
  bool AddWatch(const std::string &path);

  // Gets the next event from the inotify poller.
  bool GetNextEvent(InotifyEvent *event);

 private:
  InotifyPoller ip_;
};

#endif
