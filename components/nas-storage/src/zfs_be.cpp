#include "nas/storage/zfs_be.hpp"
#include "run_command.hpp"
#include <iostream>
#include <sstream>

namespace nas::storage {

std::expected<void, std::string> ZfsBEManager::create_be(const std::string& name, bool dry_run) {
    if (dry_run) return {};
    auto res = detail::RunCommand("zfs clone -o canmount=noauto pool/ROOT/default@current pool/ROOT/" + name);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

std::expected<void, std::string> ZfsBEManager::activate_be(const std::string& name, bool dry_run) {
    if (dry_run) return {};
    auto res = detail::RunCommand("zpool set bootfs=pool/ROOT/" + name + " pool");
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

std::expected<std::vector<BootEnvironment>, std::string> ZfsBEManager::list_bes(bool dry_run) {
    std::vector<BootEnvironment> bes;
    if (dry_run) {
        bes.push_back({"default", true, "2024-01-01"});
        bes.push_back({"new_be", false, "2024-01-02"});
        return bes;
    }
    auto res = detail::RunCommand("zfs list -H -o name -t filesystem | grep pool/ROOT/");
    if (!res.has_value()) return std::unexpected(res.error().message);
    std::istringstream stream(res.value());
    std::string line;
    while(std::getline(stream, line)) {
        BootEnvironment be;
        be.name = line;
        be.active = false;
        bes.push_back(be);
    }
    return bes;
}

} // namespace nas::storage
