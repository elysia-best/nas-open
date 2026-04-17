#pragma once

#include <memory>

#include "nas/core/config.hpp"
#include "nas/core/event.hpp"
#include "nas/core/logger.hpp"

namespace nasd {

struct Context {
  std::shared_ptr<nas::core::ILogger> logger;
  std::shared_ptr<nas::core::IConfigStore> config;
  std::shared_ptr<nas::core::IEventBus> event_bus;
};

}  // namespace nasd
