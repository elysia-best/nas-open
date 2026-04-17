#pragma once

// Internal-only helper shared by storage implementation files.
// Not part of the public nas-storage API.

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

#include "nas/error.hpp"

namespace nas::storage::detail {

// Run a shell command and capture its stdout.
// Returns the captured output on success (exit-status 0).
// Returns an Error{kInternal} on popen failure or non-zero exit.
inline Result<std::string> RunCommand(const std::string& cmd) {
  // NOLINTNEXTLINE(cert-env33-c)
  std::FILE* pipe = ::popen(cmd.c_str(), "r");
  if (!pipe) {
    return Fail(ErrorCode::kInternal,
                std::string("popen failed: ") + std::strerror(errno));
  }

  std::string output;
  std::array<char, 256> buf{};
  while (std::fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr) {
    output += buf.data();
  }

  int status = ::pclose(pipe);
  if (status != 0) {
    return Fail(ErrorCode::kInternal,
                "command failed (exit " + std::to_string(status) + "): " + cmd);
  }
  return output;
}

}  // namespace nas::storage::detail
