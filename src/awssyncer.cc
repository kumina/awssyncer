// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "aws_command_runner.h"
#include "filesystem_dirt.h"
#include "filtering_inotify_poller.h"
#include "multiple_command_runner.h"
#include "nonrecursive_inotify_poller.h"
#include "posix_command_runner.h"
#include "recursive_inotify_poller.h"

#include <sys/stat.h>

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <experimental/optional>
#include <iostream>
#include <thread>

// Returns a reference to the logging sink, already prefixing the line
// with the current timestamp.
static std::ostream& Log() {
  time_t t = std::time(nullptr);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%FT%T%z: ", std::localtime(&t));
  return std::cout << buf;
}

// Performs a single loop of the syncer, handling inotify events and
// potentially starting a new 'aws s3' process.
static bool ProcessEvents(InotifyPoller* ip, FilesystemDirt* dirt,
                          AwsCommandRunner* runner) {
  // Handle all inotify events.
  std::experimental::optional<InotifyEvent> event;
  while ((event = ip->GetNextEvent()))
    dirt->AddDirtyPath(event->path);
  if (ip->EventsDropped()) {
    Log() << "Inotify dropped events" << std::endl;
    return false;
  }

  // Potentially spawn another sync command.
  if (runner->Finished()) {
    if (runner->PreviousCommandFailed()) {
      Log() << "Failed to execute command" << std::endl;
      return false;
    }
    std::experimental::optional<std::string> path = dirt->ExtractDirtyPath();
    if (path) {
      Log() << "Processing path " << *path << std::endl;
      struct stat sb;
      if (lstat(path->c_str(), &sb) == 0) {
        // Only sync files and directories. AWS doesn't support other types.
        if (S_ISDIR(sb.st_mode))
          runner->SyncDirectory(*path);
        else if (S_ISREG(sb.st_mode))
          runner->CopyFile(*path);
      } else if (errno == ENOENT) {
        runner->Remove(*path);
      } else {
        Log() << "Failed to stat " << *path << std::endl;
        return false;
      }
    }
  }
  return true;
}

// Instantiates all objects needed for syncing and handles events until
// an error has occurred.
static void RunSyncer(const std::string& local_path,
                      const std::string& s3_bucket,
                      const std::string& filter_regex) {
  // Set up inotify polling.
  NonrecursiveInotifyPoller nip;
  FilteringInotifyPoller fip(&nip, filter_regex);
  RecursiveInotifyPoller rip(&fip);
  bool res = rip.AddWatch(local_path);
  assert(res);

  // Start off marking the entire file system as dirty. This forces an initial
  // synchronisation of all of the data.
  FilesystemDirt dirt;
  dirt.AddDirtyPath(local_path);

  // Objects for running the AWS command line tool sequentially.
  PosixCommandRunner posix_runner;
  MultipleCommandRunner multiple_command_runner(&posix_runner);
  AwsCommandRunner runner(&multiple_command_runner, local_path, s3_bucket);

  while (ProcessEvents(&rip, &dirt, &runner)) {
    // TODO(ed): Call poll() here instead of sleeping for a second.
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main() {
  // Obtain configuration from environment variables.
  std::string local_path = std::getenv("LOCAL_PATH");
  std::string s3_bucket = std::getenv("S3_BUCKET");
  std::string filter_regex = std::getenv("FILTER_REGEX");

  // Keep on running indefinitely, restarting if an error occurred. Put a pause
  // of a couple of seconds in between, so that we never perform any actions in
  // a tight loop.
  for (;;) {
    RunSyncer(local_path, s3_bucket, filter_regex);
    Log() << "Restarting in five seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}
