#include "nas/monitor/cloud_sync.hpp"

namespace nas::monitor {

Result<void> CloudSync::Pause() {
  state_ = SyncState::kPaused;
  return Ok();
}

Result<void> CloudSync::Resume() {
  state_ = SyncState::kRunning;
  return Ok();
}

Result<void> CloudSync::Cancel() {
  state_ = SyncState::kCanceled;
  return Ok();
}

SyncState CloudSync::State() const { return state_; }

}  // namespace nas::monitor
