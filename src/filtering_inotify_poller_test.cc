#include "filtering_inotify_poller.h"
#include "inotify_poller.h"

#include <gtest/gtest.h>

namespace {

// TODO(ed): Rewrite this to use gmock.h.
class FixedResponseInotifyPoller : public InotifyPoller {
 public:
  FixedResponseInotifyPoller() : done_(false) {
  }

  bool AddWatch(const std::string& path) override {
    EXPECT_EQ("/some/other/path", path);
    return true;
  }

  bool GetNextEvent(InotifyEvent* event) {
    event->path = done_ ? "/some/other/path" : "/matching/path/";
    done_ = true;
    return true;
  }

  bool EventsDropped() {
    return false;
  }

 private:
  bool done_;
};
}

TEST(FilteringInotifyPollerTest, AddWatch) {
  // We shouldn't be allowed to install watches on matching paths.
  FixedResponseInotifyPoller frip;
  FilteringInotifyPoller fip(&frip, "matching");
  ASSERT_FALSE(fip.AddWatch("/matching/path/"));
  ASSERT_TRUE(fip.AddWatch("/some/other/path"));
}

TEST(FilteringInotifyPollerTest, GetNextEvent_match) {
  // We should return the second event, as we have matches.
  FixedResponseInotifyPoller frip;
  FilteringInotifyPoller fip(&frip, "matching");
  InotifyEvent event;
  ASSERT_TRUE(fip.GetNextEvent(&event));
  ASSERT_EQ("/some/other/path", event.path);
}

TEST(FilteringInotifyPollerTest, GetNextEvent_nomatch) {
  // We should return the first event, as there is no match.
  FixedResponseInotifyPoller frip;
  FilteringInotifyPoller fip(&frip, "not_present");
  InotifyEvent event;
  ASSERT_TRUE(fip.GetNextEvent(&event));
  ASSERT_EQ("/matching/path/", event.path);
}
