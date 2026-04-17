#include "nas/storage/partition.hpp"

namespace nas::storage {

Result<std::string> PartitionManager::ProbeTopologyJson(bool dry_run) const {
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return R"({"command":"lsblk --json","mode":"dry-run"})";
}

}  // namespace nas::storage
