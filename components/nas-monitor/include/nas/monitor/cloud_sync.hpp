#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::monitor {

enum class SyncState { kRunning, kPaused, kCanceled };

class CloudSync {
 public:
  [[nodiscard]] Result<void> Pause();
  [[nodiscard]] Result<void> Resume();
  [[nodiscard]] Result<void> Cancel();
  [[nodiscard]] SyncState State() const;

 private:
  SyncState state_{SyncState::kRunning};
};

}  // namespace nas::monitor
