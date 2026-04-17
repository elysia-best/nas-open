#pragma once

#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::cli {

struct Command {
  std::string name_space;
  std::string entity;
  std::string action;
  std::vector<std::string> flags;
};

class CommandRouter {
 public:
  [[nodiscard]] Result<Command> Parse(const std::vector<std::string>& argv) const;
  [[nodiscard]] Result<std::string> FormatDryRun(const Command& cmd) const;
};

}  // namespace nas::cli
