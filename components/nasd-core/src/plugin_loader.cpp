#include "nasd/core/plugin_loader.hpp"

#include <dlfcn.h>

namespace nasd::core {

PluginLoader::~PluginLoader() {
  std::lock_guard<std::mutex> lock(mu_);
  for (auto& [_, handle] : handles_) {
    dlclose(handle);
  }
  handles_.clear();
}

nas::Result<void> PluginLoader::Load(const std::string& path, nasd::Context* ctx) {
  std::lock_guard<std::mutex> lock(mu_);
  if (handles_.contains(path)) {
    return nas::Fail(nas::ErrorCode::kConflict, "plugin already loaded: " + path);
  }

  void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    return nas::Fail(nas::ErrorCode::kNotFound, "unable to load plugin: " + path);
  }

  auto* init = reinterpret_cast<PluginInitFn>(dlsym(handle, "nas_plugin_init"));
  if (!init) {
    dlclose(handle);
    return nas::Fail(nas::ErrorCode::kInvalidArgument, "missing symbol nas_plugin_init");
  }

  init(ctx);
  handles_.emplace(path, handle);
  return nas::Ok();
}

nas::Result<void> PluginLoader::Unload(const std::string& path) {
  std::lock_guard<std::mutex> lock(mu_);
  auto it = handles_.find(path);
  if (it == handles_.end()) {
    return nas::Fail(nas::ErrorCode::kNotFound, "plugin not loaded: " + path);
  }

  dlclose(it->second);
  handles_.erase(it);
  return nas::Ok();
}

nas::Result<void> PluginLoader::Reload(const std::string& path, nasd::Context* ctx) {
  std::lock_guard<std::mutex> lock(mu_);
  auto it = handles_.find(path);
  if (it != handles_.end()) {
    dlclose(it->second);
    handles_.erase(it);
  }

  void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    return nas::Fail(nas::ErrorCode::kNotFound, "unable to reload plugin: " + path);
  }

  auto* init = reinterpret_cast<PluginInitFn>(dlsym(handle, "nas_plugin_init"));
  if (!init) {
    dlclose(handle);
    return nas::Fail(nas::ErrorCode::kInvalidArgument, "missing symbol nas_plugin_init");
  }

  init(ctx);
  handles_.emplace(path, handle);
  return nas::Ok();
}

std::vector<PluginInfo> PluginLoader::LoadedPlugins() const {
  std::lock_guard<std::mutex> lock(mu_);
  std::vector<PluginInfo> result;
  result.reserve(handles_.size());
  for (const auto& [path, _] : handles_) {
    result.push_back(PluginInfo{.path = path, .loaded = true});
  }
  return result;
}

}  // namespace nasd::core
