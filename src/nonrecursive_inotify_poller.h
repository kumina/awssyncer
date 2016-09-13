// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef NONRECURSIVE_INOTIFY_POLLER_H
#define NONRECURSIVE_INOTIFY_POLLER_H

#include "inotify_poller.h"

#include <cstddef>
#include <experimental/optional>
#include <map>
#include <string>

// Poller of inotify events.
class NonrecursiveInotifyPoller : public InotifyPoller {
 public:
  NonrecursiveInotifyPoller();
  ~NonrecursiveInotifyPoller();

  // Attaches a watch to a single directory.
  bool AddWatch(const std::string& path) override;

  // Extracts the next event from the inotify event queue. Returns an
  // object that evaluates to false if no events are present.
  std::experimental::optional<InotifyEvent> GetNextEvent() override;

  // Whether or not this queue had to drop events due to a large amount of
  // activity happening on the file system.
  bool EventsDropped() override {
    return events_dropped_;
  }

 private:
  const int fd_;
  std::map<int, std::string> directories_;
  bool events_dropped_;

  char read_buffer_[65536];
  std::size_t read_buffer_length_;
  std::size_t read_buffer_offset_;
};

#endif
