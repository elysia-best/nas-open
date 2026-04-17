#pragma once
#include <string>
#include <vector>
#include <expected>

namespace nas::storage {

struct BootEnvironment {
    std::string name;
    bool active;
    std::string creation_date;
};

class ZfsBEManager {
public:
    std::expected<void, std::string> create_be(const std::string& name);
    std::expected<void, std::string> activate_be(const std::string& name);
    std::expected<std::vector<BootEnvironment>, std::string> list_bes();
};

} // namespace nas::storage
