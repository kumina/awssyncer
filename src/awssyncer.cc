// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "aws_command_runner.h"
#include "filesystem_dirt.h"
#include "filtering_inotify_poller.h"
#include "multiple_command_runner.h"
#include "nonrecursive_inotify_poller.h"
#include "periodic_filesystem_dirtier.h"
#include "posix_command_runner.h"
#include "recursive_inotify_poller.h"

#include <sys/stat.h>

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <experimental/optional>
#include <iomanip>
#include <iostream>
#include <set>
#include <thread>

// Returns a reference to the logging sink, already prefixing the line
// with the current timestamp.
static std::ostream& Log() {
  time_t t = std::time(nullptr);
  return std::cout << std::put_time(std::localtime(&t), "%FT%T%z: ");
}

// Performs a single loop of the syncer, handling inotify events and
// potentially starting a new 'aws s3' process.
static bool ProcessEvents(InotifyPoller* ip, FilesystemDirt* dirt,
                          PeriodicFilesystemDirtier* periodic_dirtier,
                          AwsCommandRunner* runner) {
  // Handle all inotify events.
  std::experimental::optional<InotifyEvent> event;
  while ((event = ip->GetNextEvent()))
    dirt->AddDirtyPath(event->path);
  if (ip->EventsDropped()) {
    Log() << "Inotify dropped events" << std::endl;
    return false;
  }

  // Periodically force dirtying of certain directories to really ensure
  // we're not missing out on anything.
  periodic_dirtier->Tick();

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
                      const std::string& filter_regex,
                      const std::set<std::string>& sync_excludes) {
  // Set up inotify polling.
  NonrecursiveInotifyPoller nip;
  FilteringInotifyPoller fip(&nip, filter_regex);
  RecursiveInotifyPoller rip(&fip);
  bool res = rip.AddWatch(local_path);
  assert(res);

  // Data structure for keeping track of filesystem dirtiness. Force a
  // full sync of the data every six hours.
  FilesystemDirt dirt;
  PeriodicFilesystemDirtier periodic_dirtier(&dirt, local_path,
                                             std::chrono::hours(6));

  // Objects for running the AWS command line tool sequentially.
  PosixCommandRunner posix_runner;
  MultipleCommandRunner multiple_command_runner(&posix_runner);
  AwsCommandRunner runner(&multiple_command_runner, local_path, s3_bucket,
                          sync_excludes);

  while (ProcessEvents(&rip, &dirt, &periodic_dirtier, &runner)) {
    // TODO(ed): Call poll() here instead of sleeping for a second.
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main() {
  // Obtain configuration from environment variables.
  std::string local_path = std::getenv("LOCAL_PATH");
  std::string s3_bucket = std::getenv("S3_BUCKET");
  std::string filter_regex = std::getenv("FILTER_REGEX");
  std::string sync_excludes_str = std::getenv("SYNC_EXCLUDES");

  // Convert single string of globs into a list by splitting on the '|',
  // for example: "*.jpg|*.mp3" -> {"*.jpg", "*.mp3"}.
  std::set<std::string> sync_excludes;
  for (;;) {
    std::size_t split = sync_excludes_str.find('|');
    if (split != std::string::npos) {
      sync_excludes.insert(sync_excludes_str.substr(0, split));
      sync_excludes_str.erase(0, split + 1);
    } else {
      sync_excludes.insert(sync_excludes_str);
      break;
    }
  }

  // Keep on running indefinitely, restarting if an error occurred. Put a pause
  // of a couple of seconds in between, so that we never perform any actions in
  // a tight loop.
  for (;;) {
    RunSyncer(local_path, s3_bucket, filter_regex, sync_excludes);
    Log() << "Restarting in five seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}
