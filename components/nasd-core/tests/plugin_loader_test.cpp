#include <cassert>

#include "nasd/core/plugin_loader.hpp"

int main() {
  nasd::core::PluginLoader loader;
  nasd::Context ctx;

  auto res = loader.Load("/definitely/not/exist.so", &ctx);
  assert(!res.has_value());
  assert(res.error().code == nas::ErrorCode::kNotFound);

  auto unload = loader.Unload("/definitely/not/exist.so");
  assert(!unload.has_value());
  assert(unload.error().code == nas::ErrorCode::kNotFound);

  return 0;
}
