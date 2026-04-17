#include "nas/monitor/cloud_sync.hpp"
#include "run_command.hpp"
#include <iostream>

namespace nas::monitor {

Result<void> CloudSync::Pause() {
  state_ = SyncState::kPaused;
  auto res = detail::RunCommand("killall -STOP rclone 2>/dev/null");
  return Ok();
}

Result<void> CloudSync::Resume() {
  state_ = SyncState::kRunning;
  auto res = detail::RunCommand("killall -CONT rclone 2>/dev/null");
  return Ok();
}

Result<void> CloudSync::Cancel() {
  state_ = SyncState::kCanceled;
  auto res = detail::RunCommand("killall rclone 2>/dev/null");
  return Ok();
}

SyncState CloudSync::State() const {
  return state_;
}

}  // namespace nas::monitor
