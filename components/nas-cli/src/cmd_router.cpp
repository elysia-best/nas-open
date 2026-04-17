#include "nas/cli/cmd_router.hpp"

#include <sstream>
#include "nas/monitor/cloud_sync.hpp"
#include "nas/monitor/diag_report.hpp"
#include "nas/monitor/hw_inventory.hpp"
#include "nas/monitor/smart_poller.hpp"
#include "nas/storage/partition.hpp"

namespace nas::cli {

Result<Command> CommandRouter::Parse(const std::vector<std::string>& argv) const {
  if (argv.empty() || argv.size() < 2) {
    return Fail(ErrorCode::kInvalidArgument,
                "command must be: <namespace> <entity> <action> [flags]");
  }

  if (argv.size() == 2 && argv[1] == "help") {
    return Command{.name_space = argv[0], .entity = argv[1], .action = ""};
  }

  if (argv.size() < 3) {
    return Fail(ErrorCode::kInvalidArgument,
                "command must be: <namespace> <entity> <action> [flags]");
  }

  Command cmd{.name_space = argv[0], .entity = argv[1], .action = argv[2]};
  for (std::size_t i = 3; i < argv.size(); ++i) {
    cmd.flags.push_back(argv[i]);
  }
  return cmd;
}

Result<std::string> CommandRouter::FormatDryRun(const Command& cmd) const {
  if (cmd.name_space.empty() || cmd.entity.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "invalid command segments");
  }

  std::ostringstream oss;
  if (cmd.name_space == "storage") {
      oss << "STORAGE: executing " << cmd.entity << " action " << cmd.action;
  } else {
      oss << "DRY-RUN: " << cmd.name_space << ' ' << cmd.entity << ' ' << cmd.action;
  }
  for (const auto& flag : cmd.flags) {
    oss << ' ' << flag;
  }
  return oss.str();
}

Result<std::string> CommandRouter::Execute(const Command& cmd) const {
  if (cmd.name_space == "monitor") {
      if (cmd.entity == "help") {
          return "Monitor commands:\n"
                 "  monitor hw collect           - Collect hardware inventory\n"
                 "  monitor diag generate        - Generate diagnostic report\n"
                 "  monitor smart poll <device>  - Poll SMART status\n"
                 "  monitor cloud <pause|resume|cancel|state> - Manage cloud sync\n";
      }
      if (cmd.entity == "hw" && cmd.action == "collect") {
          nas::monitor::HardwareInventory inv;
          auto items = inv.Collect();
          std::ostringstream oss;
          for (const auto& item : items) {
              oss << item.name << ": " << item.value.value_or("N/A") << "\n";
          }
          return oss.str();
      } else if (cmd.entity == "diag" && cmd.action == "generate") {
          nas::monitor::DiagnosticReport diag;
          auto res = diag.GenerateHtml();
          if (!res) return std::unexpected(res.error());
          return res.value();
      } else if (cmd.entity == "smart" && cmd.action == "poll") {
          if (cmd.flags.empty()) return Fail(ErrorCode::kInvalidArgument, "disk device missing, use: monitor smart poll <device>");
          nas::monitor::SmartPoller smart;
          auto res = smart.Poll(cmd.flags[0]);
          if (!res) return std::unexpected(res.error());
          std::ostringstream oss;
          oss << "Device: " << res.value().device << "\n"
              << "Health: " << res.value().health << "\n"
              << "Temp(C): ";
          if (res.value().temperature_c) oss << *res.value().temperature_c;
          else oss << "N/A";
          oss << "\n";
          return oss.str();
      } else if (cmd.entity == "cloud") {
          nas::monitor::CloudSync sync;
          if (cmd.action == "pause") {
              auto res = sync.Pause();
              if (res) return "Paused";
              return std::unexpected(res.error());
          } else if (cmd.action == "resume") {
              auto res = sync.Resume();
              if (res) return "Resumed";
              return std::unexpected(res.error());
          } else if (cmd.action == "cancel") {
              auto res = sync.Cancel();
              if (res) return "Canceled";
              return std::unexpected(res.error());
          } else if (cmd.action == "state") {
              auto st = sync.State();
              if (st == nas::monitor::SyncState::kRunning) return "Running";
              if (st == nas::monitor::SyncState::kPaused) return "Paused";
              return "Canceled";
          }
      }
      return Fail(ErrorCode::kInvalidArgument, "unknown monitor entity/action");
  } else if (cmd.name_space == "storage") {
      if (cmd.entity == "help") {
          return "Storage commands:\n"
                 "  storage pool <list|create|destroy> - Manage zfs storage pools\n"
                 "  storage disk <list|format>         - Manage hard disks\n"
                 "  storage share <create|delete>      - Manage samba/nfs shares\n"
                 "  storage snap <create|rollback>     - Manage zfs snapshots\n";
      } else if (cmd.entity == "disk" && cmd.action == "list") {
          nas::storage::PartitionManager pm;
          auto res = pm.ProbeTopologyJson(false);
          if (!res) return std::unexpected(res.error());
          return res.value();
      }
      // Delegate to Storage's Execute or handle mock for now
  }
  return FormatDryRun(cmd);
}

}  // namespace nas::cli
