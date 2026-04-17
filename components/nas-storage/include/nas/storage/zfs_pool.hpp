#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::storage {

// Structured dataset information returned by ListDatasetsInfo.
struct DatasetInfo {
  std::string name;
  std::uint64_t used_bytes{0};
  std::uint64_t avail_bytes{0};
  std::string mountpoint;
};

class ZfsPool {
 public:
  // -----------------------------------------------------------------------
  // Existing API (kept for backward compatibility)
  // -----------------------------------------------------------------------

  // List all datasets.
  // dry_run=true  → returns the command that would be run.
  // dry_run=false → executes `zfs list -Hp -o name,used,avail,mountpoint`
  //                 and returns the raw output.
  [[nodiscard]] Result<std::string> ListDatasets(bool dry_run) const;

  // Create a snapshot of dataset named "<dataset>@<snap>".
  // dry_run=true  → returns the command string.
  // dry_run=false → executes `zfs snapshot <dataset>@<snap>`.
  [[nodiscard]] Result<std::string> Snapshot(const std::string& dataset,
                                             const std::string& snap,
                                             bool dry_run) const;

  // -----------------------------------------------------------------------
  // Pool management
  // -----------------------------------------------------------------------

  // Create a new ZFS storage pool.
  // vdev_type: "mirror", "raidz", "raidz2", "raidz3", or "" (stripe).
  // devices:   list of block device paths (e.g. "/dev/sdb").
  [[nodiscard]] Result<std::string> CreatePool(const std::string& pool_name,
                                               const std::string& vdev_type,
                                               const std::vector<std::string>& devices,
                                               bool dry_run) const;

  // Destroy a pool (forcefully).
  [[nodiscard]] Result<void> DestroyPool(const std::string& pool_name,
                                         bool dry_run) const;

  // Query pool health and configuration.
  [[nodiscard]] Result<std::string> GetPoolStatus(const std::string& pool_name,
                                                   bool dry_run) const;

  // -----------------------------------------------------------------------
  // Dataset management
  // -----------------------------------------------------------------------

  // Create a new dataset (filesystem).
  [[nodiscard]] Result<void> CreateDataset(const std::string& dataset,
                                            bool dry_run) const;

  // Destroy a dataset (and all snapshots under it).
  [[nodiscard]] Result<void> DestroyDataset(const std::string& dataset,
                                             bool dry_run) const;

  // Delete a specific snapshot: <dataset>@<snap>.
  [[nodiscard]] Result<void> DeleteSnapshot(const std::string& dataset,
                                             const std::string& snap,
                                             bool dry_run) const;

  // List all snapshots for a dataset.
  [[nodiscard]] Result<std::string> ListSnapshots(const std::string& dataset,
                                                   bool dry_run) const;

  // Set a ZFS property on a dataset (e.g. compression=lz4).
  [[nodiscard]] Result<void> SetProperty(const std::string& dataset,
                                          const std::string& property,
                                          const std::string& value,
                                          bool dry_run) const;

  // Like ListDatasets but returns structured DatasetInfo records.
  // dry_run=true returns two synthetic entries.
  [[nodiscard]] Result<std::vector<DatasetInfo>> ListDatasetsInfo(
      bool dry_run) const;
};

}  // namespace nas::storage
