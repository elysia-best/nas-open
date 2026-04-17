#include "nasd/grpc/server.hpp"

#include <string>

namespace nasd::grpc {

Server::Server(nasd::core::PluginLoader& loader, nasd::Context ctx)
    : loader_(loader), ctx_(std::move(ctx)) {}

void Server::Start() {
  running_.store(true);
  if (ctx_.logger) {
    ctx_.logger->AsyncLog(nas::core::LogLevel::kInfo, "nasd server started");
  }
  if (ctx_.event_bus) {
    ctx_.event_bus->Publish({kHealthzTopic, R"({"event":"start","running":true})"});
  }
}

void Server::Stop() {
  running_.store(false);
  if (ctx_.logger) {
    ctx_.logger->AsyncLog(nas::core::LogLevel::kInfo, "nasd server stopped");
  }
  if (ctx_.event_bus) {
    ctx_.event_bus->Publish({kHealthzTopic, R"({"event":"stop","running":false})"});
  }
}

void Server::SetQueueDepth(std::size_t depth) {
  queue_depth_.store(depth);
}

HealthzStatus Server::Healthz() const {
  return HealthzStatus{
      .plugin_count = loader_.LoadedPlugins().size(),
      .queue_depth = queue_depth_.load(),
      .running = running_.load(),
  };
}

}  // namespace nasd::grpc
