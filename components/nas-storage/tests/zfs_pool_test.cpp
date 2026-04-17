#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "nas/storage/zfs_pool.hpp"

int main() {
  nas::storage::ZfsPool zfs;

  // -----------------------------------------------------------------------
  // Existing API – dry_run path
  // -----------------------------------------------------------------------
  auto list = zfs.ListDatasets(true);
  assert(list.has_value());
  assert(list.value().find("zfs list") != std::string::npos);

  auto snap = zfs.Snapshot("tank", "backup-2026", true);
  assert(snap.has_value());
  assert(snap.value().find("tank@backup-2026") != std::string::npos);

  // Validation errors (both modes)
  assert(zfs.Snapshot("", "", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(zfs.Snapshot("tank", "", true).error().code == nas::ErrorCode::kInvalidArgument);
  assert(zfs.Snapshot("", "snap1", true).error().code == nas::ErrorCode::kInvalidArgument);

  // Existing API – live mode (zfs not installed in sandbox, must fail gracefully)
  assert(!zfs.ListDatasets(false).has_value());
  assert(!zfs.Snapshot("tank", "snap1", false).has_value());

  // -----------------------------------------------------------------------
  // CreatePool
  // -----------------------------------------------------------------------
  auto pool = zfs.CreatePool("tank", "mirror", {"/dev/sdb", "/dev/sdc"}, true);
  assert(pool.has_value());
  assert(pool.value().find("zpool create") != std::string::npos);
  assert(pool.value().find("tank")         != std::string::npos);
  assert(pool.value().find("mirror")       != std::string::npos);

  // Stripe (empty vdev_type)
  auto stripe = zfs.CreatePool("data", "", {"/dev/sdb"}, true);
  assert(stripe.has_value());
  assert(stripe.value().find("data") != std::string::npos);

  // Validation
  assert(!zfs.CreatePool("", "mirror", {"/dev/sdb"}, true).has_value());
  assert(!zfs.CreatePool("tank", "invalid-raid", {"/dev/sdb"}, true).has_value());
  assert(!zfs.CreatePool("tank", "mirror", {}, true).has_value());

  // Live mode fails (zpool not installed)
  assert(!zfs.CreatePool("tank", "", {"/dev/sdb"}, false).has_value());

  // -----------------------------------------------------------------------
  // DestroyPool
  // -----------------------------------------------------------------------
  assert(zfs.DestroyPool("tank", true).has_value());
  assert(!zfs.DestroyPool("", true).has_value());
  assert(!zfs.DestroyPool("tank", false).has_value()); // not installed

  // -----------------------------------------------------------------------
  // GetPoolStatus
  // -----------------------------------------------------------------------
  auto status = zfs.GetPoolStatus("tank", true);
  assert(status.has_value());
  assert(status.value().find("tank") != std::string::npos);
  assert(!zfs.GetPoolStatus("", true).has_value());
  assert(!zfs.GetPoolStatus("tank", false).has_value()); // not installed

  // -----------------------------------------------------------------------
  // CreateDataset
  // -----------------------------------------------------------------------
  assert(zfs.CreateDataset("tank/data", true).has_value());
  assert(!zfs.CreateDataset("", true).has_value());
  assert(!zfs.CreateDataset("tank/data", false).has_value());

  // -----------------------------------------------------------------------
  // DestroyDataset
  // -----------------------------------------------------------------------
  assert(zfs.DestroyDataset("tank/data", true).has_value());
  assert(!zfs.DestroyDataset("", true).has_value());
  assert(!zfs.DestroyDataset("tank/data", false).has_value());

  // -----------------------------------------------------------------------
  // DeleteSnapshot
  // -----------------------------------------------------------------------
  assert(zfs.DeleteSnapshot("tank", "snap1", true).has_value());
  assert(!zfs.DeleteSnapshot("", "snap1", true).has_value());
  assert(!zfs.DeleteSnapshot("tank", "", true).has_value());
  assert(!zfs.DeleteSnapshot("tank", "snap1", false).has_value());

  // -----------------------------------------------------------------------
  // ListSnapshots
  // -----------------------------------------------------------------------
  auto snaps = zfs.ListSnapshots("tank", true);
  assert(snaps.has_value());
  assert(snaps.value().find("tank@") != std::string::npos);
  assert(!zfs.ListSnapshots("", true).has_value());
  assert(!zfs.ListSnapshots("tank", false).has_value());

  // -----------------------------------------------------------------------
  // SetProperty
  // -----------------------------------------------------------------------
  assert(zfs.SetProperty("tank/data", "compression", "lz4", true).has_value());
  assert(zfs.SetProperty("tank/data", "quota",       "10G", true).has_value());
  assert(!zfs.SetProperty("",         "compression", "lz4", true).has_value());
  assert(!zfs.SetProperty("tank/data", "",           "lz4", true).has_value());
  assert(!zfs.SetProperty("tank/data", "compression", "", true).has_value());
  assert(!zfs.SetProperty("tank/data", "compression", "lz4", false).has_value());

  // -----------------------------------------------------------------------
  // ListDatasetsInfo
  // -----------------------------------------------------------------------
  auto info = zfs.ListDatasetsInfo(true);
  assert(info.has_value());
  assert(info.value().size() == 2);
  assert(!info.value()[0].name.empty());
  assert(info.value()[0].used_bytes  > 0);
  assert(info.value()[0].avail_bytes > 0);
  assert(!info.value()[0].mountpoint.empty());
  assert(!zfs.ListDatasetsInfo(false).has_value()); // not installed

  return 0;
}
