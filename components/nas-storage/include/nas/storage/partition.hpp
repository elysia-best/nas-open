#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::storage {

// -----------------------------------------------------------------------
// PartitionEntry describes a single partition visible via lsblk / parted.
// -----------------------------------------------------------------------
struct PartitionEntry {
  int number{0};              // 1-based partition number (0 if unknown)
  std::string device;         // e.g. "/dev/sda1"
  std::string type;           // "part", "disk", "rom", "lvm", ...
  std::string fs_type;        // "ext4", "xfs", "swap", "vfat", "" if raw
  std::string label;          // filesystem / partition label (may be empty)
  std::uint64_t size_bytes{0};
};

// -----------------------------------------------------------------------
// PartitionSpec is the input for CreatePartition.
// -----------------------------------------------------------------------
struct PartitionSpec {
  // Filesystem type to create after partitioning ("ext4", "xfs", "btrfs",
  // "swap", "fat32", "linux-swap").  Empty = raw partition, no mkfs.
  std::string fs_type;

  // Size in MiB; 0 means "use all remaining free space".
  std::uint64_t size_mib{0};

  // Optional partition/filesystem label.
  std::string label;
};

// -----------------------------------------------------------------------
// PartitionManager: disk partition management.
//
// dry_run = true  → return a human-readable command preview string.
// dry_run = false → execute the system command (requires root + real disk).
// -----------------------------------------------------------------------
class PartitionManager {
 public:
  // Probe full block-device topology (lsblk --json).
  [[nodiscard]] Result<std::string> ProbeTopologyJson(bool dry_run) const;

  // List all partitions on a block device (e.g. "/dev/sda").
  // In dry_run mode returns a preview JSON string.
  // In live mode parses lsblk -J output.
  [[nodiscard]] Result<std::vector<PartitionEntry>> ListPartitions(
      const std::string& device, bool dry_run) const;

  // Initialise a new partition table ("gpt" or "msdos").
  // WARNING: destroys all existing data on the device.
  [[nodiscard]] Result<void> CreateTable(
      const std::string& device, const std::string& table_type, bool dry_run) const;

  // Append a new partition to the device according to spec.
  [[nodiscard]] Result<PartitionEntry> CreatePartition(
      const std::string& device, const PartitionSpec& spec, bool dry_run) const;

  // Remove a partition by its 1-based partition number.
  [[nodiscard]] Result<void> DeletePartition(
      const std::string& device, int partition_number, bool dry_run) const;

  // Format a partition with a filesystem.
  // partition: block device path, e.g. "/dev/sda1"
  // fs_type:   "ext4", "xfs", "btrfs", "vfat", "swap"
  // label:     optional filesystem label (may be empty)
  [[nodiscard]] Result<void> FormatPartition(
      const std::string& partition, const std::string& fs_type,
      const std::string& label, bool dry_run) const;

  // Mount a partition to a mountpoint.
  // options: comma-separated mount options (may be empty → defaults)
  [[nodiscard]] Result<void> Mount(
      const std::string& partition, const std::string& mountpoint,
      const std::string& options, bool dry_run) const;

  // Unmount a mounted filesystem by mountpoint or device path.
  [[nodiscard]] Result<void> Unmount(
      const std::string& mountpoint, bool dry_run) const;

  // -----------------------------------------------------------------------
  // LVM Operations
  // -----------------------------------------------------------------------
  // Create an LVM Physical Volume
  [[nodiscard]] Result<void> CreatePhysicalVolume(
      const std::string& device, bool dry_run) const;

  // Create an LVM Volume Group
  [[nodiscard]] Result<void> CreateVolumeGroup(
      const std::string& vg_name, const std::vector<std::string>& pvs, bool dry_run) const;

  // Create an LVM Logical Volume
  [[nodiscard]] Result<void> CreateLogicalVolume(
      const std::string& vg_name, const std::string& lv_name, std::uint64_t size_mib, bool dry_run) const;

};

}  // namespace nas::storage
