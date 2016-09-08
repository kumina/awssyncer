#ifndef POSIX_COMMAND_RUNNER_H
#define POSIX_COMMAND_RUNNER_H

#include "command_runner.h"

#include <string>
#include <vector>

// Utility class for running shell commands.
class PosixCommandRunner : public CommandRunner {
 public:
  PosixCommandRunner()
      : current_process_(-1), previous_command_failed_(false) {}

  // Returns true of there is no command currently running.
  bool Finished() override;

  // Returns true if the previous command did not complete successfully.
  bool PreviousCommandFailed() override;

  // Runs a command by specifying a list of command line arguments.
  void RunCommand(const std::vector<std::string> &command) override;

 private:
  pid_t current_process_;
  bool previous_command_failed_;
};

#endif
