#include "nas/error.hpp"

namespace nas {
int ensure_library_linkage() {
  return static_cast<int>(ErrorCode::kOk);
}
}  // namespace nas
