#pragma once

#include "nas/osal/net.hpp"

namespace nas::osal {

class LinuxNetworkManager final : public INetworkManager {
 public:
  Result<std::vector<NetworkInterface>> ListInterfaces() override;
  Result<void> SetInterfaceUp(const std::string& name, bool up) override;
};

}  // namespace nas::osal
