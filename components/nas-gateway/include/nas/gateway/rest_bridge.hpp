#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::gateway {

class RestBridge {
 public:
  [[nodiscard]] Result<std::string> Route(const std::string& operation_id,
                                          const std::string& json_body) const;
};

}  // namespace nas::gateway
