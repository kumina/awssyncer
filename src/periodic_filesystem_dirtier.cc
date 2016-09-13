// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "periodic_filesystem_dirtier.h"
#include "filesystem_dirt.h"

#include <chrono>
#include <string>

void PeriodicFilesystemDirtier::Tick() {
  auto now = std::chrono::steady_clock::now();
  if (now >= next_cycle_) {
    filesystem_dirt_->AddDirtyPath(path_);
    next_cycle_ = now + interval_;
  }
}
