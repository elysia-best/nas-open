#include <cassert>
#include "nas/storage/nfs_acl.hpp"

using namespace nas::storage;

int main() {
    NfsAclManager manager;
    auto set_res = manager.set_acl("/tmp/test", "A::OWNER@:rwaDxtTnNcCy");
    assert(set_res.has_value());
    auto get_res = manager.get_acl("/tmp/test");
    assert(get_res.has_value());
    return 0;
}
