#include "nas/storage/iscsi_target.hpp"

namespace nas::storage {

Result<std::string> IscsiTarget::MapLun(const std::string& iqn, const std::string& lun,
                                        bool dry_run) const {
  if (iqn.empty() || lun.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "iqn/lun is empty");
  }
  if (!dry_run) {
    return Fail(ErrorCode::kPermissionDenied, "non dry-run mode is disabled in scaffold");
  }
  return "targetcli /iscsi/" + iqn + " /tpg1/luns create " + lun;
}

}  // namespace nas::storage
