#include <cassert>
#include <filesystem>

#include "nas/osal/linux_block.hpp"

int main() {
  nas::osal::LinuxBlockDeviceManager mgr;

  // Enumerate may fail in sandbox (no /sys/block); that is acceptable
  auto result = mgr.Enumerate();
  // If it succeeded, all entries must have a non-empty name
  if (result.has_value()) {
    for (const auto& bd : result.value()) {
      assert(!bd.name.empty());
    }
  }

  // Rescan should always succeed
  assert(mgr.Rescan().has_value());

  return 0;
}
