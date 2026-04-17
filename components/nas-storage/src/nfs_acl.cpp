#include "nas/storage/nfs_acl.hpp"
#include "run_command.hpp"

namespace nas::storage {

std::expected<void, std::string> NfsAclManager::set_acl(const std::string& path, const std::string& acl_spec, bool dry_run) {
    if (dry_run) return {};
    auto res = detail::RunCommand("nfs4_setfacl -a " + acl_spec + " " + path);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return {};
}

std::expected<std::string, std::string> NfsAclManager::get_acl(const std::string& path, bool dry_run) {
    if (dry_run) return "A::OWNER@:rwaDxtTnNcCy";
    auto res = detail::RunCommand("nfs4_getfacl " + path);
    if (!res.has_value()) return std::unexpected(res.error().message);
    return res.value();
}

} // namespace nas::storage
