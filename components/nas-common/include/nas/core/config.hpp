#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::core {

class IConfigStore {
 public:
  virtual ~IConfigStore() = default;

  virtual Result<void> LoadJson(const std::string& json) = 0;
  virtual Result<void> LoadYaml(const std::string& yaml) = 0;
  virtual Result<std::string> DumpJson() const = 0;
};

}  // namespace nas::core
