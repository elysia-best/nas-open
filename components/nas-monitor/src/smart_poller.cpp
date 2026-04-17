#include "nas/monitor/smart_poller.hpp"

namespace nas::monitor {

Result<SmartStatus> SmartPoller::Poll(const std::string& device) const {
  if (device.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "device is empty");
  }
  return SmartStatus{.device = device, .health = "OK", .temperature_c = std::nullopt};
}

}  // namespace nas::monitor
