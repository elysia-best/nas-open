#include "nas/storage/nfs_export.hpp"
#include "run_command.hpp"
#include <fstream>
#include <sstream>

namespace nas::storage {

std::expected<void, std::string> NfsExportManager::add_export(const NfsExportConfig& config) {
    std::ofstream out("/etc/exports.d/nas_" + config.path.substr(config.path.find_last_of('/') + 1) + ".exports");
    if (!out) return std::unexpected("Failed to open export file for writing");
    out << config.path << " " << config.clients << "(" << config.options << ")\n";
    return {};
}

std::expected<void, std::string> NfsExportManager::remove_export(const std::string& path) {
    std::string filename = "/etc/exports.d/nas_" + path.substr(path.find_last_of('/') + 1) + ".exports";
    auto res = detail::RunCommand("rm -f " + filename);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

std::expected<void, std::string> NfsExportManager::reload_exports() {
    auto res = detail::RunCommand("exportfs -ra");
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

} // namespace nas::storage
