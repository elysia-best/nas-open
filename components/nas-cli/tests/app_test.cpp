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
  assert(out.find("DRY-RUN") == 0);
  assert(out.find("storage") != std::string::npos);
  assert(out.find("pool") != std::string::npos);
  assert(out.find("list") != std::string::npos);
  assert(out.find("--json") != std::string::npos);

  // Too few args
  auto bad = app.Run({"storage", "pool"});
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kInvalidArgument);

  // No args
  auto empty = app.Run({});
  assert(!empty.has_value());

  // Various namespaces
  auto monitor = app.Run({"monitor", "smart", "poll", "--dev=/dev/sda"});
  assert(monitor.has_value());
  assert(monitor.value().find("monitor") != std::string::npos);

  return 0;
}
