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

  add_executable(nas-storage-extended-test tests/storage_extended_test.cpp)
  target_link_libraries(nas-storage-extended-test PRIVATE nas-storage)
  add_test(NAME nas-storage-extended-test COMMAND nas-storage-extended-test)

  add_executable(nas-storage-partition-test tests/partition_manager_test.cpp)
  target_link_libraries(nas-storage-partition-test PRIVATE nas-storage)
  add_test(NAME nas-storage-partition-test COMMAND nas-storage-partition-test)

  add_executable(nas-storage-zfs-pool-test tests/zfs_pool_test.cpp)
  target_link_libraries(nas-storage-zfs-pool-test PRIVATE nas-storage)
  add_test(NAME nas-storage-zfs-pool-test COMMAND nas-storage-zfs-pool-test)

  add_executable(nas-storage-iscsi-target-test tests/iscsi_target_test.cpp)
  target_link_libraries(nas-storage-iscsi-target-test PRIVATE nas-storage)
  add_test(NAME nas-storage-iscsi-target-test COMMAND nas-storage-iscsi-target-test)

  add_executable(nas-storage-share-config-test tests/share_config_test.cpp)
  target_link_libraries(nas-storage-share-config-test PRIVATE nas-storage)
  add_test(NAME nas-storage-share-config-test COMMAND nas-storage-share-config-test)
endif()
