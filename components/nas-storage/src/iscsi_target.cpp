#include "nas/storage/iscsi_target.hpp"

#include "run_command.hpp"

namespace nas::storage {

using detail::RunCommand;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static Result<void> ValidateIqn(const std::string& iqn) {
  if (iqn.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "iqn is empty");
  }
  return Ok();
}

static Result<void> ValidateLun(const std::string& lun) {
  if (lun.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "lun is empty");
  }
  return Ok();
}

// ---------------------------------------------------------------------------
// IscsiTarget public API
// ---------------------------------------------------------------------------

Result<std::string> IscsiTarget::MapLun(const std::string& iqn, const std::string& lun,
                                        bool dry_run) const {
  if (auto v = ValidateIqn(iqn); !v.has_value()) {
    return std::unexpected(v.error());
  }
  if (auto v = ValidateLun(lun); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string cmd =
      "targetcli /iscsi/" + iqn + "/tpg1/luns create " + lun;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
    return cmd;
  }
  return cmd;
}

Result<std::string> IscsiTarget::CreateTarget(const std::string& iqn,
                                               bool dry_run) const {
  if (auto v = ValidateIqn(iqn); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string cmd = "targetcli /iscsi create " + iqn;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
    return cmd;
  }
  return cmd;
}

Result<void> IscsiTarget::DeleteTarget(const std::string& iqn, bool dry_run) const {
  if (auto v = ValidateIqn(iqn); !v.has_value()) return v;

  const std::string cmd = "targetcli /iscsi delete " + iqn;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<std::string> IscsiTarget::ListTargets(bool dry_run) const {
  const std::string cmd = "targetcli /iscsi ls";
  if (!dry_run) {
    return RunCommand(cmd);
  }
  return "o- iscsi ......................................................[Targets: 0]\n";
}

Result<void> IscsiTarget::UnmapLun(const std::string& iqn, const std::string& lun,
                                    bool dry_run) const {
  if (auto v = ValidateIqn(iqn); !v.has_value()) return v;
  if (auto v = ValidateLun(lun); !v.has_value()) return v;

  const std::string cmd =
      "targetcli /iscsi/" + iqn + "/tpg1/luns delete " + lun;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

}  // namespace nas::storage
