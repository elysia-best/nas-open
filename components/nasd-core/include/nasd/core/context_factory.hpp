#pragma once

#include <memory>

#include "nas/core/inmemory_config_store.hpp"
#include "nas/core/local_event_bus.hpp"
#include "nas/core/stderr_logger.hpp"
#include "nasd/core/context.hpp"

namespace nasd::core {

// ContextFactory assembles a default Context using the concrete
// implementations from nas-common.  Tests and main() can call
// ContextFactory::Default() to obtain a ready-to-use Context.
class ContextFactory {
 public:
  ContextFactory() = delete;

  static nasd::Context Default();

  // CreateWithConfig returns a Context pre-loaded with the given flat-JSON
  // configuration string.  Returns an error if the JSON is invalid.
  static nas::Result<nasd::Context> CreateWithConfig(const std::string& json_config);
};

}  // namespace nasd::core
