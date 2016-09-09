#include "filtering_inotify_poller.h"

#include <cassert>
#include <regex>

FilteringInotifyPoller::FilteringInotifyPoller(InotifyPoller* ip,
                                               const std::string& regex)
    : ip_(ip), regex_(regex, std::regex::extended | std::regex::nosubs) {
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
  return std::regex_search(path, regex_);
}
