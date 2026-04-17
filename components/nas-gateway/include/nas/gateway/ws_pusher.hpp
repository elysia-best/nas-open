#pragma once

#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::gateway {

class WsPusher {
 public:
  [[nodiscard]] Result<void> Subscribe(const std::string& channel);
  [[nodiscard]] Result<std::vector<std::string>> Subscriptions() const;

 private:
  std::vector<std::string> channels_;
};

}  // namespace nas::gateway
