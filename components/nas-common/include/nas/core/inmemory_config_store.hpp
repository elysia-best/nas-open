#pragma once

#include <map>
#include <mutex>

#include "nas/core/config.hpp"

namespace nas::core {

class InMemoryConfigStore final : public IConfigStore {
 public:
  Result<void> LoadJson(const std::string& json) override;
  Result<void> LoadYaml(const std::string& yaml) override;
  Result<std::string> DumpJson() const override;
  Result<std::string> Get(const std::string& key) const override;
  Result<void> Set(const std::string& key, const std::string& value) override;

 private:
  std::map<std::string, std::string> data_;
  mutable std::mutex mu_;
};

}  // namespace nas::core
