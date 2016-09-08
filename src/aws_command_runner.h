#ifndef AWS_COMMAND_RUNNER_H
#define AWS_COMMAND_RUNNER_H

#include <string>
#include <vector>

class MultipleCommandRunner;

// Adapter for MultipleCommandrunner to invoke the AWS command line utility.
// This only exposes the operations in which we're interested.
class AwsCommandRunner {
 public:
  AwsCommandRunner(MultipleCommandRunner *command_runner,
                   const std::string &local_path, const std::string& s3_bucket)
      : command_runner_(command_runner),
        local_path_(local_path), s3_bucket_(s3_bucket) {}

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
  // Underlying command runner.
  MultipleCommandRunner *command_runner_;

  // Path prefix to strip from local paths.
  std::string local_path_;

  // S3 bucket name, e.g. "mybucket".
  std::string s3_bucket_;

  // Translates a local path to a full URL to the S3 bucket with the
  // pathname prefix stripped off.
  std::string GetFullUrl(const std::string& path);
};

#endif
