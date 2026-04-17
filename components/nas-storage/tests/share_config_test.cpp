#include <cassert>
#include <string>

#include "nas/storage/share_config.hpp"

int main() {
  nas::storage::ShareConfig share;

  // -----------------------------------------------------------------------
  // RenderNfsExports – pure generation (both modes identical)
  // -----------------------------------------------------------------------
  auto nfs_dry = share.RenderNfsExports("/mnt/data", true);
  assert(nfs_dry.has_value());
  assert(nfs_dry.value().find("/mnt/data") != std::string::npos);

  auto nfs_live = share.RenderNfsExports("/mnt/data", false);
  assert(nfs_live.has_value());
  assert(nfs_live.value() == nfs_dry.value());

  // Validation
  assert(!share.RenderNfsExports("", true).has_value());
  assert(share.RenderNfsExports("", true).error().code == nas::ErrorCode::kInvalidArgument);

  // -----------------------------------------------------------------------
  // RenderSambaConfig – pure generation (both modes identical)
  // -----------------------------------------------------------------------
  auto smb_dry = share.RenderSambaConfig("Public", true);
  assert(smb_dry.has_value());
  assert(smb_dry.value().find("Public")     != std::string::npos);
  assert(smb_dry.value().find("browseable") != std::string::npos);

  auto smb_live = share.RenderSambaConfig("Public", false);
  assert(smb_live.has_value());
  assert(smb_live.value() == smb_dry.value());

  // Validation
  assert(!share.RenderSambaConfig("", true).has_value());
  assert(share.RenderSambaConfig("", true).error().code == nas::ErrorCode::kInvalidArgument);

  // -----------------------------------------------------------------------
  // ApplyNfsExports – dry_run
  // -----------------------------------------------------------------------
  auto apply_nfs = share.ApplyNfsExports("/mnt/backup", "", true);
  assert(apply_nfs.has_value());
  assert(apply_nfs.value().find("/mnt/backup") != std::string::npos);
  assert(apply_nfs.value().find("rw")          != std::string::npos);

  // Custom options
  auto apply_nfs_ro = share.ApplyNfsExports("/mnt/readonly", "*(ro,sync)", true);
  assert(apply_nfs_ro.has_value());
  assert(apply_nfs_ro.value().find("ro,sync") != std::string::npos);

  // Validation
  assert(!share.ApplyNfsExports("", "", true).has_value());
  assert(share.ApplyNfsExports("", "", true).error().code == nas::ErrorCode::kInvalidArgument);

  // Live mode fails (cannot write /etc/exports in sandbox)
  assert(!share.ApplyNfsExports("/mnt/data", "", false).has_value());

  // -----------------------------------------------------------------------
  // RemoveNfsExport – dry_run (always succeeds; live mode fails in sandbox)
  // -----------------------------------------------------------------------
  assert(share.RemoveNfsExport("/mnt/data", true).has_value());
  assert(!share.RemoveNfsExport("", true).has_value());
  assert(share.RemoveNfsExport("", true).error().code == nas::ErrorCode::kInvalidArgument);

  // Live mode: /etc/exports not writable in sandbox → fails gracefully
  // (we only check it doesn't crash or abort)
  (void)share.RemoveNfsExport("/mnt/data", false);

  // -----------------------------------------------------------------------
  // ApplySambaShare – dry_run
  // -----------------------------------------------------------------------
  auto apply_smb = share.ApplySambaShare("Backup", "/mnt/backup", false, true);
  assert(apply_smb.has_value());
  assert(apply_smb.value().find("[Backup]")     != std::string::npos);
  assert(apply_smb.value().find("/mnt/backup")  != std::string::npos);
  assert(apply_smb.value().find("browseable")   != std::string::npos);
  assert(apply_smb.value().find("read only = no") != std::string::npos);

  // Read-only share
  auto apply_smb_ro = share.ApplySambaShare("ReadOnly", "/mnt/ro", true, true);
  assert(apply_smb_ro.has_value());
  assert(apply_smb_ro.value().find("read only = yes") != std::string::npos);

  // Default path (empty path → derived from share name)
  auto apply_smb_default = share.ApplySambaShare("Media", "", false, true);
  assert(apply_smb_default.has_value());
  assert(apply_smb_default.value().find("Media") != std::string::npos);

  // Validation
  assert(!share.ApplySambaShare("", "/mnt/data", false, true).has_value());
  assert(share.ApplySambaShare("", "/mnt/data", false, true).error().code ==
         nas::ErrorCode::kInvalidArgument);

  // Live mode fails (cannot write /etc/samba/smb.conf in sandbox)
  assert(!share.ApplySambaShare("Test", "/mnt/test", false, false).has_value());

  // -----------------------------------------------------------------------
  // RemoveSambaShare – dry_run
  // -----------------------------------------------------------------------
  assert(share.RemoveSambaShare("Public", true).has_value());
  assert(!share.RemoveSambaShare("", true).has_value());
  assert(share.RemoveSambaShare("", true).error().code == nas::ErrorCode::kInvalidArgument);

  // Live mode: /etc/samba/smb.conf may not exist → tolerate gracefully
  (void)share.RemoveSambaShare("Public", false);

  return 0;
}
