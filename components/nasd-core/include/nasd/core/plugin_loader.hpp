#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "nas/error.hpp"
#include "nasd/core/context.hpp"

namespace nasd::core {

using PluginInitFn = void (*)(nasd::Context* ctx);

struct PluginInfo {
  std::string path;
  bool loaded{false};
};

class PluginLoader {
 public:
  PluginLoader() = default;
  ~PluginLoader();

  PluginLoader(const PluginLoader&) = delete;
  PluginLoader& operator=(const PluginLoader&) = delete;

  nas::Result<void> Load(const std::string& path, nasd::Context* ctx);
  nas::Result<void> Unload(const std::string& path);
  nas::Result<void> Reload(const std::string& path, nasd::Context* ctx);
  [[nodiscard]] std::vector<PluginInfo> LoadedPlugins() const;

 private:
  std::unordered_map<std::string, void*> handles_;
  mutable std::mutex mu_;
};

}  // namespace nasd::core
