#include "inotify_poller.h"

#include <cassert>
#include <iostream>

int main() {
  InotifyPoller ip;
  bool res = ip.AddWatch("/tmp");
  assert(res);
  for (;;) {
    InotifyEvent event;
    res = ip.GetNextEvent(&event);
    assert(res);
    std::cout << event.path << std::endl;
  }
}
