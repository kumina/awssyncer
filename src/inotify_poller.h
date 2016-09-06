#ifndef INOTIFY_POLLER_H
#define INOTIFY_POLLER_H

#include <map>
#include <string>

// Type of filesystem event that has occurred.
enum class InotifyEventType { CREATED, MODIFIED, DELETED };

// A filesystem event.
struct InotifyEvent {
  InotifyEventType type;  // Type of event.
  std::string path;       // Path to which the event applied.
};

// Poller of inotify events.
class InotifyPoller {
 public:
  InotifyPoller();

  // Attaches a watch to a single directory.
  bool AddWatch(const std::string &path);

  // Extracts the next event from the inotify event queue. Returns false
  // if no events are present.
  bool GetNextEvent(InotifyEvent *event);

 private:
  int fd_;
  std::map<int, std::string> directories_;

  char read_buffer_[65536];
  size_t read_buffer_length_;
  size_t read_buffer_offset_;
};

#endif
