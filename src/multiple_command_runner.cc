// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "multiple_command_runner.h"
#include "command_runner.h"

#include <cassert>

bool MultipleCommandRunner::Finished() {
  if (!command_runner_->Finished())
    return false;
  if (PreviousCommandFailed())
    return true;

  if (!enqueued_commands_.empty()) {
    const auto& next = enqueued_commands_.front();
    command_runner_->RunCommand(next.first);
    ignore_failures_for_current_command_ = next.second;
    enqueued_commands_.pop();
    return false;
  }

  return true;
}

bool MultipleCommandRunner::PreviousCommandFailed() {
  if (previous_command_failed_)
    return true;
  if (!ignore_failures_for_current_command_)
    previous_command_failed_ = command_runner_->PreviousCommandFailed();
  return previous_command_failed_;
}

void MultipleCommandRunner::RunCommand(const std::vector<std::string>& command,
                                       bool ignore_failure) {
  assert(!previous_command_failed_ &&
         "Attempted to schedule more commands after failure");
  enqueued_commands_.push({command, ignore_failure});
}
