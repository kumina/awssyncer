// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef RECURSIVE_INOTIFY_POLLER_H
#define RECURSIVE_INOTIFY_POLLER_H

#include "inotify_poller.h"

#include <experimental/optional>
#include <string>

// Inotify poller that automatically inspects a full directory
// hierarchy.
class RecursiveInotifyPoller : public InotifyPoller {
 public:
  RecursiveInotifyPoller(InotifyPoller* ip) : ip_(ip) {
  }

  // Adds watches for a directory and all of its child directories.
  bool AddWatch(const std::string& path) override;

  // Gets the next event from the inotify poller.
  std::experimental::optional<InotifyEvent> GetNextEvent() override;

  // Whether or not this queue had to drop events due to a large amount of
  // activity happening on the file system.
  bool EventsDropped() override {
    return ip_->EventsDropped();
  }

 private:
  InotifyPoller* const ip_;
};

#endif
