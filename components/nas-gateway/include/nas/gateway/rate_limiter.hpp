#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

namespace nas::gateway {

class RateLimiter {
 public:
  RateLimiter(std::size_t limit, std::chrono::seconds window)
      : limit_(limit), window_(window) {}

  bool Allow(const std::string& key);

 private:
  struct Slot {
    std::size_t count{0};
    std::chrono::steady_clock::time_point begin;
  };

  std::size_t limit_;
  std::chrono::seconds window_;
  std::unordered_map<std::string, Slot> slots_;
};

}  // namespace nas::gateway
