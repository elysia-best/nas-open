#include "nas/gateway/auth_middleware.hpp"

namespace nas::gateway {

Result<void> AuthMiddleware::IssueToken(const std::string& user, std::string* token) const {
  if (user.empty() || token == nullptr) {
    return Fail(ErrorCode::kInvalidArgument, "invalid token issue request");
  }
  *token = "token:" + user;
  return Ok();
}

Result<void> AuthMiddleware::ValidateToken(const std::string& token) const {
  if (token.rfind("token:", 0) != 0) {
    return Fail(ErrorCode::kPermissionDenied, "invalid token");
  }
  return Ok();
}

}  // namespace nas::gateway
