#ifndef FILTERING_INOTIFY_POLLER_H
#define FILTERING_INOTIFY_POLLER_H

#include "inotify_poller.h"

#include <regex.h>

// Inotify poller that can remove files and/or directories from the results
// when they match a regular expression.
//
// Matching is performed on the full pathname. Directories contain trailing
// slashes.
class FilteringInotifyPoller : public InotifyPoller {
 public:
  FilteringInotifyPoller(InotifyPoller* ip, const std::string& regex);

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
  regex_t regex_;

  bool MatchPath(const std::string& path);
};

#endif
