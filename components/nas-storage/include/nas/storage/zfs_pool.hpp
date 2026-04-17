#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class ZfsPool {
 public:
  [[nodiscard]] Result<std::string> ListDatasets(bool dry_run) const;
  [[nodiscard]] Result<std::string> Snapshot(const std::string& dataset, const std::string& snap,
                                             bool dry_run) const;
};

}  // namespace nas::storage
