#include "nas/osal/posix_fs.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace nas::osal {

Result<std::vector<FileEntry>> PosixFileSystem::List(const std::string& root) {
  namespace fs = std::filesystem;
  std::error_code ec;
  if (!fs::exists(root, ec) || ec) {
    return Fail(ErrorCode::kNotFound, "directory does not exist: " + root);
  }
  fs::directory_iterator it(root, ec);
  if (ec) {
    return Fail(ErrorCode::kNotFound, "cannot list directory: " + ec.message());
  }
  std::vector<FileEntry> entries;
  for (const auto& de : it) {
    std::error_code ec2;
    FileEntry fe;
    fe.path = de.path().string();
    fe.is_directory = de.is_directory(ec2);
    fe.size_bytes = de.is_regular_file(ec2) ? de.file_size(ec2) : 0;
    entries.push_back(std::move(fe));
  }
  return entries;
}

Result<std::string> PosixFileSystem::ReadAll(const std::string& path) {
  std::ifstream ifs(path, std::ios::binary);
  if (!ifs) {
    return Fail(ErrorCode::kNotFound, "cannot open file: " + path);
  }
  std::ostringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}

Result<void> PosixFileSystem::WriteAll(const std::string& path, const std::string& data) {
  std::ofstream ofs(path, std::ios::trunc | std::ios::binary);
  if (!ofs) {
    return Fail(ErrorCode::kPermissionDenied, "cannot write file: " + path);
  }
  ofs << data;
  return Ok();
}

}  // namespace nas::osal
