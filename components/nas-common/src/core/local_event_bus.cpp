#include "nas/core/local_event_bus.hpp"

namespace nas::core {

Result<void> LocalEventBus::Publish(const Event& event) {
  std::lock_guard<std::mutex> lock(mu_);
  auto it = subscribers_.find(event.topic);
  if (it != subscribers_.end()) {
    for (const auto& [id, handler] : it->second) {
      handler(event);
    }
  }
  return Ok();
}

Result<std::size_t> LocalEventBus::Subscribe(const std::string& topic, EventHandler handler) {
  std::lock_guard<std::mutex> lock(mu_);
  std::size_t id = next_id_++;
  subscribers_[topic][id] = std::move(handler);
  return id;
}

Result<void> LocalEventBus::Unsubscribe(std::size_t subscription_id) {
  std::lock_guard<std::mutex> lock(mu_);
  for (auto& [topic, handlers] : subscribers_) {
    if (handlers.erase(subscription_id) != 0) {
      return Ok();
    }
  }
  return Fail(ErrorCode::kNotFound, "subscription not found");
}

}  // namespace nas::core
