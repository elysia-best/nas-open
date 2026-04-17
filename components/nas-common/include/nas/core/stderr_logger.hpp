#pragma once

#include <mutex>

#include "nas/core/logger.hpp"

namespace nas::core {

class StderrLogger final : public ILogger {
 public:
  Result<void> AsyncLog(LogLevel level, const std::string& message) override;
  Result<void> Flush() override;

 private:
  mutable std::mutex mu_;
};

}  // namespace nas::core
