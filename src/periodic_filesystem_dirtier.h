// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PERIODIC_FILESYSTEM_DIRTIER_H
#define PERIODIC_FILESYSTEM_DIRTIER_H

#include <chrono>
#include <string>

class FilesystemDirt;

// A utility class for periodically marking certain files or directories
// dirty. This is used by the syncer to schedule a full sync every
// couple of hours. Strictly speaking this shouldn't be needed, as the
// syncer already does a full sync when event queue overflows occur.
// Still, doing a full sync rarely never hurts.
class PeriodicFilesystemDirtier {
 public:
  PeriodicFilesystemDirtier(FilesystemDirt* filesystem_dirt,
                            const std::string& path,
                            std::chrono::steady_clock::duration interval)
      : filesystem_dirt_(filesystem_dirt), path_(path), interval_(interval) {
  }

  // Marks the stored path dirty if the time exceeds a point in time.
  void Tick();

 private:
  // Filesystem dirt tracker.
  FilesystemDirt* const filesystem_dirt_;

  // Path that should be marked dirty in the dirt tracker.
  const std::string path_;

  // Interval between periodic dirtying.
  const std::chrono::steady_clock::duration interval_;

  // Next moment in time at which dirtying should occur.
  std::chrono::steady_clock::time_point next_cycle_;
};

#endif
