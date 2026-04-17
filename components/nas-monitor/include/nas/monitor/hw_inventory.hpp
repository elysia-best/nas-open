#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nas::monitor {

struct HwItem {
  std::string name;
  std::optional<std::string> value;
};

class HardwareInventory {
 public:
  [[nodiscard]] std::vector<HwItem> Collect() const;
};

}  // namespace nas::monitor
