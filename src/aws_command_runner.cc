#include "aws_command_runner.h"
#include "command_runner.h"

bool AwsCommandRunner::Finished() {
  return command_runner_->Finished();
}

bool AwsCommandRunner::PreviousCommandFailed() {
  return command_runner_->PreviousCommandFailed();
}

void AwsCommandRunner::RunSync(const std::string &path) {
  return command_runner_->RunCommand({
      "aws", "s3", "sync", path, url_ + path,
  });
}

void AwsCommandRunner::RunRemove(const std::string &path) {
  return command_runner_->RunCommand({
      "aws", "s3", "rm", "--recursive", url_ + path,
  });
}
