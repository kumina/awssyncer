#include "recursive_inotify_poller.h"

#include <dirent.h>

#include <cstring>

bool RecursiveInotifyPoller::AddWatch(const std::string &path) {
  // Add the path itself.
  if (!ip_->AddWatch(path))
    return false;

  // Recursive add all of its children.
  DIR *d = opendir(path.c_str());
  if (d != nullptr) {
    struct dirent *de;
    while ((de = readdir(d)) != nullptr) {
      if (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 &&
          strcmp(de->d_name, "..") != 0)
        AddWatch(path + de->d_name + '/');
    }
    closedir(d);
  }
  return true;
}

bool RecursiveInotifyPoller::GetNextEvent(InotifyEvent *event) {
  // Fetch next event.
  if (!ip_->GetNextEvent(event))
    return false;

  // Add a new watch in case we've created a new directory.
  if (event->type == InotifyEventType::CREATED &&
      event->path[event->path.size() - 1] == '/')
    AddWatch(event->path);
  return true;
}
