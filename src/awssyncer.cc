#include "inotify_poller.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  InotifyPoller ip;
  bool res = ip.AddWatch("/tmp");
  assert(res);
  for (;;) {
    InotifyEvent event;
    while (!ip.GetNextEvent(&event)) {
      std::cout << "Didn't get any events" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << event.path << std::endl;
  }
}
