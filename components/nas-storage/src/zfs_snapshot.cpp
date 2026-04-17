#include "nas/storage/zfs_snapshot.hpp"
#include "run_command.hpp"

namespace nas::storage {

std::expected<void, std::string> ZfsSnapshotManager::create_snapshot(const std::string& dataset, const std::string& snap_name, bool dry_run) {
    if (dry_run) return {};
    auto res = detail::RunCommand("zfs snapshot " + dataset + "@" + snap_name);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

std::expected<void, std::string> ZfsSnapshotManager::send_snapshot(const std::string& snapshot, const std::string& target, bool incremental, bool dry_run) {
    if (dry_run) return {};
    std::string cmd = incremental ? "zfs send -i " : "zfs send ";
    cmd += snapshot + " | " + target;
    auto res = detail::RunCommand(cmd);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

} // namespace nas::storage
