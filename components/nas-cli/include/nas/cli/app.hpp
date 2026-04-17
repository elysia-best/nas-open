#pragma once

#include <string>
#include <vector>

#include "nas/error.hpp"

namespace nas::cli {

class App {
 public:
  [[nodiscard]] Result<std::string> Run(const std::vector<std::string>& args) const;
};

}  // namespace nas::cli
