#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::osal {

struct FileEntry {
  std::string path;
  bool is_directory{false};
  std::uint64_t size_bytes{0};
};

class IFileSystem {
 public:
  virtual ~IFileSystem() = default;

  virtual Result<std::vector<FileEntry>> List(const std::string& root) = 0;
  virtual Result<std::string> ReadAll(const std::string& path) = 0;
  virtual Result<void> WriteAll(const std::string& path, const std::string& data) = 0;
};

}  // namespace nas::osal
