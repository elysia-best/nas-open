#include "nas/cli/cmd_router.hpp"

#include <sstream>

namespace nas::cli {

Result<Command> CommandRouter::Parse(const std::vector<std::string>& argv) const {
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
  if (cmd.name_space.empty() || cmd.entity.empty() || cmd.action.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "invalid command segments");
  }

  std::ostringstream oss;
  oss << "DRY-RUN: " << cmd.name_space << ' ' << cmd.entity << ' ' << cmd.action;
  for (const auto& flag : cmd.flags) {
    oss << ' ' << flag;
  }
  return oss.str();
}

}  // namespace nas::cli
