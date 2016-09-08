#include "aws_command_runner.h"
#include "command_runner.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

namespace {

class MockCommandRunner : public CommandRunner {
 public:
  MOCK_METHOD0(Finished, bool());
  MOCK_METHOD0(PreviousCommandFailed, bool());
  MOCK_METHOD1(RunCommand, void(const std::vector<std::string> &command));
};

}

TEST(AwsCommandRunner, Finished) {
  MockCommandRunner command_runner;
  EXPECT_CALL(command_runner, Finished())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  AwsCommandRunner aws_command_runner(&command_runner, "s3://unused");
  ASSERT_FALSE(aws_command_runner.Finished());
  ASSERT_TRUE(aws_command_runner.Finished());
}

TEST(AwsCommandRunner, PreviousCommandFailed) {
  MockCommandRunner command_runner;
  EXPECT_CALL(command_runner, PreviousCommandFailed())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  AwsCommandRunner aws_command_runner(&command_runner, "s3://unused");
  ASSERT_FALSE(aws_command_runner.PreviousCommandFailed());
  ASSERT_TRUE(aws_command_runner.PreviousCommandFailed());
}

TEST(AwsCommandRunner, RunSync) {
  MockCommandRunner command_runner;
  std::vector<std::string> args{{"aws", "s3", "sync", "/tmp/foo",
                                 "s3://bucket/tmp/foo"}};
  EXPECT_CALL(command_runner, RunCommand(args))
      .Times(1);

  AwsCommandRunner aws_command_runner(&command_runner, "s3://bucket");
  aws_command_runner.RunSync("/tmp/foo");
}

TEST(AwsCommandRunner, RunRemove) {
  MockCommandRunner command_runner;
  std::vector<std::string> args{{"aws", "s3", "rm", "--recursive",
                                 "s3://bucket/tmp/foo"}};
  EXPECT_CALL(command_runner, RunCommand(args))
      .Times(1);

  AwsCommandRunner aws_command_runner(&command_runner, "s3://bucket");
  aws_command_runner.RunRemove("/tmp/foo");
}
