#include <cassert>

#include "nas/storage/iscsi_target.hpp"
#include "nas/storage/partition.hpp"
#include "nas/storage/share_config.hpp"
#include "nas/storage/zfs_pool.hpp"

int main() {
  nas::storage::PartitionManager p;
  auto topo = p.ProbeTopologyJson(true);
  assert(topo.has_value());

  nas::storage::ZfsPool z;
  auto list = z.ListDatasets(true);
  assert(list.has_value());
  auto snap_bad = z.Snapshot("", "s1", true);
  assert(!snap_bad.has_value());

  nas::storage::IscsiTarget i;
  auto map = i.MapLun("iqn.2026-04.local:nas", "lun0", true);
  assert(map.has_value());

  nas::storage::ShareConfig s;
  auto nfs = s.RenderNfsExports("/data", true);
  assert(nfs.has_value());
  auto smb = s.RenderSambaConfig("Public", true);
  assert(smb.has_value());

  return 0;
}
