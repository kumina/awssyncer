#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include <string>
#include <vector>

// Utility class for running shell commands.
class CommandRunner {
 public:
  virtual ~CommandRunner() {}

  // Returns true of there is no command currently running.
  virtual bool Finished() = 0;

  // Returns true if the previous command did not complete successfully.
  virtual bool PreviousCommandFailed() = 0;

  // Runs a command by specifying a list of command line arguments.
  virtual void RunCommand(const std::vector<std::string> &command) = 0;
};

#endif
