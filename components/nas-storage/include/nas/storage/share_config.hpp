#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class ShareConfig {
 public:
  [[nodiscard]] Result<std::string> RenderNfsExports(const std::string& path,
                                                     bool dry_run) const;
  [[nodiscard]] Result<std::string> RenderSambaConfig(const std::string& share_name,
                                                      bool dry_run) const;
};

}  // namespace nas::storage
