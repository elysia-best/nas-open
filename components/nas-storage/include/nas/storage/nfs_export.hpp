#pragma once
#include <string>
#include <vector>
#include <expected>

namespace nas::storage {

struct NfsExportConfig {
    std::string path;
    std::string clients;
    std::string options;
};

class NfsExportManager {
public:
    std::expected<void, std::string> add_export(const NfsExportConfig& config, bool dry_run = false);
    std::expected<void, std::string> remove_export(const std::string& path, bool dry_run = false);
    std::expected<void, std::string> reload_exports(bool dry_run = false);
};

} // namespace nas::storage
