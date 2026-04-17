#include <cassert>
#include "nas/storage/nfs_export.hpp"

using namespace nas::storage;

int main() {
    NfsExportManager manager;
    NfsExportConfig config{"/mnt/pool1/ds1", "*", "rw,sync"};
    auto res_add = manager.add_export(config);
    assert(res_add.has_value());

    auto res_reload = manager.reload_exports();
    assert(res_reload.has_value());

    auto res_rem = manager.remove_export("/mnt/pool1/ds1");
    assert(res_rem.has_value());
    return 0;
}
