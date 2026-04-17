#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class ShareConfig {
 public:
  // -----------------------------------------------------------------------
  // Config rendering (pure text generation, both modes return config text)
  // -----------------------------------------------------------------------

  // Generate an NFS exports line for path with default options.
  // Both dry_run=true and dry_run=false return the rendered text.
  [[nodiscard]] Result<std::string> RenderNfsExports(const std::string& path,
                                                     bool dry_run) const;

  // Generate a Samba share config block.
  // Both dry_run=true and dry_run=false return the rendered text.
  [[nodiscard]] Result<std::string> RenderSambaConfig(const std::string& share_name,
                                                      bool dry_run) const;

  // -----------------------------------------------------------------------
  // Apply / Remove NFS exports  (write file + reload exportfs)
  // -----------------------------------------------------------------------

  // Append a new NFS export entry to /etc/exports and reload with exportfs.
  // options: export options (may be empty → "*(rw,sync,no_subtree_check)").
  // dry_run=true  → returns the line that would be added.
  // dry_run=false → writes /etc/exports, runs `exportfs -ra`.
  [[nodiscard]] Result<std::string> ApplyNfsExports(const std::string& path,
                                                    const std::string& options,
                                                    bool dry_run) const;

  // Remove an NFS export for path from /etc/exports and reload exportfs.
  // dry_run=true  → returns the line that would be removed.
  // dry_run=false → edits /etc/exports, runs `exportfs -ra`.
  [[nodiscard]] Result<void> RemoveNfsExport(const std::string& path,
                                              bool dry_run) const;

  // -----------------------------------------------------------------------
  // Apply / Remove Samba shares  (write file + reload smbd)
  // -----------------------------------------------------------------------

  // Append a new share block to /etc/samba/smb.conf and reload Samba.
  // path:      directory to share (may be empty to use share_name as default).
  // read_only: whether the share is read-only.
  // dry_run=true  → returns the config block that would be added.
  // dry_run=false → writes /etc/samba/smb.conf, runs `smbcontrol all reload-config`.
  [[nodiscard]] Result<std::string> ApplySambaShare(const std::string& share_name,
                                                    const std::string& path,
                                                    bool read_only,
                                                    bool dry_run) const;

  // Remove a Samba share block from /etc/samba/smb.conf and reload Samba.
  // dry_run=true  → returns the command that would be run.
  // dry_run=false → edits /etc/samba/smb.conf, runs `smbcontrol all reload-config`.
  [[nodiscard]] Result<void> RemoveSambaShare(const std::string& share_name,
                                               bool dry_run) const;
};

}  // namespace nas::storage
