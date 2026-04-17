#include <cassert>
#include <vector>

#include "nas/cli/cmd_router.hpp"

int main() {
  nas::cli::CommandRouter router;

  auto bad = router.Parse({"storage", "pool"});
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kInvalidArgument);

  auto ok = router.Parse({"storage", "pool", "list", "--json"});
  assert(ok.has_value());
  assert(ok->name_space == "storage");
  assert(ok->entity == "pool");
  assert(ok->action == "list");
  assert(ok->flags.size() == 1);

  auto dry = router.FormatDryRun(*ok);
  assert(dry.has_value());
  assert(dry.value().find("DRY-RUN") == 0);

  return 0;
}
