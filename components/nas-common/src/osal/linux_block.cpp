#include "nas/osal/linux_block.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace nas::osal {

static constexpr const char* kSysBlockPath = "/sys/block";

Result<std::vector<BlockDevice>> LinuxBlockDeviceManager::Enumerate() {
  namespace fs = std::filesystem;
  std::error_code ec;
  if (!fs::exists(kSysBlockPath, ec) || ec) {
    return Fail(ErrorCode::kNotFound, "sysfs block path not available");
  }
  fs::directory_iterator it(kSysBlockPath, ec);
  if (ec) {
    return Fail(ErrorCode::kNotFound, "cannot enumerate block devices: " + ec.message());
  }

  std::vector<BlockDevice> devices;
  for (const auto& de : it) {
    BlockDevice bd;
    bd.name = de.path().filename().string();

    std::ifstream size_file(de.path() / "size");
    if (size_file) {
      std::uint64_t sectors = 0;
      size_file >> sectors;
      bd.size_bytes = sectors * 512;
    }

    std::ifstream model_file(de.path() / "device" / "model");
    if (model_file) {
      std::getline(model_file, bd.model);
    }

    std::ifstream removable_file(de.path() / "removable");
    if (removable_file) {
      int removable = 0;
      removable_file >> removable;
      bd.removable = removable != 0;
    }

    devices.push_back(std::move(bd));
  }
  return devices;
}

Result<void> LinuxBlockDeviceManager::Rescan() {
  return Ok();
}

}  // namespace nas::osal
