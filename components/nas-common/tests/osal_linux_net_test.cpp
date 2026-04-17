#include <cassert>

#include "nas/osal/linux_net.hpp"

int main() {
  nas::osal::LinuxNetworkManager mgr;

  auto ifaces = mgr.ListInterfaces();
  assert(ifaces.has_value());
  // At least loopback (127.0.0.1) should be present in any Linux environment
  bool found_lo = false;
  for (const auto& iface : ifaces.value()) {
    assert(!iface.name.empty());
    if (iface.address == "127.0.0.1") found_lo = true;
  }
  assert(found_lo);

  return 0;
}
