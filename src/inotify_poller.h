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
  virtual ~InotifyPoller() {}

  // Attaches a watch to a single directory.
  virtual bool AddWatch(const std::string &path) = 0;

  // Extracts the next event from the inotify event queue. Returns false
  // if no events are present.
  virtual bool GetNextEvent(InotifyEvent *event) = 0;
};

#endif
