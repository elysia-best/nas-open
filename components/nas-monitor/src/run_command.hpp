#pragma once
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include "nas/error.hpp"

namespace nas::monitor::detail {
inline Result<std::string> RunCommand(const std::string& cmd) {
  std::FILE* pipe = ::popen(cmd.c_str(), "r");
  if (!pipe) return Fail(ErrorCode::kInternal, "popen failed");
  std::string output;
  std::array<char, 256> buf;
  while (std::fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr) {
    output += buf.data();
  }
  int status = ::pclose(pipe);
  if (status != 0) {
    return Fail(ErrorCode::kInternal, "command failed: " + cmd);
  }
  return output;
}
}
