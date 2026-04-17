#include "nas/gateway/rate_limiter.hpp"

namespace nas::gateway {

bool RateLimiter::Allow(const std::string& key) {
  auto now = std::chrono::steady_clock::now();
  auto& slot = slots_[key];
  if (slot.count == 0 || now - slot.begin >= window_) {
    slot.begin = now;
    slot.count = 1;
    return true;
  }
  if (slot.count < limit_) {
    ++slot.count;
    return true;
  }
  return false;
}

}  // namespace nas::gateway
