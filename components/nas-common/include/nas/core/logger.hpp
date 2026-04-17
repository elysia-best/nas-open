#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::core {

enum class LogLevel { kTrace, kDebug, kInfo, kWarn, kError, kCritical };

class ILogger {
 public:
  virtual ~ILogger() = default;

  virtual Result<void> AsyncLog(LogLevel level, const std::string& message) = 0;
  virtual Result<void> Flush() = 0;
};

}  // namespace nas::core
