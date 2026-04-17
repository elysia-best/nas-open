#include "nasd/grpc/server.hpp"

namespace nasd::grpc {

Server::Server(nasd::core::PluginLoader& loader) : loader_(loader) {}

void Server::Start() { running_.store(true); }

void Server::Stop() { running_.store(false); }

void Server::SetQueueDepth(std::size_t depth) { queue_depth_.store(depth); }

HealthzStatus Server::Healthz() const {
  return HealthzStatus{
      .plugin_count = loader_.LoadedPlugins().size(),
      .queue_depth = queue_depth_.load(),
      .running = running_.load(),
  };
}

}  // namespace nasd::grpc
