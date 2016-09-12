// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "recursive_inotify_poller.h"

#include <dirent.h>

#include <cstring>
#include <experimental/optional>

bool RecursiveInotifyPoller::AddWatch(const std::string& path) {
  // Add the path itself.
  if (!ip_->AddWatch(path))
    return false;

  // Recursive add all of its children.
  DIR* d = opendir(path.c_str());
  if (d != nullptr) {
    struct dirent* de;
    while ((de = readdir(d)) != nullptr) {
      if (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 &&
          strcmp(de->d_name, "..") != 0)
        AddWatch(path + de->d_name + '/');
    }
    closedir(d);
  }
  return true;
}

std::experimental::optional<InotifyEvent>
RecursiveInotifyPoller::GetNextEvent() {
  // Add a new watch in case we've created a new directory.
  std::experimental::optional<InotifyEvent> event = ip_->GetNextEvent();
  if (event && event->type == InotifyEventType::CREATED &&
      event->path[event->path.size() - 1] == '/')
    AddWatch(event->path);
  return event;
}
