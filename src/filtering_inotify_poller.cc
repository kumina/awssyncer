#include "filtering_inotify_poller.h"

#include <cassert>
#include <experimental/optional>
#include <regex>

FilteringInotifyPoller::FilteringInotifyPoller(InotifyPoller* ip,
                                               const std::string& regex)
    : ip_(ip), regex_(regex, std::regex::extended | std::regex::nosubs) {
}

bool FilteringInotifyPoller::AddWatch(const std::string& path) {
  // Don't allow installing watches if the pathname matches.
  return !std::regex_search(path, regex_) && ip_->AddWatch(path);
}

std::experimental::optional<InotifyEvent>
FilteringInotifyPoller::GetNextEvent() {
  // Skip all events that have matching pathnames.
  std::experimental::optional<InotifyEvent> event;
  while ((event = ip_->GetNextEvent()) &&
         std::regex_search(event->path, regex_))
    ;
  return event;
}
