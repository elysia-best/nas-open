#include <cassert>
#include "nas/storage/zfs_snapshot.hpp"

using namespace nas::storage;

int main() {
    ZfsSnapshotManager manager;
    auto result = manager.create_snapshot("pool/dataset", "snap1");
    assert(result.has_value());

    auto result2 = manager.send_snapshot("pool/ds@snap1", "ssh user@host zfs recv backup/ds");
    assert(result2.has_value());
    return 0;
}
