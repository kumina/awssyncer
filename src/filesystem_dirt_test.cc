#include "filesystem_dirt.h"

#include <gtest/gtest.h>

TEST(FilesystemDirtTest, example) {
  // There shouldn't be any dirt initially.
  FilesystemDirt dirt;
  ASSERT_FALSE(dirt.HasDirtyPaths());

  // Attempt to store a single path.
  {
    dirt.AddDirtyPath("/a/b");
    ASSERT_TRUE(dirt.HasDirtyPaths());

    ASSERT_EQ("/a/b", dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.HasDirtyPaths());
  }

  // Dirtying /a should discard information about /a/b.
  {
    dirt.AddDirtyPath("/a/b");
    ASSERT_TRUE(dirt.HasDirtyPaths());
    dirt.AddDirtyPath("/a");
    ASSERT_TRUE(dirt.HasDirtyPaths());

    ASSERT_EQ("/a", dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.HasDirtyPaths());
  }

  // Dirtying /a/b should not have any effect if /a is dirty.
  {
    dirt.AddDirtyPath("/a");
    ASSERT_TRUE(dirt.HasDirtyPaths());
    dirt.AddDirtyPath("/a/b");
    ASSERT_TRUE(dirt.HasDirtyPaths());

    ASSERT_EQ("/a", dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.HasDirtyPaths());
  }
}
