#pragma once

#include <functional>
#include <string>

#include "nas/error.hpp"

namespace nas::core {

struct Event {
  std::string topic;
  std::string payload;
};

using EventHandler = std::function<void(const Event&)>;

class IEventBus {
 public:
  virtual ~IEventBus() = default;

  virtual Result<void> Publish(const Event& event) = 0;
  virtual Result<std::size_t> Subscribe(const std::string& topic, EventHandler handler) = 0;
  virtual Result<void> Unsubscribe(std::size_t subscription_id) = 0;
};

}  // namespace nas::core
