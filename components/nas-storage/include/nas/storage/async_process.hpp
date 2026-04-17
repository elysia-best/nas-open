#pragma once

#include <string>
#include <vector>

namespace nas::storage {

struct AsyncProcess {
  std::string command;
  std::vector<std::string> args;
  int timeout_sec{30};
  bool dry_run{true};
};

}  // namespace nas::storage
