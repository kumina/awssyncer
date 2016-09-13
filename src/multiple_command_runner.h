// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef MULTIPLE_COMMAND_RUNNER_H
#define MULTIPLE_COMMAND_RUNNER_H

#include <queue>
#include <string>
#include <vector>

class CommandRunner;

// Command runner that can batch multiple commands and run them sequentially.
class MultipleCommandRunner {
 public:
  MultipleCommandRunner(CommandRunner* command_runner)
      : command_runner_(command_runner),
        ignore_failures_for_current_command_(false),
        previous_command_failed_(false) {
  }

  // Returns true of there is no command currently running.
  bool Finished();

  // Returns true if one of the previous commands did not complete
  // successfully.
  bool PreviousCommandFailed();

  // Enqueues a command by specifying a list of command line arguments.
  void RunCommand(const std::vector<std::string>& command, bool ignore_failure);

 private:
  CommandRunner* const command_runner_;
  std::queue<std::pair<std::vector<std::string>, bool>> enqueued_commands_;
  bool ignore_failures_for_current_command_;
  bool previous_command_failed_;
};

#endif
