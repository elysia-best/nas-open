#pragma once

#include <string>
#include <unordered_map>

namespace nasd::core {

inline const std::unordered_map<std::string, std::string> kGrpcRouteMap = {
    {"StorageService.ListPools", "storage/list_pools"},
    {"StorageService.CreateSnapshot", "storage/create_snapshot"},
    {"MonitorService.GetSmartStatus", "monitor/get_smart_status"},
};

}  // namespace nasd::core
