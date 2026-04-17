#include <cassert>
#include <string>

#include "nas/gateway/auth_middleware.hpp"

int main() {
  nas::gateway::AuthMiddleware auth;

  // IssueToken for valid user
  std::string token;
  auto issue = auth.IssueToken("alice", &token);
  assert(issue.has_value());
  assert(!token.empty());

  // ValidateToken with valid token
  assert(auth.ValidateToken(token).has_value());

  // ValidateToken with invalid token
  auto bad_validate = auth.ValidateToken("garbage");
  assert(!bad_validate.has_value());
  assert(bad_validate.error().code == nas::ErrorCode::kPermissionDenied);

  // IssueToken with empty user
  std::string tok2;
  auto bad_issue = auth.IssueToken("", &tok2);
  assert(!bad_issue.has_value());
  assert(bad_issue.error().code == nas::ErrorCode::kInvalidArgument);

  // IssueToken with null token pointer
  auto bad_null = auth.IssueToken("bob", nullptr);
  assert(!bad_null.has_value());
  assert(bad_null.error().code == nas::ErrorCode::kInvalidArgument);

  return 0;
}
