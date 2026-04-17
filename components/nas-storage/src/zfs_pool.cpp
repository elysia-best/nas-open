#include "nas/storage/zfs_pool.hpp"

namespace nas::storage {

Result<std::string> ZfsPool::ListDatasets(bool dry_run) const {
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return "zfs list -Hp -o name,used,avail,mountpoint";
}

Result<std::string> ZfsPool::Snapshot(const std::string& dataset, const std::string& snap,
                                      bool dry_run) const {
  if (dataset.empty() || snap.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "dataset/snapshot is empty");
  }
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return "zfs snapshot " + dataset + "@" + snap;
}

}  // namespace nas::storage
