#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::osal {

struct BlockDevice {
  std::string name;
  std::string model;
  std::uint64_t size_bytes{0};
  bool removable{false};
};

class IBlockDeviceManager {
 public:
  virtual ~IBlockDeviceManager() = default;

  virtual Result<std::vector<BlockDevice>> Enumerate() = 0;
  virtual Result<void> Rescan() = 0;
};

}  // namespace nas::osal
