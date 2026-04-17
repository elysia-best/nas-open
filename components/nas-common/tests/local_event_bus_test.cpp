#include <cassert>
#include <string>

#include "nas/core/local_event_bus.hpp"

int main() {
  nas::core::LocalEventBus bus;

  std::string received;
  auto sub = bus.Subscribe("storage.events", [&](const nas::core::Event& ev) {
    received = ev.payload;
  });
  assert(sub.has_value());
  std::size_t id = sub.value();

  // Publish to subscribed topic
  assert(bus.Publish({"storage.events", "pool_created"}).has_value());
  assert(received == "pool_created");

  // Publish to non-subscribed topic — no crash, just no handler called
  received.clear();
  assert(bus.Publish({"other.topic", "ignored"}).has_value());
  assert(received.empty());

  // Unsubscribe
  assert(bus.Unsubscribe(id).has_value());

  // After unsubscribe, handler must not be called
  assert(bus.Publish({"storage.events", "should_be_ignored"}).has_value());
  assert(received.empty());

  // Unsubscribe non-existent
  auto bad = bus.Unsubscribe(9999);
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kNotFound);

  return 0;
}
