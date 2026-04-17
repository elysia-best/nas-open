#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class IscsiTarget {
 public:
  [[nodiscard]] Result<std::string> MapLun(const std::string& iqn, const std::string& lun,
                                           bool dry_run) const;
};

}  // namespace nas::storage
