add_library(nas-storage STATIC
  src/partition.cpp
  src/zfs_pool.cpp
  src/iscsi_target.cpp
  src/share_config.cpp
)

add_library(nas::storage ALIAS nas-storage)

target_compile_features(nas-storage PUBLIC cxx_std_23)

target_include_directories(nas-storage
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nas-storage PUBLIC nas-common)

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nas-storage-dry-run-test tests/dry_run_test.cpp)
  target_link_libraries(nas-storage-dry-run-test PRIVATE nas-storage)
  add_test(NAME nas-storage-dry-run-test COMMAND nas-storage-dry-run-test)
endif()
