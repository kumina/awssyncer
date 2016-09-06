#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include <string>
#include <vector>

// Utility class for running shell commands.
class CommandRunner {
 public:
  CommandRunner() : current_process_(-1), previous_command_failed_(false) {}

  // Returns true of there is no command currently running.
  bool Finished();

  // Returns true if the previous command did not complete successfully.
  bool PreviousCommandFailed();

  // Runs a command by specifying a list of command line arguments.
  void RunCommand(const std::vector<std::string> &command);

 private:
  pid_t current_process_;
  bool previous_command_failed_;
};

#endif
