#include "nas/error.hpp"

namespace nas {
int nas_common_link_anchor() {
  return static_cast<int>(ErrorCode::kOk);
}
}  // namespace nas
