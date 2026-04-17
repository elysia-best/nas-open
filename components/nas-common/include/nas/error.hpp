#pragma once

#include <expected>
#include <string>
#include <utility>

namespace nas {

enum class ErrorCode {
  kOk = 0,
  kInvalidArgument,
  kNotFound,
  kPermissionDenied,
  kConflict,
  kTimeout,
  kInternal
};

struct Error {
  ErrorCode code{ErrorCode::kOk};
  std::string message;

  [[nodiscard]] bool ok() const noexcept { return code == ErrorCode::kOk; }
};

template <typename T>
using Result = std::expected<T, Error>;

inline Result<void> Ok() { return {}; }

inline std::unexpected<Error> Fail(ErrorCode code, std::string message) {
  return std::unexpected<Error>{Error{code, std::move(message)}};
}

}  // namespace nas
