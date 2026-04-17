#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class PartitionManager {
 public:
  [[nodiscard]] Result<std::string> ProbeTopologyJson(bool dry_run) const;
};

}  // namespace nas::storage
