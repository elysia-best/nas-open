#include <cassert>
#include <string>

#include "nasd/core/context_factory.hpp"
#include "nasd/core/plugin_loader.hpp"
#include "nasd/grpc/server.hpp"

int main() {
  // Default context must have all three services populated
  auto ctx = nasd::core::ContextFactory::Default();
  assert(ctx.logger != nullptr);
  assert(ctx.config != nullptr);
  assert(ctx.event_bus != nullptr);

  // CreateWithConfig with valid JSON
  auto ctx2 = nasd::core::ContextFactory::CreateWithConfig(
      R"({"daemon_port":"9000","log_level":"debug"})");
  assert(ctx2.has_value());
  assert(ctx2->config->Get("daemon_port").value() == "9000");

  // CreateWithConfig with invalid JSON
  auto bad = nasd::core::ContextFactory::CreateWithConfig("not json");
  assert(!bad.has_value());

  // Server wiring: Start/Stop must publish events to the event bus
  nasd::core::PluginLoader loader;
  std::string last_event;
  ctx.event_bus->Subscribe(nasd::grpc::kHealthzTopic,
                            [&](const nas::core::Event& ev) { last_event = ev.payload; });

  nasd::grpc::Server server(loader, ctx);
  assert(!server.Healthz().running);

  server.Start();
  assert(server.Healthz().running);
  assert(last_event.find("start") != std::string::npos);

  server.Stop();
  assert(!server.Healthz().running);
  assert(last_event.find("stop") != std::string::npos);

  return 0;
}
