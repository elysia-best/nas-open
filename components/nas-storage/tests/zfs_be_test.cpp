#include <cassert>
#include "nas/storage/zfs_be.hpp"

using namespace nas::storage;

int main() {
    ZfsBEManager manager;
    auto result = manager.create_be("new_be", true);
    assert(result.has_value());

    auto result2 = manager.activate_be("new_be", true);
    assert(result2.has_value());

    auto result3 = manager.list_bes(true);
    assert(result3.has_value());
    return 0;
}
