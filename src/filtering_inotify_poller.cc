#include "filtering_inotify_poller.h"

#include <regex.h>

#include <cassert>

FilteringInotifyPoller::FilteringInotifyPoller(InotifyPoller *ip,
                                               const std::string &regex)
   : ip_(ip) {
  int ret = regcomp(&regex_, regex.c_str(), REG_EXTENDED | REG_NOSUB);
  assert(ret == 0 && "Invalid regular expression");
}

bool FilteringInotifyPoller::AddWatch(const std::string &path) {
  return !MatchPath(path) && ip_->AddWatch(path);
}

bool FilteringInotifyPoller::GetNextEvent(InotifyEvent *event) {
  while (ip_->GetNextEvent(event)) {
    if (!MatchPath(event->path))
      return true;
  }
  return false;
}

bool FilteringInotifyPoller::MatchPath(const std::string &path) {
  return regexec(&regex_, path.c_str(), 0, NULL, 0) == 0;
}
