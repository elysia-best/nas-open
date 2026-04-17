#include "nas/monitor/hw_inventory.hpp"

namespace nas::monitor {

std::vector<HwItem> HardwareInventory::Collect() const {
  return {
      HwItem{.name = "cpu", .value = std::nullopt},
      HwItem{.name = "memory", .value = std::nullopt},
      HwItem{.name = "nic", .value = std::nullopt},
      HwItem{.name = "disk", .value = std::nullopt},
  };
}

}  // namespace nas::monitor
