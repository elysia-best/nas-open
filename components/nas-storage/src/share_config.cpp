#include "nas/storage/share_config.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "run_command.hpp"

namespace nas::storage {

using detail::RunCommand;

// ---------------------------------------------------------------------------
// Internal constants and helpers
// ---------------------------------------------------------------------------

static constexpr const char* kNfsExportsFile  = "/etc/exports";
static constexpr const char* kSambaConfigFile = "/etc/samba/smb.conf";

static Result<void> ValidatePath(const std::string& path) {
  if (path.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "path is empty");
  }
  return Ok();
}

static Result<void> ValidateShareName(const std::string& name) {
  if (name.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "share_name is empty");
  }
  return Ok();
}

// Build the exports line for a path + options.
static std::string BuildNfsLine(const std::string& path, const std::string& options) {
  const std::string opts = options.empty() ? "*(rw,sync,no_subtree_check)" : options;
  return path + " " + opts;
}

// Build a Samba share config block.
static std::string BuildSambaBlock(const std::string& share_name,
                                    const std::string& path,
                                    bool read_only) {
  const std::string share_path = path.empty() ? "/srv/samba/" + share_name : path;
  std::string block;
  block += "[" + share_name + "]\n";
  block += "  path = " + share_path + "\n";
  block += "  browseable = yes\n";
  block += "  read only = " + std::string(read_only ? "yes" : "no") + "\n";
  block += "  guest ok = no\n";
  return block;
}

// Append a line to a file.
static Result<void> AppendToFile(const std::string& file_path,
                                  const std::string& content) {
  std::ofstream ofs(file_path, std::ios::app);
  if (!ofs) {
    return Fail(ErrorCode::kInternal, "cannot open file for writing: " + file_path);
  }
  ofs << content << "\n";
  if (!ofs) {
    return Fail(ErrorCode::kInternal, "write failed: " + file_path);
  }
  return Ok();
}

// Read entire file; returns empty string if it does not exist.
static std::string ReadFileOrEmpty(const std::string& file_path) {
  std::ifstream ifs(file_path);
  if (!ifs) return {};
  return {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
}

// Write content to file, replacing existing content.
static Result<void> WriteFile(const std::string& file_path, const std::string& content) {
  std::ofstream ofs(file_path, std::ios::trunc);
  if (!ofs) {
    return Fail(ErrorCode::kInternal, "cannot open file for writing: " + file_path);
  }
  ofs << content;
  if (!ofs) {
    return Fail(ErrorCode::kInternal, "write failed: " + file_path);
  }
  return Ok();
}

// ---------------------------------------------------------------------------
// ShareConfig public API
// ---------------------------------------------------------------------------

Result<std::string> ShareConfig::RenderNfsExports(const std::string& path,
                                                   bool dry_run) const {
  if (auto v = ValidatePath(path); !v.has_value()) {
    return std::unexpected(v.error());
  }
  (void)dry_run;  // rendering is the same regardless of mode
  return BuildNfsLine(path, {});
}

Result<std::string> ShareConfig::RenderSambaConfig(const std::string& share_name,
                                                    bool dry_run) const {
  if (auto v = ValidateShareName(share_name); !v.has_value()) {
    return std::unexpected(v.error());
  }
  (void)dry_run;  // rendering is the same regardless of mode
  return BuildSambaBlock(share_name, {}, /*read_only=*/false);
}

Result<std::string> ShareConfig::ApplyNfsExports(const std::string& path,
                                                  const std::string& options,
                                                  bool dry_run) const {
  if (auto v = ValidatePath(path); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string line = BuildNfsLine(path, options);

  if (!dry_run) {
    if (auto v = AppendToFile(kNfsExportsFile, line); !v.has_value()) {
      return std::unexpected(v.error());
    }
    auto res = RunCommand("exportfs -ra");
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return line;
}

Result<void> ShareConfig::RemoveNfsExport(const std::string& path,
                                           bool dry_run) const {
  if (auto v = ValidatePath(path); !v.has_value()) return v;

  if (!dry_run) {
    const std::string existing = ReadFileOrEmpty(kNfsExportsFile);
    std::ostringstream filtered;
    std::istringstream ss(existing);
    std::string line;
    while (std::getline(ss, line)) {
      // Drop lines that export this path (starts with path followed by space or EOL).
      if (line.find(path) == 0 &&
          (line.size() == path.size() || line[path.size()] == ' ' ||
           line[path.size()] == '\t')) {
        continue;
      }
      filtered << line << "\n";
    }
    if (auto v = WriteFile(kNfsExportsFile, filtered.str()); !v.has_value()) {
      return v;
    }
    auto res = RunCommand("exportfs -ra");
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

Result<std::string> ShareConfig::ApplySambaShare(const std::string& share_name,
                                                   const std::string& path,
                                                   bool read_only,
                                                   bool dry_run) const {
  if (auto v = ValidateShareName(share_name); !v.has_value()) {
    return std::unexpected(v.error());
  }

  const std::string block = BuildSambaBlock(share_name, path, read_only);

  if (!dry_run) {
    if (auto v = AppendToFile(kSambaConfigFile, block); !v.has_value()) {
      return std::unexpected(v.error());
    }
    auto res = RunCommand("smbcontrol all reload-config");
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return block;
}

Result<void> ShareConfig::RemoveSambaShare(const std::string& share_name,
                                            bool dry_run) const {
  if (auto v = ValidateShareName(share_name); !v.has_value()) return v;

  if (!dry_run) {
    const std::string existing = ReadFileOrEmpty(kSambaConfigFile);
    std::ostringstream filtered;
    std::istringstream ss(existing);
    std::string line;
    bool in_block = false;
    const std::string header = "[" + share_name + "]";
    while (std::getline(ss, line)) {
      if (line == header) {
        in_block = true;
        continue;
      }
      // A new [section] ends the current block.
      if (in_block && !line.empty() && line.front() == '[') {
        in_block = false;
      }
      if (!in_block) {
        filtered << line << "\n";
      }
    }
    if (auto v = WriteFile(kSambaConfigFile, filtered.str()); !v.has_value()) {
      return v;
    }
    auto res = RunCommand("smbcontrol all reload-config");
    if (!res.has_value()) return std::unexpected(res.error());
  }
  return Ok();
}

}  // namespace nas::storage
