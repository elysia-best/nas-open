#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::monitor {

class DiagnosticReport {
 public:
  [[nodiscard]] Result<std::string> GenerateHtml() const;
};

}  // namespace nas::monitor
