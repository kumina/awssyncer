// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "aws_command_runner.h"
#include "command_runner.h"
#include "multiple_command_runner.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

namespace {
class MockCommandRunner : public CommandRunner {
 public:
  MOCK_METHOD0(Finished, bool());
  MOCK_METHOD0(PreviousCommandFailed, bool());
  MOCK_METHOD1(RunCommand, void(const std::vector<std::string>& command));
};
}

TEST(AwsCommandRunner, Finished) {
  MockCommandRunner command_runner;
  MultipleCommandRunner multiple_command_runner(&command_runner);
  EXPECT_CALL(command_runner, Finished())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .WillRepeatedly(Return(false));

  AwsCommandRunner aws_command_runner(&multiple_command_runner, "/tmp/",
                                      "unused", {});
  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_TRUE(aws_command_runner.Finished());
}

TEST(AwsCommandRunner, PreviousCommandFailed) {
  MockCommandRunner command_runner;
  MultipleCommandRunner multiple_command_runner(&command_runner);
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  AwsCommandRunner aws_command_runner(&multiple_command_runner, "/tmp/",
                                      "unused", {});
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_TRUE(aws_command_runner.PreviousCommandFailed());
}

TEST(AwsCommandRunner, CopyFile) {
  MockCommandRunner command_runner;
  MultipleCommandRunner multiple_command_runner(&command_runner);
  EXPECT_CALL(command_runner,
              RunCommand(std::vector<std::string>({
                  "aws", "s3", "rm", "--recursive", "s3://bucket/foo",
              })))
      .Times(1);
  EXPECT_CALL(command_runner,
              RunCommand(std::vector<std::string>({
                  "aws", "s3", "cp", "/tmp/foo", "s3://bucket/foo",
              })))
      .Times(1);
  EXPECT_CALL(command_runner, Finished()).WillRepeatedly(Return(true));
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .WillRepeatedly(Return(false));

  AwsCommandRunner aws_command_runner(&multiple_command_runner, "/tmp/",
                                      "bucket", {});
  aws_command_runner.CopyFile("/tmp/foo");

  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_TRUE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
}

TEST(AwsCommandRunner, SyncDirectory) {
  MockCommandRunner command_runner;
  MultipleCommandRunner multiple_command_runner(&command_runner);
  EXPECT_CALL(command_runner, RunCommand(std::vector<std::string>({
                                  "aws", "s3", "rm", "s3://bucket/foo",
                              })))
      .Times(1);
  EXPECT_CALL(command_runner,
              RunCommand(std::vector<std::string>({
                  "aws", "s3", "sync", "--delete", "--exclude", "*.jpg",
                  "--exclude", "*.mp3", "/tmp/foo", "s3://bucket/foo",
              })))
      .Times(1);
  EXPECT_CALL(command_runner, Finished()).WillRepeatedly(Return(true));
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .WillRepeatedly(Return(false));

  AwsCommandRunner aws_command_runner(&multiple_command_runner, "/tmp/",
                                      "bucket", {"*.jpg", "*.mp3"});
  aws_command_runner.SyncDirectory("/tmp/foo");

  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_TRUE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
}

TEST(AwsCommandRunner, Remove) {
  MockCommandRunner command_runner;
  MultipleCommandRunner multiple_command_runner(&command_runner);
  EXPECT_CALL(command_runner, RunCommand(std::vector<std::string>({
                                  "aws", "s3", "rm", "s3://bucket/foo",
                              })))
      .Times(1);
  EXPECT_CALL(command_runner,
              RunCommand(std::vector<std::string>({
                  "aws", "s3", "rm", "--recursive", "s3://bucket/foo",
              })))
      .Times(1);
  EXPECT_CALL(command_runner, Finished()).WillRepeatedly(Return(true));
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .WillRepeatedly(Return(false));

  AwsCommandRunner aws_command_runner(&multiple_command_runner, "/tmp/",
                                      "bucket", {});
  aws_command_runner.Remove("/tmp/foo");

  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_TRUE(aws_command_runner.Finished());
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
}
