#include <cassert>
#include <string>
#include <vector>

#include "nas/cli/app.hpp"

int main() {
  nas::cli::App app;

  // Successful invocation
  auto ok = app.Run({"storage", "pool", "list", "--json"});
  assert(ok.has_value());
  const auto& out = ok.value();
  assert(out.find("STORAGE:") == 0);
  assert(out.find("pool") != std::string::npos);
  assert(out.find("list") != std::string::npos);
  assert(out.find("--json") != std::string::npos);

  // Too few args
  auto bad = app.Run({"storage", "pool"});
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kInvalidArgument);

  // Help command for storage
  auto s_help = app.Run({"storage", "help"});
  assert(s_help.has_value());
  assert(s_help.value().find("Storage commands:") != std::string::npos);

  // Help command for monitor
  auto m_help = app.Run({"monitor", "help"});
  assert(m_help.has_value());
  assert(m_help.value().find("Monitor commands:") != std::string::npos);

  // No args
  auto empty = app.Run({});
  assert(!empty.has_value());

  // Various namespaces
  auto monitor = app.Run({"monitor", "smart", "poll", "/dev/sda"});
  assert(monitor.has_value());
  assert(monitor.value().find("Device: /dev/sda") != std::string::npos);

  return 0;
}
