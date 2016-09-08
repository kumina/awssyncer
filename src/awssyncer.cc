#include "aws_command_runner.h"
#include "filesystem_dirt.h"
#include "multiple_command_runner.h"
#include "nonrecursive_inotify_poller.h"
#include "posix_command_runner.h"
#include "recursive_inotify_poller.h"

#include <sys/stat.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  NonrecursiveInotifyPoller nip;
  RecursiveInotifyPoller rip(&nip);
  bool res = rip.AddWatch("/home/ed/syncdir/");
  assert(res);

  FilesystemDirt dirt;
  PosixCommandRunner posix_runner;
  MultipleCommandRunner multiple_command_runner(&posix_runner);
  AwsCommandRunner runner(&multiple_command_runner, "s3://oreokoekje");

  for (;;) {
    // Handle all inotify events.
    InotifyEvent event;
    while (rip.GetNextEvent(&event))
      dirt.AddDirtyPath(event.path);
    if (nip.EventsDropped()) {
      // TODO(ed): Deal with this.
      std::cout << "DROPPED EVENTS" << std::endl;
    }

    // Potentially spawn another sync command.
    if (runner.Finished()) {
      if (runner.PreviousCommandFailed()) {
        // TODO(ed): Deal with this.
        std::cout << "PREVIOUS COMMAND FAILED" << std::endl;
      }
      if (dirt.HasDirtyPaths()) {
        std::string path = dirt.ExtractDirtyPath();
        std::cout << "PROCESSING PATH " << path << std::endl;
        struct stat sb;
        if (lstat(path.c_str(), &sb) == 0) {
          if (S_ISDIR(sb.st_mode))
            runner.SyncDirectory(path);
          else
            runner.CopyFile(path);
        } else if (errno == ENOENT) {
          runner.Remove(path);
        } else {
          // TODO(ed): Deal with this.
          std::cout << "FAILED TO STAT" << path << std::endl;
        }
      }
    }

    // TODO(ed): Call poll() here instead of sleeping for a second.
    std::cout << "End quantum" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
