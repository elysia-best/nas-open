#include <cassert>
#include <string>
#include <vector>

#include "nas/storage/partition.hpp"

int main() {
  nas::storage::PartitionManager pm;

  // -----------------------------------------------------------------
  // ProbeTopologyJson
  // -----------------------------------------------------------------
  auto topo = pm.ProbeTopologyJson(true);
  assert(topo.has_value());
  assert(topo.value().find("lsblk") != std::string::npos);
  assert(topo.value().find("dry-run") != std::string::npos);

  // non-dry-run uses real lsblk; tolerate failure in sandbox
  // (We do NOT assert success for non-dry-run calls that touch the OS.)

  // -----------------------------------------------------------------
  // ListPartitions – dry_run
  // -----------------------------------------------------------------
  auto parts = pm.ListPartitions("/dev/sda", true);
  assert(parts.has_value());
  assert(parts.value().size() == 2);
  assert(parts.value()[0].device == "/dev/sda1");
  assert(parts.value()[0].fs_type == "ext4");
  assert(parts.value()[1].device == "/dev/sda2");
  assert(parts.value()[1].fs_type == "swap");
  assert(parts.value()[0].size_bytes > 0);
  assert(parts.value()[1].size_bytes > 0);

  // ListPartitions – validation
  auto bad_dev = pm.ListPartitions("", true);
  assert(!bad_dev.has_value());
  assert(bad_dev.error().code == nas::ErrorCode::kInvalidArgument);

  auto bad_rel = pm.ListPartitions("sda", true);
  assert(!bad_rel.has_value());
  assert(bad_rel.error().code == nas::ErrorCode::kInvalidArgument);

  // -----------------------------------------------------------------
  // CreateTable – dry_run
  // -----------------------------------------------------------------
  assert(pm.CreateTable("/dev/sdb", "gpt", true).has_value());
  assert(pm.CreateTable("/dev/sdb", "msdos", true).has_value());

  // CreateTable – bad table type
  auto bad_tbl = pm.CreateTable("/dev/sdb", "mbr", true);
  assert(!bad_tbl.has_value());
  assert(bad_tbl.error().code == nas::ErrorCode::kInvalidArgument);

  // CreateTable – empty device
  assert(!pm.CreateTable("", "gpt", true).has_value());

  // -----------------------------------------------------------------
  // CreatePartition – dry_run
  // -----------------------------------------------------------------
  nas::storage::PartitionSpec spec_ext4;
  spec_ext4.fs_type  = "ext4";
  spec_ext4.size_mib = 10240;
  spec_ext4.label    = "data";

  auto entry = pm.CreatePartition("/dev/sdb", spec_ext4, true);
  assert(entry.has_value());
  assert(entry.value().device == "/dev/sdb1");
  assert(entry.value().fs_type == "ext4");
  assert(entry.value().label == "data");
  assert(entry.value().size_bytes == 10240ULL * 1024 * 1024);
  assert(entry.value().type == "part");

  // CreatePartition – use all remaining space (size_mib == 0)
  nas::storage::PartitionSpec spec_xfs;
  spec_xfs.fs_type  = "xfs";
  spec_xfs.size_mib = 0;
  auto full_entry = pm.CreatePartition("/dev/sdc", spec_xfs, true);
  assert(full_entry.has_value());
  assert(full_entry.value().size_bytes == 0);  // 0 MiB → 0 bytes

  // CreatePartition – bad fs_type
  nas::storage::PartitionSpec bad_spec;
  bad_spec.fs_type  = "ntfs";
  bad_spec.size_mib = 1024;
  auto bad_part = pm.CreatePartition("/dev/sdb", bad_spec, true);
  assert(!bad_part.has_value());
  assert(bad_part.error().code == nas::ErrorCode::kInvalidArgument);

  // CreatePartition – empty device
  assert(!pm.CreatePartition("", spec_ext4, true).has_value());

  // CreatePartition – no fs_type (raw partition) should succeed
  nas::storage::PartitionSpec raw_spec;
  raw_spec.size_mib = 512;
  assert(pm.CreatePartition("/dev/sdd", raw_spec, true).has_value());

  // CreatePartition – nvme-style device (ends with digit) → "p1" suffix
  auto nvme = pm.CreatePartition("/dev/nvme0n1", spec_ext4, true);
  assert(nvme.has_value());
  assert(nvme.value().device == "/dev/nvme0n1p1");

  // -----------------------------------------------------------------
  // DeletePartition – dry_run
  // -----------------------------------------------------------------
  assert(pm.DeletePartition("/dev/sdb", 1, true).has_value());
  assert(pm.DeletePartition("/dev/sdb", 3, true).has_value());

  // DeletePartition – invalid partition number
  auto bad_num = pm.DeletePartition("/dev/sdb", 0, true);
  assert(!bad_num.has_value());
  assert(bad_num.error().code == nas::ErrorCode::kInvalidArgument);

  auto neg_num = pm.DeletePartition("/dev/sdb", -1, true);
  assert(!neg_num.has_value());

  // DeletePartition – empty device
  assert(!pm.DeletePartition("", 1, true).has_value());

  // -----------------------------------------------------------------
  // FormatPartition – dry_run
  // -----------------------------------------------------------------
  assert(pm.FormatPartition("/dev/sdb1", "ext4", "data",   true).has_value());
  assert(pm.FormatPartition("/dev/sdb2", "xfs",  "backup", true).has_value());
  assert(pm.FormatPartition("/dev/sdb3", "btrfs","media",  true).has_value());
  assert(pm.FormatPartition("/dev/sdb4", "swap", "",       true).has_value());
  assert(pm.FormatPartition("/dev/sdb5", "vfat", "EFI",    true).has_value());

  // FormatPartition – bad fs_type
  auto bad_fmt = pm.FormatPartition("/dev/sdb1", "ntfs", "", true);
  assert(!bad_fmt.has_value());
  assert(bad_fmt.error().code == nas::ErrorCode::kInvalidArgument);

  // FormatPartition – empty partition
  assert(!pm.FormatPartition("", "ext4", "", true).has_value());

  // FormatPartition – relative path
  assert(!pm.FormatPartition("sdb1", "ext4", "", true).has_value());

  // -----------------------------------------------------------------
  // Mount – dry_run
  // -----------------------------------------------------------------
  assert(pm.Mount("/dev/sdb1", "/mnt/data",   "",          true).has_value());
  assert(pm.Mount("/dev/sdb1", "/mnt/data",   "ro,noatime",true).has_value());

  // Mount – empty mountpoint
  auto bad_mp = pm.Mount("/dev/sdb1", "", "", true);
  assert(!bad_mp.has_value());
  assert(bad_mp.error().code == nas::ErrorCode::kInvalidArgument);

  // Mount – relative mountpoint
  auto rel_mp = pm.Mount("/dev/sdb1", "mnt/data", "", true);
  assert(!rel_mp.has_value());
  assert(rel_mp.error().code == nas::ErrorCode::kInvalidArgument);

  // Mount – empty partition
  assert(!pm.Mount("", "/mnt/data", "", true).has_value());

  // -----------------------------------------------------------------
  // Unmount – dry_run
  // -----------------------------------------------------------------
  assert(pm.Unmount("/mnt/data",   true).has_value());
  assert(pm.Unmount("/dev/sdb1",   true).has_value());

  // Unmount – empty mountpoint
  auto bad_um = pm.Unmount("", true);
  assert(!bad_um.has_value());
  assert(bad_um.error().code == nas::ErrorCode::kInvalidArgument);

  return 0;
}
