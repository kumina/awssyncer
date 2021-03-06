// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef AWS_COMMAND_RUNNER_H
#define AWS_COMMAND_RUNNER_H

#include <set>
#include <string>

class MultipleCommandRunner;

// Adapter for MultipleCommandrunner to invoke the AWS command line utility.
// This only exposes the operations in which we're interested.
class AwsCommandRunner {
 public:
  AwsCommandRunner(MultipleCommandRunner* command_runner,
                   const std::string& local_path, const std::string& s3_bucket,
                   const std::set<std::string>& sync_excludes)
      : command_runner_(command_runner),
        local_path_(local_path),
        s3_bucket_(s3_bucket),
        sync_excludes_(sync_excludes) {
  }

  // Returns true of there is no command currently running.
  bool Finished();

  // Returns true if the previous command did not complete successfully.
  bool PreviousCommandFailed();

  // Runs an AWS S3 file copier process.
  void CopyFile(const std::string& path);

  // Runs an AWS S3 directory syncer process.
  void SyncDirectory(const std::string& path);

  // Runs an AWS S3 remover process.
  void Remove(const std::string& path);

 private:
  // Underlying command runner.
  MultipleCommandRunner* const command_runner_;

  // Path prefix to strip from local paths.
  const std::string local_path_;

  // S3 bucket name, e.g. "mybucket".
  const std::string s3_bucket_;

  // List of globbing patterns that are passed to "aws s3 sync". Each of
  // these is turned into an additional "--excludes" flag.
  const std::set<std::string> sync_excludes_;

  // Translates a local path to a full URL to the S3 bucket with the
  // pathname prefix stripped off.
  std::string GetFullUrl(const std::string& path);
};

#endif
