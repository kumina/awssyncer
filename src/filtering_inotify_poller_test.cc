#include "filtering_inotify_poller.h"
#include "inotify_poller.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <experimental/optional>
#include <string>

using ::testing::Return;

namespace {
class MockInotifyPoller : public InotifyPoller {
 public:
  MOCK_METHOD1(AddWatch, bool(const std::string& path));
  MOCK_METHOD0(GetNextEvent, std::experimental::optional<InotifyEvent>());
  MOCK_METHOD0(EventsDropped, bool());
};
}

TEST(FilteringInotifyPollerTest, AddWatch) {
  // We shouldn't be allowed to install watches on matching paths.
  MockInotifyPoller mip;
  EXPECT_CALL(mip, AddWatch("/some/other/path/1")).WillOnce(Return(true));
  EXPECT_CALL(mip, AddWatch("/some/other/path/2")).WillOnce(Return(false));

  FilteringInotifyPoller fip(&mip, "matching");
  ASSERT_FALSE(fip.AddWatch("/matching/path/"));
  ASSERT_TRUE(fip.AddWatch("/some/other/path/1"));
  ASSERT_FALSE(fip.AddWatch("/some/other/path/2"));
}

TEST(FilteringInotifyPollerTest, GetNextEvent) {
  // We should return the second event, as the first one is matched.
  InotifyEvent event1, event2;
  event1.path = "/matching/path";
  event2.path = "/some/other/path";
  MockInotifyPoller mip;
  EXPECT_CALL(mip, GetNextEvent())
      .WillOnce(Return(event1))
      .WillOnce(Return(event2));

  FilteringInotifyPoller fip(&mip, "matching");
  std::experimental::optional<InotifyEvent> event = fip.GetNextEvent();
  ASSERT_TRUE(event);
  ASSERT_EQ("/some/other/path", event->path);
}

TEST(FilteringInotifyPollerTest, EventsDropped) {
  // Test propagation of EventsDropped() calls.
  MockInotifyPoller mip;
  EXPECT_CALL(mip, EventsDropped())
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  FilteringInotifyPoller fip(&mip, "matching");
  ASSERT_FALSE(fip.EventsDropped());
  ASSERT_TRUE(fip.EventsDropped());
}
