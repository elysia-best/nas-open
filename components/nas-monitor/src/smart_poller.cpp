#include "nas/monitor/smart_poller.hpp"
#include "run_command.hpp"
#include <regex>

namespace nas::monitor {

Result<SmartStatus> SmartPoller::Poll(const std::string& device) const {
  if (device.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "device is empty");
  }
  
  auto res = detail::RunCommand("smartctl -a " + device);
  if (!res.has_value()) {
      return SmartStatus{.device = device, .health = "FAILED", .temperature_c = std::nullopt};
  }
  
  std::string output = res.value();
  std::string health = "UNKNOWN";
  std::optional<int> temp = std::nullopt;
  
  if (output.find("SMART overall-health self-assessment test result: PASSED") != std::string::npos) {
      health = "OK";
  } else {
      health = "WARNING";
  }
  
  std::regex temp_regex("Temperature:\\s+(\\d+)\\s+Celsius");
  std::smatch match;
  if (std::regex_search(output, match, temp_regex) && match.size() > 1) {
      temp = std::stoi(match.str(1));
  } else {
      std::regex temp_regex2("Temperature_Celsius[^0-9]+(\\d+)");
      if (std::regex_search(output, match, temp_regex2) && match.size() > 1) {
          temp = std::stoi(match.str(1));
      }
  }

  return SmartStatus{.device = device, .health = health, .temperature_c = temp};
}

}  // namespace nas::monitor
