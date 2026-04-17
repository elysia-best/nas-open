#include <cassert>
#include <chrono>

#include "nas/gateway/rate_limiter.hpp"

int main() {
  nas::gateway::RateLimiter limiter(2, std::chrono::seconds(60));
  assert(limiter.Allow("u1"));
  assert(limiter.Allow("u1"));
  assert(!limiter.Allow("u1"));
  assert(limiter.Allow("u2"));
  return 0;
}
