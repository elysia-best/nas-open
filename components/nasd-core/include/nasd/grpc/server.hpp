#pragma once

#include <atomic>
#include <cstddef>

#include "nasd/core/context.hpp"
#include "nasd/core/plugin_loader.hpp"

namespace nasd::grpc {

static constexpr const char* kHealthzTopic = "nasd.healthz";

struct HealthzStatus {
  std::size_t plugin_count{0};
  std::size_t queue_depth{0};
  bool running{false};
};

class Server {
 public:
  explicit Server(nasd::core::PluginLoader& loader, nasd::Context ctx);

  void Start();
  void Stop();
  void SetQueueDepth(std::size_t depth);
  [[nodiscard]] HealthzStatus Healthz() const;

 private:
  nasd::core::PluginLoader& loader_;
  nasd::Context ctx_;
  std::atomic<bool> running_{false};
  std::atomic<std::size_t> queue_depth_{0};
};

}  // namespace nasd::grpc
