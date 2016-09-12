// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "aws_command_runner.h"
#include "multiple_command_runner.h"

#include <cassert>

bool AwsCommandRunner::Finished() {
  return command_runner_->Finished();
}

bool AwsCommandRunner::PreviousCommandFailed() {
  return command_runner_->PreviousCommandFailed();
}

void AwsCommandRunner::CopyFile(const std::string& path) {
  std::string full_url = GetFullUrl(path);
  command_runner_->RunCommand({"aws", "s3", "rm", "--recursive", full_url},
                              false);
  command_runner_->RunCommand({"aws", "s3", "cp", path, full_url}, false);
}

void AwsCommandRunner::SyncDirectory(const std::string& path) {
  std::string full_url = GetFullUrl(path);
  command_runner_->RunCommand({"aws", "s3", "rm", full_url}, true);
  command_runner_->RunCommand({"aws", "s3", "sync", path, full_url}, false);
}

void AwsCommandRunner::Remove(const std::string& path) {
  std::string full_url = GetFullUrl(path);
  command_runner_->RunCommand({"aws", "s3", "rm", full_url}, true);
  command_runner_->RunCommand({"aws", "s3", "rm", "--recursive", full_url},
                              false);
}

std::string AwsCommandRunner::GetFullUrl(const std::string& path) {
  size_t strip_length = local_path_.size() - 1;
  assert(path.compare(0, strip_length, local_path_, 0, strip_length) == 0 &&
         "Path does not start with the required prefix");
  return "s3://" + s3_bucket_ + std::string(path, strip_length);
}
