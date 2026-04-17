#pragma once
#include <string>
#include <expected>

namespace nas::storage {

class NfsAclManager {
public:
    std::expected<void, std::string> set_acl(const std::string& path, const std::string& acl_spec, bool dry_run = false);
    std::expected<std::string, std::string> get_acl(const std::string& path, bool dry_run = false);
};

} // namespace nas::storage
