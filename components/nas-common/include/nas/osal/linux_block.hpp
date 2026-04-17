#pragma once

#include "nas/osal/block.hpp"

namespace nas::osal {

class LinuxBlockDeviceManager final : public IBlockDeviceManager {
 public:
  Result<std::vector<BlockDevice>> Enumerate() override;
  Result<void> Rescan() override;
};

}  // namespace nas::osal
