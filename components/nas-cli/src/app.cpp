#include "nas/cli/app.hpp"

#include "nas/cli/cmd_router.hpp"

namespace nas::cli {

Result<std::string> App::Run(const std::vector<std::string>& args) const {
  CommandRouter router;
  auto parsed = router.Parse(args);
  if (!parsed.has_value()) {
    return std::unexpected(parsed.error());
  }
  return router.Execute(parsed.value());
}

}  // namespace nas::cli
