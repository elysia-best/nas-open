#include "nas/gateway/rest_bridge.hpp"

namespace nas::gateway {

Result<std::string> RestBridge::Route(const std::string& operation_id,
                                      const std::string& json_body) const {
  if (operation_id.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "operation_id is empty");
  }
  return std::string{"{\"operation\":\""} + operation_id + "\",\"payload\":" +
         json_body + "}";
}

}  // namespace nas::gateway
