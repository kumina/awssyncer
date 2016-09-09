#include "filesystem_dirt.h"

#include <cassert>
#include <experimental/optional>

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

std::experimental::optional<std::string> FilesystemDirt::ExtractDirtyPath() {
  assert((!dirty_ || children_.empty()) &&
         "Dirtying should always remove children");
  if (dirty_) {
    // End of search: path is dirty. Return "", so that the pathname
    // components leading up to it can be prepended.
    dirty_ = false;
    return std::string("");
  } else if (!children_.empty()) {
    // Get child that is dirty and prepend the pathname compoment. Reap
    // the child in case its dirty paths are exhausted.
    auto child = children_.begin();
    std::string path("/" + child->first + *child->second.ExtractDirtyPath());
    if (!child->second.dirty_ && child->second.children_.empty())
      children_.erase(child);
    return path;
  } else {
    // There are no dirty paths.
    return {};
  }
}
