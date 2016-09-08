#ifndef AWS_COMMAND_RUNNER_H
#define AWS_COMMAND_RUNNER_H

#include <string>
#include <vector>

class MultipleCommandRunner;

class AwsCommandRunner {
 public:
  AwsCommandRunner(MultipleCommandRunner *command_runner,
                   const std::string &url)
      : command_runner_(command_runner), url_(url) {}

  // Returns true of there is no command currently running.
  bool Finished();

  // Returns true if the previous command did not complete successfully.
  bool PreviousCommandFailed();

  // Runs an AWS S3 file copier process.
  void CopyFile(const std::string &path);

  // Runs an AWS S3 directory syncer process.
  void SyncDirectory(const std::string &path);

  // Runs an AWS S3 remover process.
  void Remove(const std::string &path);

 private:
  MultipleCommandRunner *command_runner_;
  std::string url_;
};

#endif
