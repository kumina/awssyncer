// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "posix_command_runner.h"

#include <sys/wait.h>

#include <signal.h>
#include <spawn.h>

#include <cassert>

extern char** environ;

PosixCommandRunner::~PosixCommandRunner() {
  // Kill the child process.
  if (current_process_ >= 0) {
    kill(current_process_, SIGTERM);
    waitpid(current_process_, nullptr, 0);
  }
}

bool PosixCommandRunner::Finished() {
  if (current_process_ >= 0) {
    // Command may still be running. Ask the kernel for the exit status.
    int stat;
    pid_t res = waitpid(current_process_, &stat, WNOHANG);
    assert((res == 0 || res == current_process_) && "Unexpected return value");
    if (res == 0)
      return false;

    // Command has just terminated. Store whether it has failed.
    current_process_ = -1;
    previous_command_failed_ = !WIFEXITED(stat) || WEXITSTATUS(stat) != 0;
  }
  return true;
}

bool PosixCommandRunner::PreviousCommandFailed() {
  assert(current_process_ == -1 && "Previous command is still running");
  return previous_command_failed_;
}

void PosixCommandRunner::RunCommand(const std::vector<std::string>& command) {
  assert(current_process_ == -1 && "Previous command is still running");

  // Convert argument strings to char *, as expected by posix_spawnp().
  std::vector<char*> args;
  for (const std::string& arg : command)
    args.push_back(const_cast<char*>(arg.c_str()));
  args.push_back(nullptr);

  // Spawn subprocess.
  if (posix_spawnp(&current_process_, command[0].c_str(), nullptr, nullptr,
                   args.data(), environ) != 0) {
    current_process_ = -1;
    previous_command_failed_ = true;
  }
  previous_command_failed_ = false;
}
