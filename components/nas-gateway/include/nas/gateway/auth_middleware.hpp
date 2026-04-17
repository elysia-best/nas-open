#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::gateway {

class AuthMiddleware {
 public:
  [[nodiscard]] Result<void> IssueToken(const std::string& user, std::string* token) const;
  [[nodiscard]] Result<void> ValidateToken(const std::string& token) const;
};

}  // namespace nas::gateway
