#pragma once

#include <string>

#include "nas/error.hpp"

namespace nas::storage {

class IscsiTarget {
 public:
  // -----------------------------------------------------------------------
  // Existing API
  // -----------------------------------------------------------------------

  // Map a LUN to an iSCSI target's portal group.
  // dry_run=true  → returns the targetcli command string.
  // dry_run=false → executes the command via targetcli.
  [[nodiscard]] Result<std::string> MapLun(const std::string& iqn,
                                            const std::string& lun,
                                            bool dry_run) const;

  // -----------------------------------------------------------------------
  // Target management
  // -----------------------------------------------------------------------

  // Create a new iSCSI target with the given IQN.
  [[nodiscard]] Result<std::string> CreateTarget(const std::string& iqn,
                                                  bool dry_run) const;

  // Delete an iSCSI target (and all its LUNs).
  [[nodiscard]] Result<void> DeleteTarget(const std::string& iqn,
                                           bool dry_run) const;

  // List all configured iSCSI targets.
  [[nodiscard]] Result<std::string> ListTargets(bool dry_run) const;

  // Unmap (remove) a LUN from a target portal group.
  [[nodiscard]] Result<void> UnmapLun(const std::string& iqn,
                                       const std::string& lun,
                                       bool dry_run) const;
};

}  // namespace nas::storage
