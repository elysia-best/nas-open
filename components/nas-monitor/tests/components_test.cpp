#include <cassert>

#include "nas/monitor/cloud_sync.hpp"
#include "nas/monitor/diag_report.hpp"
#include "nas/monitor/hw_inventory.hpp"
#include "nas/monitor/smart_poller.hpp"

int main() {
  nas::monitor::SmartPoller smart;
  auto bad = smart.Poll("");
  assert(!bad.has_value());
  auto ok = smart.Poll("/dev/sda");
  assert(ok.has_value());

  nas::monitor::CloudSync sync;
  assert(sync.Pause().has_value());
  assert(sync.State() == nas::monitor::SyncState::kPaused);
  assert(sync.Resume().has_value());
  assert(sync.State() == nas::monitor::SyncState::kRunning);
  assert(sync.Cancel().has_value());
  assert(sync.State() == nas::monitor::SyncState::kCanceled);

  nas::monitor::HardwareInventory inv;
  auto items = inv.Collect();
  assert(!items.empty());

  nas::monitor::DiagnosticReport report;
  auto html = report.GenerateHtml();
  assert(html.has_value());
  return 0;
}
