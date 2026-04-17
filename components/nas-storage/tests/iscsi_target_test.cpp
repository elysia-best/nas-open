#include <cassert>
#include <string>

#include "nas/storage/iscsi_target.hpp"

int main() {
  nas::storage::IscsiTarget iscsi;

  // -----------------------------------------------------------------------
  // MapLun – existing API
  // -----------------------------------------------------------------------
  auto lun = iscsi.MapLun("iqn.2026-04.local:nas", "lun0", true);
  assert(lun.has_value());
  assert(lun.value().find("iqn.2026-04.local:nas") != std::string::npos);
  assert(lun.value().find("lun0")                  != std::string::npos);

  // Validation
  assert(!iscsi.MapLun("", "lun0", true).has_value());
  assert(iscsi.MapLun("", "lun0", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(!iscsi.MapLun("iqn.test", "", true).has_value());
  assert(iscsi.MapLun("iqn.test", "", true).error().code == nas::ErrorCode::kInvalidArgument);

  // Live mode fails (targetcli not installed)
  assert(!iscsi.MapLun("iqn.test", "lun0", false).has_value());

  // -----------------------------------------------------------------------
  // CreateTarget
  // -----------------------------------------------------------------------
  auto target = iscsi.CreateTarget("iqn.2026-04.local:nas", true);
  assert(target.has_value());
  assert(target.value().find("iqn.2026-04.local:nas") != std::string::npos);
  assert(target.value().find("targetcli")             != std::string::npos);

  assert(!iscsi.CreateTarget("", true).has_value());
  assert(iscsi.CreateTarget("", true).error().code == nas::ErrorCode::kInvalidArgument);

  assert(!iscsi.CreateTarget("iqn.test", false).has_value()); // not installed

  // -----------------------------------------------------------------------
  // DeleteTarget
  // -----------------------------------------------------------------------
  assert(iscsi.DeleteTarget("iqn.2026-04.local:nas", true).has_value());
  assert(!iscsi.DeleteTarget("", true).has_value());
  assert(iscsi.DeleteTarget("", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(!iscsi.DeleteTarget("iqn.test", false).has_value()); // not installed

  // -----------------------------------------------------------------------
  // ListTargets
  // -----------------------------------------------------------------------
  auto targets = iscsi.ListTargets(true);
  assert(targets.has_value());
  assert(!targets.value().empty());
  assert(!iscsi.ListTargets(false).has_value()); // not installed

  // -----------------------------------------------------------------------
  // UnmapLun
  // -----------------------------------------------------------------------
  assert(iscsi.UnmapLun("iqn.test", "lun0", true).has_value());
  assert(!iscsi.UnmapLun("", "lun0", true).has_value());
  assert(iscsi.UnmapLun("", "lun0", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(!iscsi.UnmapLun("iqn.test", "", true).has_value());
  assert(iscsi.UnmapLun("iqn.test", "", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(!iscsi.UnmapLun("iqn.test", "lun0", false).has_value()); // not installed

  return 0;
}
