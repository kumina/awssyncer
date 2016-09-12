// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "filesystem_dirt.h"

#include <gtest/gtest.h>

TEST(FilesystemDirtTest, example) {
  FilesystemDirt dirt;

  // Attempt to store a single path.
  {
    dirt.AddDirtyPath("/a/b");
    ASSERT_EQ("/a/b", *dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.ExtractDirtyPath());
  }

  // Dirtying /a should discard information about /a/b.
  {
    dirt.AddDirtyPath("/a/b");
    dirt.AddDirtyPath("/a");

    ASSERT_EQ("/a", *dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.ExtractDirtyPath());
  }

  // Dirtying /a/b should not have any effect if /a is dirty.
  {
    dirt.AddDirtyPath("/a");
    dirt.AddDirtyPath("/a/b");

    ASSERT_EQ("/a", *dirt.ExtractDirtyPath());
    ASSERT_FALSE(dirt.ExtractDirtyPath());
  }
}
