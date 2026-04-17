#pragma once

#include "nas/osal/fs.hpp"

namespace nas::osal {

class PosixFileSystem final : public IFileSystem {
 public:
  Result<std::vector<FileEntry>> List(const std::string& root) override;
  Result<std::string> ReadAll(const std::string& path) override;
  Result<void> WriteAll(const std::string& path, const std::string& data) override;
};

}  // namespace nas::osal
