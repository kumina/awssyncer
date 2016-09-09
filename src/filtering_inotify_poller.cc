#include "filtering_inotify_poller.h"

#include <regex.h>

#include <cassert>

FilteringInotifyPoller::FilteringInotifyPoller(InotifyPoller* ip,
                                               const std::string& regex)
    : ip_(ip) {
  int ret = regcomp(&regex_, regex.c_str(), REG_EXTENDED | REG_NOSUB);
  assert(ret == 0 && "Invalid regular expression");
}

bool FilteringInotifyPoller::AddWatch(const std::string& path) {
  // Don't allow installing watches if the pathname matches.
  return !MatchPath(path) && ip_->AddWatch(path);
}

bool FilteringInotifyPoller::GetNextEvent(InotifyEvent* event) {
  // Skip all events that have matching pathnames.
  while (ip_->GetNextEvent(event)) {
    if (!MatchPath(event->path))
      return true;
  }
  return false;
}

bool FilteringInotifyPoller::MatchPath(const std::string& path) {
  return regexec(&regex_, path.c_str(), 0, nullptr, 0) == 0;
}
