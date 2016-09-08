#ifndef AWS_COMMAND_RUNNER_H
#define AWS_COMMAND_RUNNER_H

#include <string>

class CommandRunner;

class AwsCommandRunner {
 public:
  AwsCommandRunner(CommandRunner *command_runner, const std::string &url)
      : command_runner_(command_runner), url_(url) {}

  // Returns true of there is no command currently running.
  bool Finished();

  // Returns true if the previous command did not complete successfully.
  bool PreviousCommandFailed();

  // Runs an AWS S3 syncer process.
  void RunSync(const std::string &path);

  // Runs an AWS S3 remover process.
  void RunRemove(const std::string &path);

 private:
  CommandRunner *command_runner_;
  std::string url_;
};

#endif
