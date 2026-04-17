#include <cassert>
#include <filesystem>
#include <string>

#include "nas/osal/posix_fs.hpp"

int main() {
  nas::osal::PosixFileSystem fs;
  namespace stdfs = std::filesystem;

  // Create temp directory and file
  auto tmp = stdfs::temp_directory_path() / "nas_osal_fs_test";
  stdfs::create_directories(tmp);
  auto tmp_file = (tmp / "hello.txt").string();

  // Write
  auto write_res = fs.WriteAll(tmp_file, "hello world");
  assert(write_res.has_value());

  // Read back
  auto read_res = fs.ReadAll(tmp_file);
  assert(read_res.has_value());
  assert(read_res.value() == "hello world");

  // List directory
  auto list_res = fs.List(tmp.string());
  assert(list_res.has_value());
  assert(!list_res.value().empty());

  // Non-existent read
  auto bad_read = fs.ReadAll("/definitely/not/here.txt");
  assert(!bad_read.has_value());
  assert(bad_read.error().code == nas::ErrorCode::kNotFound);

  // Cleanup
  stdfs::remove_all(tmp);
  return 0;
}
