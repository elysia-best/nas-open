#pragma once

#include <mutex>
#include <unordered_map>

#include "nas/core/event.hpp"

namespace nas::core {

class LocalEventBus final : public IEventBus {
 public:
  Result<void> Publish(const Event& event) override;
  Result<std::size_t> Subscribe(const std::string& topic, EventHandler handler) override;
  Result<void> Unsubscribe(std::size_t subscription_id) override;

 private:
  std::unordered_map<std::string, std::unordered_map<std::size_t, EventHandler>> subscribers_;
  std::size_t next_id_{1};
  mutable std::mutex mu_;
};

}  // namespace nas::core
