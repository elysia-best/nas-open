#include "nas/storage/share_config.hpp"

namespace nas::storage {

Result<std::string> ShareConfig::RenderNfsExports(const std::string& path, bool dry_run) const {
  if (path.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "path is empty");
  }
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return path + " *(rw,sync,no_subtree_check)";
}

Result<std::string> ShareConfig::RenderSambaConfig(const std::string& share_name,
                                                   bool dry_run) const {
  if (share_name.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "share_name is empty");
  }
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return "[" + share_name + "]\n  browseable = yes\n  read only = no";
}

}  // namespace nas::storage
