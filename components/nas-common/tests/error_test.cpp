#include <cassert>
#include <string>

#include "nas/error.hpp"

int main() {
  {
    nas::Result<int> value = 42;
    assert(value.has_value());
    assert(value.value() == 42);
  }

  {
    nas::Result<int> value = nas::Fail(nas::ErrorCode::kInvalidArgument, "bad input");
    assert(!value.has_value());
    assert(value.error().code == nas::ErrorCode::kInvalidArgument);
    assert(value.error().message == "bad input");
  }

  {
    auto ok = nas::Ok();
    assert(ok.has_value());
  }

  return 0;
}
