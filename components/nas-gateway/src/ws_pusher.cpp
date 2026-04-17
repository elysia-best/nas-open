#include "nas/gateway/ws_pusher.hpp"

#include <algorithm>

namespace nas::gateway {

Result<void> WsPusher::Subscribe(const std::string& channel) {
  if (channel.empty()) {
    return Fail(ErrorCode::kInvalidArgument, "channel is empty");
  }
  if (std::find(channels_.begin(), channels_.end(), channel) == channels_.end()) {
    channels_.push_back(channel);
  }
  return Ok();
}

Result<std::vector<std::string>> WsPusher::Subscriptions() const { return channels_; }

}  // namespace nas::gateway
