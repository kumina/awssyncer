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
