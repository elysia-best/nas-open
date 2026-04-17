#include <cassert>
#include <string>

#include "nas/gateway/rest_bridge.hpp"

int main() {
  nas::gateway::RestBridge bridge;

  // Valid route
  auto result = bridge.Route("ListPools", R"({"page":1})");
  assert(result.has_value());
  assert(result.value().find("ListPools") != std::string::npos);
  assert(result.value().find("page") != std::string::npos);

  // Empty operation_id
  auto bad = bridge.Route("", R"({})");
  assert(!bad.has_value());
  assert(bad.error().code == nas::ErrorCode::kInvalidArgument);

  // Route with empty body still succeeds (body may be empty for GET-like ops)
  auto empty_body = bridge.Route("GetSmartStatus", "{}");
  assert(empty_body.has_value());

  return 0;
}
