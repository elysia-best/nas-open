#pragma once
#include <string>
#include <expected>

namespace nas::storage {

class ZfsSnapshotManager {
public:
    std::expected<void, std::string> create_snapshot(const std::string& dataset, const std::string& snap_name);
    std::expected<void, std::string> send_snapshot(const std::string& snapshot, const std::string& target, bool incremental = false);
};

} // namespace nas::storage
