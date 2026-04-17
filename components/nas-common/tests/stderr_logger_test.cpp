#include <cassert>

#include "nas/core/stderr_logger.hpp"

int main() {
  nas::core::StderrLogger logger;

  assert(logger.AsyncLog(nas::core::LogLevel::kInfo, "test info").has_value());
  assert(logger.AsyncLog(nas::core::LogLevel::kWarn, "test warn").has_value());
  assert(logger.AsyncLog(nas::core::LogLevel::kError, "test error").has_value());
  assert(logger.Flush().has_value());

  return 0;
}
