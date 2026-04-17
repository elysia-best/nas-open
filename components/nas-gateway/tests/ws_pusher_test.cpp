#include <cassert>
#include <string>

#include "nas/gateway/ws_pusher.hpp"

int main() {
  nas::gateway::WsPusher pusher;

  // Subscribe to channels
  assert(pusher.Subscribe("storage.events").has_value());
  assert(pusher.Subscribe("monitor.smart").has_value());

  // Duplicate subscription should succeed without adding duplicate
  assert(pusher.Subscribe("storage.events").has_value());
  auto subs = pusher.Subscriptions();
  assert(subs.has_value());
  assert(subs.value().size() == 2);  // no duplicate

  // Subscribe to empty channel fails
  auto bad = pusher.Subscribe("");
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kInvalidArgument);

  return 0;
}
