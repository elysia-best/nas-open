#include "nas/storage/partition.hpp"

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sstream>

namespace nas::storage {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Run a shell command and capture stdout.  Returns the captured output on
// success (exit-status 0) or an Error on failure.
static Result<std::string> RunCommand(const std::string& cmd) {
  // NOLINTNEXTLINE(cert-env33-c)
  std::FILE* pipe = ::popen(cmd.c_str(), "r");
  if (!pipe) {
    return Fail(ErrorCode::kInternal,
                std::string("popen failed: ") + std::strerror(errno));
  }

  std::string output;
  std::array<char, 256> buf{};
  while (std::fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr) {
    output += buf.data();
  }

  int status = ::pclose(pipe);
  if (status != 0) {
    return Fail(ErrorCode::kInternal,
                "command failed (exit " + std::to_string(status) + "): " + cmd);
  }
  return output;
}

// Validate a block device path: must be non-empty and start with '/'.
static Result<void> ValidateDevicePath(const std::string& device) {
  if (device.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "device path is empty");
  }
  if (device.front() != '/') {
    return Fail(ErrorCode::kInvalidArgument,
                "device path must be absolute: " + device);
  }
  return Ok();
}

// Validate a filesystem type against known supported types.
static Result<void> ValidateFsType(const std::string& fs_type) {
  static constexpr const char* kKnownFs[] = {
      "ext4", "xfs", "btrfs", "vfat", "fat32", "swap", "linux-swap", nullptr};
  for (int i = 0; kKnownFs[i] != nullptr; ++i) {
    if (fs_type == kKnownFs[i]) return Ok();
  }
  return Fail(ErrorCode::kInvalidArgument, "unsupported fs_type: " + fs_type);
}

// Validate a partition-table type.
static Result<void> ValidateTableType(const std::string& table_type) {
  if (table_type == "gpt" || table_type == "msdos") return Ok();
  return Fail(ErrorCode::kInvalidArgument,
              "table_type must be 'gpt' or 'msdos', got: " + table_type);
}

// Map fs_type to the appropriate mkfs binary name.
static std::string MkfsCommand(const std::string& fs_type) {
  if (fs_type == "swap" || fs_type == "linux-swap") return "mkswap";
  if (fs_type == "fat32" || fs_type == "vfat")      return "mkfs.vfat";
  return "mkfs." + fs_type;
}

// Parse minimal lsblk -J output into PartitionEntry list.
// Only handles the flat "blockdevices[].children[]" structure.
static std::vector<PartitionEntry> ParseLsblkJson(const std::string& json,
                                                   const std::string& parent) {
  std::vector<PartitionEntry> result;
  // Simple ad-hoc token scan – avoids pulling in a JSON library.
  // Extracts "name","type","fstype","label","size" fields from each object.
  auto extract = [&](const std::string& key, std::size_t from) -> std::string {
    const std::string needle = "\"" + key + "\":";
    auto pos = json.find(needle, from);
    if (pos == std::string::npos) return {};
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    if (pos >= json.size()) return {};
    if (json[pos] == '"') {
      ++pos;
      std::string val;
      while (pos < json.size() && json[pos] != '"') val += json[pos++];
      return val;
    }
    // null / number
    if (json.substr(pos, 4) == "null") return {};
    std::string val;
    while (pos < json.size() && json[pos] != ',' && json[pos] != '}' &&
           json[pos] != ']' && json[pos] != '\n')
      val += json[pos++];
    // strip whitespace
    while (!val.empty() && std::isspace(static_cast<unsigned char>(val.back())))
      val.pop_back();
    return val;
  };

  // Scan all '{' objects that look like partition entries.
  std::size_t pos = 0;
  int part_num = 0;
  while ((pos = json.find('{', pos)) != std::string::npos) {
    const std::size_t block_start = pos;
    ++pos;

    std::string name     = extract("name",   block_start);
    std::string type     = extract("type",   block_start);
    std::string fs_type  = extract("fstype", block_start);
    std::string label    = extract("label",  block_start);
    std::string size_str = extract("size",   block_start);

    if (name.empty() || type.empty()) continue;
    // Only keep children of the parent device (skip the parent itself).
    if (name == parent || name.find('/') != std::string::npos) {
      // absolute path: use directly
    } else {
      // lsblk sometimes returns bare names like "sda1"
      if (type != "part" && type != "lvm") continue;
    }
    if (type != "part" && type != "lvm") continue;

    PartitionEntry pe;
    pe.device = (name.front() == '/') ? name : "/dev/" + name;
    pe.type = type;
    pe.fs_type = fs_type;
    pe.label = label;
    if (!size_str.empty()) {
      try {
        pe.size_bytes = std::stoull(size_str);
      } catch (...) {
        pe.size_bytes = 0;
      }
    }
    ++part_num;
    pe.number = part_num;
    result.push_back(std::move(pe));
  }
  return result;
}

// ---------------------------------------------------------------------------
// PartitionManager public API
// ---------------------------------------------------------------------------

Result<std::string> PartitionManager::ProbeTopologyJson(bool dry_run) const {
  if (!dry_run) {
    return RunCommand("lsblk --json -b -o NAME,TYPE,FSTYPE,LABEL,SIZE,MOUNTPOINT");
  }
  return R"({"command":"lsblk --json -b -o NAME,TYPE,FSTYPE,LABEL,SIZE,MOUNTPOINT","mode":"dry-run"})";
}

Result<std::vector<PartitionEntry>> PartitionManager::ListPartitions(
    const std::string& device, bool dry_run) const {
  if (auto v = ValidateDevicePath(device); !v.has_value()) {
    return std::unexpected(v.error());
  }

  if (!dry_run) {
    auto out = RunCommand("lsblk -J -b -o NAME,TYPE,FSTYPE,LABEL,SIZE " + device);
    if (!out.has_value()) return std::unexpected(out.error());
    // Extract bare device name (e.g. "sda" from "/dev/sda")
    std::string base = device;
    if (const auto slash = base.rfind('/'); slash != std::string::npos) {
      base = base.substr(slash + 1);
    }
    return ParseLsblkJson(out.value(), base);
  }

  // dry_run: return two synthetic partitions
  std::vector<PartitionEntry> preview;
  preview.push_back({1, device + "1", "part", "ext4", "", 10ULL * 1024 * 1024 * 1024});
  preview.push_back({2, device + "2", "part", "swap", "", 2ULL * 1024 * 1024 * 1024});
  return preview;
}

Result<void> PartitionManager::CreateTable(
    const std::string& device, const std::string& table_type, bool dry_run) const {
  if (auto v = ValidateDevicePath(device); !v.has_value()) return v;
  if (auto v = ValidateTableType(table_type); !v.has_value()) return v;

  if (!dry_run) {
    auto res = RunCommand("parted -s " + device + " mklabel " + table_type);
    if (!res.has_value()) return std::unexpected(res.error());
    return Ok();
  }
  return Ok();
}

Result<PartitionEntry> PartitionManager::CreatePartition(
    const std::string& device, const PartitionSpec& spec, bool dry_run) const {
  if (auto v = ValidateDevicePath(device); !v.has_value()) {
    return std::unexpected(v.error());
  }
  if (!spec.fs_type.empty()) {
    if (auto v = ValidateFsType(spec.fs_type); !v.has_value()) {
      return std::unexpected(v.error());
    }
  }

  // Build parted mkpart command.
  // parted -s <dev> mkpart primary <fstype> <start> <end>
  const std::string part_type   = "primary";
  const std::string start_pos   = "0%";
  const std::string end_pos     = (spec.size_mib == 0)
                                      ? "100%"
                                      : std::to_string(spec.size_mib) + "MiB";
  const std::string parted_fs   = spec.fs_type.empty() ? "ext4" : spec.fs_type;

  const std::string cmd =
      "parted -s " + device + " mkpart " + part_type + " " +
      parted_fs + " " + start_pos + " " + end_pos;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }

  // Derive partition device name heuristic: devices ending with a digit
  // (e.g. nvme0n1) use "p1" suffix; others (e.g. sda) use "1".
  // Note: this always returns partition number 1 as a dry-run/preview
  // approximation.  In live mode the caller should use ListPartitions()
  // after creation to discover the exact partition device assigned by the
  // kernel.
  const std::string part_dev =
      device + (std::isdigit(static_cast<unsigned char>(device.back())) ? "p1" : "1");

  // Guard against unreasonably large size_mib values before converting to
  // bytes.  The maximum representable size in bytes for uint64_t is ~16 EiB;
  // we cap at 16 PiB (reasonable upper bound for a single partition).
  static constexpr std::uint64_t kMaxSizeMib = 16ULL * 1024 * 1024;  // 16 PiB in MiB
  if (spec.size_mib > kMaxSizeMib) {
    return Fail(ErrorCode::kInvalidArgument,
                "size_mib exceeds maximum allowed value (16 PiB)");
  }

  PartitionEntry entry;
  entry.number = 1;
  entry.device = part_dev;
  entry.type = "part";
  entry.fs_type = spec.fs_type;
  entry.label = spec.label;
  entry.size_bytes = spec.size_mib * 1024ULL * 1024ULL;
  return entry;
}

Result<void> PartitionManager::DeletePartition(
    const std::string& device, int partition_number, bool dry_run) const {
  if (auto v = ValidateDevicePath(device); !v.has_value()) return v;
  if (partition_number <= 0) {
    return Fail(ErrorCode::kInvalidArgument,
                "partition_number must be >= 1, got: " +
                    std::to_string(partition_number));
  }

  const std::string cmd =
      "parted -s " + device + " rm " + std::to_string(partition_number);

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<void> PartitionManager::FormatPartition(
    const std::string& partition, const std::string& fs_type,
    const std::string& label, bool dry_run) const {
  if (auto v = ValidateDevicePath(partition); !v.has_value()) return v;
  if (auto v = ValidateFsType(fs_type); !v.has_value()) return v;

  const std::string mkfs_bin = MkfsCommand(fs_type);

  std::string cmd = mkfs_bin;
  if (!label.empty()) {
    // Most mkfs tools accept -L for label; mkswap uses -L as well.
    cmd += " -L " + label;
  }
  cmd += " " + partition;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<void> PartitionManager::Mount(
    const std::string& partition, const std::string& mountpoint,
    const std::string& options, bool dry_run) const {
  if (auto v = ValidateDevicePath(partition); !v.has_value()) return v;
  if (mountpoint.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "mountpoint is empty");
  }
  if (mountpoint.front() != '/') {
    return Fail(ErrorCode::kInvalidArgument,
                "mountpoint must be absolute: " + mountpoint);
  }

  std::string cmd = "mount";
  if (!options.empty()) cmd += " -o " + options;
  cmd += " " + partition + " " + mountpoint;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<void> PartitionManager::Unmount(
    const std::string& mountpoint, bool dry_run) const {
  if (mountpoint.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "mountpoint is empty");
  }

  const std::string cmd = "umount " + mountpoint;

  if (!dry_run) {
    auto res = RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

}  // namespace nas::storage
