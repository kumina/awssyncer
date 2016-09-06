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

  for (;;) {
    // Handle all inotify events.
    InotifyEvent event;
    while (rip.GetNextEvent(&event))
      dirt.AddDirtyPath(event.path);

    while (dirt.HasDirtyPaths()) {
      std::string s;
      dirt.ExtractDirtyPath(&s);
      std::cout << "Should sync " << s << std::endl;
    }

    // TODO(ed): Call poll() here instead of sleeping for a second.
    std::cout << "End quantum" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
