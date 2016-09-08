#include "filesystem_dirt.h"

#include <cassert>

void FilesystemDirt::AddDirtyPath(const std::string& path) {
  if (!dirty_) {
    if (path.empty()) {
      // No further pathname components. Mark this file dirty.
      dirty_ = true;
      children_.clear();
    } else {
      // Extract first pathname component from input path.
      size_t nonslash = path.find_first_not_of('/');
      size_t slash = path.find('/', nonslash);
      std::string first(path, nonslash, slash - nonslash);
      std::string remainder;
      if (slash != std::string::npos)
        remainder = std::string(path, slash + 1);

      // Continue to next pathname component.
      auto child = children_.insert({first, FilesystemDirt()});
      child.first->second.AddDirtyPath(remainder);
    }
  }
}

bool FilesystemDirt::HasDirtyPaths() {
  assert((!dirty_ || children_.empty()) &&
         "Dirtying should always remove children");
  return dirty_ || !children_.empty();
}

std::string FilesystemDirt::ExtractDirtyPath() {
  if (dirty_) {
    // End of search: path is dirty.
    dirty_ = false;
    return "";
  } else {
    // Get child that is dirty.
    auto child = children_.begin();
    std::string path("/" + child->first + child->second.ExtractDirtyPath());
    if (!child->second.HasDirtyPaths())
      children_.erase(child);
    return path;
  }
}
