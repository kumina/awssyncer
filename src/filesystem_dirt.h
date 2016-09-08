#ifndef FILESYSTEM_DIRT_H
#define FILESYSTEM_DIRT_H

#include <map>
#include <string>

// Class for keeping track of dirty paths.
//
// With inotify we keep track of which directories are dirty. As soon as a
// directory is marked dirty, there is no longer any need to keep track of
// dirtiness of any files stored within. This object accomplishes this by
// storing dirtiness information per pathname component, discarding subtrees
// where needed.
class FilesystemDirt {
 public:
  FilesystemDirt() : dirty_(false) {}

  // Marks a file dirty.
  void AddDirtyPath(const std::string& path);

  // Is ExtractDirtyPath() going to return anything meaningful?
  bool HasDirtyPaths();

  // Extract a pathname of a file or directory that is dirty.
  std::string ExtractDirtyPath();

 private:
  // Is this path dirty?
  bool dirty_;

  // If not, are there any files/directories stored within that are dirty or
  // contain dirty files?
  std::map<std::string, FilesystemDirt> children_;
};

#endif
