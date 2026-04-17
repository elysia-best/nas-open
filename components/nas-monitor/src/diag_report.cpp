#include "nas/monitor/diag_report.hpp"

namespace nas::monitor {

Result<std::string> DiagnosticReport::GenerateHtml() const {
  return "<html><body><h1>NAS Diagnostic Report</h1></body></html>";
}

}  // namespace nas::monitor
