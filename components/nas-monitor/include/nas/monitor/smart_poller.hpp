#pragma once

#include <optional>
#include <string>

#include "nas/error.hpp"

namespace nas::monitor {

struct SmartStatus {
  std::string device;
  std::string health;
  std::optional<int> temperature_c;
};

class SmartPoller {
 public:
  [[nodiscard]] Result<SmartStatus> Poll(const std::string& device) const;
};

}  // namespace nas::monitor
