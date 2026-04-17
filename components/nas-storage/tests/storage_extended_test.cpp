#include <cassert>
#include <string>

#include "nas/storage/iscsi_target.hpp"
#include "nas/storage/partition.hpp"
#include "nas/storage/share_config.hpp"
#include "nas/storage/zfs_pool.hpp"

int main() {
  // ZfsPool snapshot edge cases
  nas::storage::ZfsPool zfs;
  assert(zfs.Snapshot("", "", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(zfs.Snapshot("tank", "", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(zfs.Snapshot("", "snap1", true).error().code == nas::ErrorCode::kInvalidArgument);

  // dry_run=false must fail
  assert(!zfs.ListDatasets(false).has_value());
  assert(!zfs.Snapshot("tank", "snap1", false).has_value());

  // ZfsPool happy-path
  auto snap = zfs.Snapshot("tank", "backup-2026", true);
  assert(snap.has_value());
  assert(snap.value().find("tank@backup-2026") != std::string::npos);

  // iSCSI
  nas::storage::IscsiTarget iscsi;
  assert(!iscsi.MapLun("", "lun0", true).has_value());
  assert(!iscsi.MapLun("iqn.test", "", true).has_value());
  auto lun = iscsi.MapLun("iqn.test", "lun0", true);
  assert(lun.has_value());
  assert(lun.value().find("iqn.test") != std::string::npos);

  // ShareConfig NFS + Samba edge cases
  nas::storage::ShareConfig share;
  assert(!share.RenderNfsExports("", true).has_value());
  assert(!share.RenderSambaConfig("", true).has_value());

  auto nfs = share.RenderNfsExports("/mnt/data", true);
  assert(nfs.has_value());
  assert(nfs.value().find("/mnt/data") != std::string::npos);

  auto smb = share.RenderSambaConfig("PublicShare", true);
  assert(smb.has_value());
  assert(smb.value().find("PublicShare") != std::string::npos);
  assert(smb.value().find("browseable") != std::string::npos);

  // PartitionManager
  nas::storage::PartitionManager pm;
  assert(!pm.ProbeTopologyJson(false).has_value());  // non-dry-run must fail
  auto topo = pm.ProbeTopologyJson(true);
  assert(topo.has_value());
  assert(topo.value().find("lsblk") != std::string::npos);

  return 0;
}
