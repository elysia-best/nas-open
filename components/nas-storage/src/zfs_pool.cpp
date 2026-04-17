#include "nas/storage/zfs_pool.hpp"

#include <sstream>

#include "run_command.hpp"

namespace nas::storage {

using detail::RunCommand;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static constexpr const char* kKnownVdevTypes[] = {
    "mirror", "raidz", "raidz2", "raidz3",
    "",  // stripe (no keyword)
    nullptr};

static Result<void> ValidatePoolName(const std::string& name) {
  if (name.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "pool name is empty");
  }
  return Ok();
}

static Result<void> ValidateDatasetName(const std::string& dataset) {
  if (dataset.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "dataset name is empty");
  }
  return Ok();
}

static Result<void> ValidateVdevType(const std::string& vdev_type) {
  for (int i = 0; kKnownVdevTypes[i] != nullptr; ++i) {
    if (vdev_type == kKnownVdevTypes[i]) return Ok();
  }
  return Fail(ErrorCode::kInvalidArgument,
              "unsupported vdev_type: '" + vdev_type +
                  "'; expected mirror, raidz, raidz2, raidz3, or \"\" (stripe)");
}

// Parse `zfs list -Hp -o name,used,avail,mountpoint` tab-separated output.
static std::vector<DatasetInfo> ParseZfsList(const std::string& output) {
  std::vector<DatasetInfo> result;
  std::istringstream ss(output);
  std::string line;
  while (std::getline(ss, line)) {
    if (line.empty()) continue;
    DatasetInfo info;
    std::size_t pos = 0;
    auto next_tab = [&]() -> std::string {
      auto end = line.find('\t', pos);
      std::string tok = (end == std::string::npos) ? line.substr(pos)
                                                    : line.substr(pos, end - pos);
      pos = (end == std::string::npos) ? line.size() : end + 1;
      return tok;
    };
    info.name = next_tab();
    try {
      info.used_bytes = std::stoull(next_tab());
    } catch (...) {
      info.used_bytes = 0;
    }
    try {
      info.avail_bytes = std::stoull(next_tab());
    } catch (...) {
      info.avail_bytes = 0;
    }
    info.mountpoint = next_tab();
    if (!info.name.empty()) {
      result.push_back(std::move(info));
    }
  }
  return result;
}

// ---------------------------------------------------------------------------
// ZfsPool public API
// ---------------------------------------------------------------------------

Result<std::string> ZfsPool::ListDatasets(bool dry_run) const {
  const std::string cmd = "zfs list -Hp -o name,used,avail,mountpoint";
  if (!dry_run) {
    return RunCommand(cmd);
  }
  return cmd;
}

Result<std::string> ZfsPool::Snapshot(const std::string& dataset, const std::string& snap,
                                      bool dry_run) const {
  if (dataset.empty() || snap.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "dataset/snapshot is empty");
  }
  const std::string cmd = "zfs snapshot " + dataset + "@" + snap;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
    return cmd;
  }
  return cmd;
}

Result<std::string> ZfsPool::CreatePool(const std::string& pool_name,
                                         const std::string& vdev_type,
                                         const std::vector<std::string>& devices,
                                         bool dry_run) const {
  if (auto v = ValidatePoolName(pool_name); !v.has_value()) {
    return std::unexpected(v.error());
  }
  if (auto v = ValidateVdevType(vdev_type); !v.has_value()) {
    return std::unexpected(v.error());
  }
  if (devices.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "devices list is empty");
  }

  std::string cmd = "zpool create " + pool_name;
  if (!vdev_type.empty()) cmd += " " + vdev_type;
  for (const auto& dev : devices) cmd += " " + dev;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
    return cmd;
  }
  return cmd;
}

Result<void> ZfsPool::DestroyPool(const std::string& pool_name, bool dry_run) const {
  if (auto v = ValidatePoolName(pool_name); !v.has_value()) return v;

  const std::string cmd = "zpool destroy -f " + pool_name;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<std::string> ZfsPool::GetPoolStatus(const std::string& pool_name,
                                            bool dry_run) const {
  if (auto v = ValidatePoolName(pool_name); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string cmd = "zpool status -P " + pool_name;
  if (!dry_run) {
    return RunCommand(cmd);
  }
  return "pool: " + pool_name + "\n state: ONLINE\n status: (dry-run preview)";
}

Result<void> ZfsPool::CreateDataset(const std::string& dataset, bool dry_run) const {
  if (auto v = ValidateDatasetName(dataset); !v.has_value()) return v;

  const std::string cmd = "zfs create " + dataset;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<void> ZfsPool::DestroyDataset(const std::string& dataset, bool dry_run) const {
  if (auto v = ValidateDatasetName(dataset); !v.has_value()) return v;

  const std::string cmd = "zfs destroy -r " + dataset;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<void> ZfsPool::DeleteSnapshot(const std::string& dataset, const std::string& snap,
                                      bool dry_run) const {
  if (dataset.empty() || snap.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "dataset/snapshot is empty");
  }

  const std::string cmd = "zfs destroy " + dataset + "@" + snap;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<std::string> ZfsPool::ListSnapshots(const std::string& dataset,
                                            bool dry_run) const {
  if (auto v = ValidateDatasetName(dataset); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string cmd =
      "zfs list -Hp -t snapshot -o name,used,creation -r " + dataset;
  if (!dry_run) {
    return RunCommand(cmd);
  }
  return dataset + "@backup-2026-01-01\t1048576\t1735689600\n";
}

Result<void> ZfsPool::SetProperty(const std::string& dataset,
                                   const std::string& property,
                                   const std::string& value,
                                   bool dry_run) const {
  if (auto v = ValidateDatasetName(dataset); !v.has_value()) return v;
  if (property.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "property name is empty");
  }
  if (value.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "property value is empty");
  }

  const std::string cmd = "zfs set " + property + "=" + value + " " + dataset;
  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<std::vector<DatasetInfo>> ZfsPool::ListDatasetsInfo(bool dry_run) const {
  if (!dry_run) {
    auto out = RunCommand("zfs list -Hp -o name,used,avail,mountpoint");
    if (!out.has_value()) return std::unexpected(out.error());
    return ParseZfsList(out.value());
  }
  // dry_run: return two synthetic entries
  return std::vector<DatasetInfo>{
      {"tank",      8ULL * 1024 * 1024 * 1024, 92ULL * 1024 * 1024 * 1024, "/tank"},
      {"tank/data", 4ULL * 1024 * 1024 * 1024, 92ULL * 1024 * 1024 * 1024, "/tank/data"},
  };
}

}  // namespace nas::storage
