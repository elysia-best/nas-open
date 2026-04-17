#include "nasd/core/context_factory.hpp"

namespace nasd::core {

nasd::Context ContextFactory::Default() {
  nasd::Context ctx;
  ctx.logger = std::make_shared<nas::core::StderrLogger>();
  ctx.config = std::make_shared<nas::core::InMemoryConfigStore>();
  ctx.event_bus = std::make_shared<nas::core::LocalEventBus>();
  return ctx;
}

nas::Result<nasd::Context> ContextFactory::CreateWithConfig(const std::string& json_config) {
  auto ctx = Default();
  auto res = ctx.config->LoadJson(json_config);
  if (!res.has_value()) {
    return std::unexpected(res.error());
  }
  return ctx;
}

}  // namespace nasd::core
