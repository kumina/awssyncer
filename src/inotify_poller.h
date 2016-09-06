#ifndef INOTIFY_POLLER_H
#define INOTIFY_POLLER_H

#include <map>
#include <string>

struct InotifyEvent {
  std::string path;
};

class InotifyPoller {
 public:
  InotifyPoller();

  bool AddWatch(const std::string &path);
  void RemoveWatch(const std::string &path);

  bool GetNextEvent(InotifyEvent *);

 private:
  int fd_;
  std::map<int, std::string> directories_;

  char read_buffer_[65536];
  size_t read_buffer_length_;
  size_t read_buffer_offset_;
};

#endif
