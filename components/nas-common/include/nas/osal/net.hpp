#pragma once

#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::osal {

struct NetworkInterface {
  std::string name;
  std::string address;
  std::string netmask;
  bool up{false};
};

class INetworkManager {
 public:
  virtual ~INetworkManager() = default;

  virtual Result<std::vector<NetworkInterface>> ListInterfaces() = 0;
  virtual Result<void> SetInterfaceUp(const std::string& name, bool up) = 0;
};

}  // namespace nas::osal
