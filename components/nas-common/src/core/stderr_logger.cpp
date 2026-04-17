#include "nas/core/stderr_logger.hpp"

#include <iostream>

namespace nas::core {

static const char* LevelTag(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace:    return "TRACE";
    case LogLevel::kDebug:    return "DEBUG";
    case LogLevel::kInfo:     return "INFO ";
    case LogLevel::kWarn:     return "WARN ";
    case LogLevel::kError:    return "ERROR";
    case LogLevel::kCritical: return "CRIT ";
    default:                  return "?    ";
  }
}

Result<void> StderrLogger::AsyncLog(LogLevel level, const std::string& message) {
  std::lock_guard<std::mutex> lock(mu_);
  std::cerr << "[" << LevelTag(level) << "] " << message << '\n';
  return Ok();
}

Result<void> StderrLogger::Flush() {
  std::lock_guard<std::mutex> lock(mu_);
  std::cerr.flush();
  return Ok();
}

}  // namespace nas::core
