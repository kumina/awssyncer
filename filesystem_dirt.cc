#include "filesystem_dirt.h"

#include <cassert>

void FilesystemDirt::AddDirtyPath(const std::string& path) {
  if (!dirty_) {
    if (path.empty()) {
      dirty_ = true;
      children_.clear();
    } else {
      size_t nonslash = path.find_first_not_of('/');
      size_t slash = path.find('/', nonslash);
      std::string first(path, nonslash, slash - nonslash);
      std::string remainder;
      if (slash != std::string::npos)
        remainder = std::string(path, slash + 1);

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

void FilesystemDirt::ExtractDirtyPath(std::string *path) {
  if (dirty_) {
    dirty_ = false;
  } else {
    auto child = children_.begin();
    *path += '/';
    *path += child->first;
    child->second.ExtractDirtyPath(path);
    if (!child->second.HasDirtyPaths())
      children_.erase(child);
  }
}
