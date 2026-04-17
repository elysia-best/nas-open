#include "nas/monitor/diag_report.hpp"
#include "run_command.hpp"

namespace nas::monitor {

Result<std::string> DiagnosticReport::GenerateHtml() const {
  std::string html = "<html><head><title>NAS Diagnostic Report</title>";
  html += "<style>body{font-family: monospace;}</style></head><body>";
  html += "<h1>System Diagnostic Report</h1>";

  // System
  html += "<h2>System Info</h2><pre>";
  if (auto res = detail::RunCommand("uname -a"); res) html += res.value();
  html += "</pre>";

  // dmesg
  html += "<h2>Kernel Ring Buffer (dmesg | tail)</h2><pre>";
  if (auto res = detail::RunCommand("dmesg | tail -n 20"); res) html += res.value();
  html += "</pre>";

  // disk
  html += "<h2>Disks (lsblk)</h2><pre>";
  if (auto res = detail::RunCommand("lsblk"); res) html += res.value();
  html += "</pre>";
  
  // zpool
  html += "<h2>ZFS Pool Status</h2><pre>";
  if (auto res = detail::RunCommand("zpool status 2>&1"); res) html += res.value();
  else html += "zpool command failed or not installed.";
  html += "</pre>";

  html += "</body></html>";
  return html;
}

}  // namespace nas::monitor
