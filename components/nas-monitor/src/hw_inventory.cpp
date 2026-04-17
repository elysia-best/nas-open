#include "nas/monitor/hw_inventory.hpp"
#include "run_command.hpp"
#include <regex>

namespace nas::monitor {

std::vector<HwItem> HardwareInventory::Collect() const {
  std::vector<HwItem> items;
  
  // CPU
  auto cpu_res = detail::RunCommand("lscpu | grep 'Model name:'");
  if (cpu_res.has_value()) {
      std::regex re("Model name:\\s+(.+)");
      std::smatch match;
      std::string val = cpu_res.value();
      if (std::regex_search(val, match, re) && match.size() > 1) {
          items.push_back({"cpu", match.str(1)});
      }
  } else {
      items.push_back({"cpu", std::nullopt});
  }

  // Memory
  auto mem_res = detail::RunCommand("free -h | awk '/^Mem:/ {print $2}'");
  if (mem_res.has_value()) {
      std::string val = mem_res.value();
      val.erase(val.find_last_not_of(" \n\r\t") + 1);
      items.push_back({"memory", val});
  } else {
      items.push_back({"memory", std::nullopt});
  }

  // NIC
  auto nic_res = detail::RunCommand("lspci | grep -i ether");
  if (nic_res.has_value()) {
      std::string val = nic_res.value();
      val.erase(val.find_last_not_of(" \n\r\t") + 1);
      items.push_back({"nic", val.empty() ? std::nullopt : std::make_optional(val)});
  } else {
      items.push_back({"nic", std::nullopt});
  }

  // Disk
  auto disk_res = detail::RunCommand("lsblk -d -o NAME,SIZE,MODEL | grep -v 'loop\\|NAME'");
  if (disk_res.has_value()) {
      std::string val = disk_res.value();
      val.erase(val.find_last_not_of(" \n\r\t") + 1);
      items.push_back({"disk", val.empty() ? std::nullopt : std::make_optional(val)});
  } else {
      items.push_back({"disk", std::nullopt});
  }

  return items;
}

}  // namespace nas::monitor
