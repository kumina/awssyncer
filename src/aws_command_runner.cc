#include "aws_command_runner.h"
#include "multiple_command_runner.h"

bool AwsCommandRunner::Finished() {
  return command_runner_->Finished();
}

bool AwsCommandRunner::PreviousCommandFailed() {
  return command_runner_->PreviousCommandFailed();
}

void AwsCommandRunner::CopyFile(const std::string &path) {
  std::string full_url = url_ + path;
  command_runner_->RunCommand({"aws", "s3", "rm", "--recursive", full_url},
                              false);
  command_runner_->RunCommand({"aws", "s3", "cp", path, full_url}, false);

}

void AwsCommandRunner::SyncDirectory(const std::string &path) {
  std::string full_url = url_ + path;
  command_runner_->RunCommand({"aws", "s3", "rm", full_url}, true);
  command_runner_->RunCommand({"aws", "s3", "sync", path, full_url}, false);
}

void AwsCommandRunner::Remove(const std::string &path) {
  std::string full_url = url_ + path;
  command_runner_->RunCommand({"aws", "s3", "rm", full_url}, true);
  command_runner_->RunCommand({"aws", "s3", "rm", "--recursive", full_url},
                              false);
}
