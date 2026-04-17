#include <cassert>
#include <string>

#include "nas/core/inmemory_config_store.hpp"

int main() {
  nas::core::InMemoryConfigStore cfg;

  // Set/Get
  assert(cfg.Set("port", "8080").has_value());
  assert(cfg.Set("log_level", "info").has_value());

  auto port = cfg.Get("port");
  assert(port.has_value());
  assert(port.value() == "8080");

  // Missing key
  auto missing = cfg.Get("nonexistent");
  assert(!missing.has_value());
  assert(missing.error().code == nas::ErrorCode::kNotFound);

  // LoadJson
  auto load = cfg.LoadJson(R"({"host":"localhost","pool":"tank"})");
  assert(load.has_value());
  assert(cfg.Get("host").value() == "localhost");
  assert(cfg.Get("pool").value() == "tank");

  // DumpJson round-trip
  auto json = cfg.DumpJson();
  assert(json.has_value());
  assert(json.value().find("\"host\":\"localhost\"") != std::string::npos);

  // LoadYaml
  auto yaml_load = cfg.LoadYaml("key1: val1\nkey2: val2\n");
  assert(yaml_load.has_value());
  assert(cfg.Get("key1").value() == "val1");
  assert(cfg.Get("key2").value() == "val2");

  // Invalid JSON
  auto bad = cfg.LoadJson("not json");
  assert(!bad.has_value());

  // Empty key
  auto bad_set = cfg.Set("", "v");
  assert(!bad_set.has_value());

  return 0;
}
