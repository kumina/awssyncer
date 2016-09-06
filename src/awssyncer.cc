#include "command_runner.h"
#include "filesystem_dirt.h"
#include "nonrecursive_inotify_poller.h"
#include "recursive_inotify_poller.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  NonrecursiveInotifyPoller nip;
  RecursiveInotifyPoller rip(&nip);
  bool res = rip.AddWatch("/tmp/");
  assert(res);

  FilesystemDirt dirt;
  CommandRunner runner;


  for (;;) {
    // Handle all inotify events.
    InotifyEvent event;
    while (rip.GetNextEvent(&event))
      dirt.AddDirtyPath(event.path);
    if (nip.EventsDropped()) {
      // TODO(ed): Deal with this.
    }

    if (runner.Finished()) {
      if (runner.PreviousCommandFailed()) {
        // TODO(ed): Deal with this.
      }
      if (dirt.HasDirtyPaths()) {
        std::string s;
        dirt.ExtractDirtyPath(&s);
        runner.RunCommand({"echo", "--", s});
      }
    }

    // TODO(ed): Call poll() here instead of sleeping for a second.
    std::cout << "End quantum" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
