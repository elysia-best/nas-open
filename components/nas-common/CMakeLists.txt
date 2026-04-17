add_library(nas-common STATIC
  src/nas_common.cpp
  src/osal/posix_fs.cpp
  src/osal/linux_block.cpp
  src/osal/linux_net.cpp
  src/core/local_event_bus.cpp
  src/core/stderr_logger.cpp
  src/core/inmemory_config_store.cpp
)

add_library(nas::common ALIAS nas-common)

target_compile_features(nas-common PUBLIC cxx_std_23)

target_include_directories(nas-common
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS nas-common
  EXPORT nas-common-targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT nas-common-targets
  FILE nas-common-targets.cmake
  NAMESPACE nas::
  DESTINATION lib/cmake/nas-common
)

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nas-common-error-test
    tests/error_test.cpp
  )
  target_link_libraries(nas-common-error-test PRIVATE nas-common)
  add_test(NAME nas-common-error-test COMMAND nas-common-error-test)

  add_executable(nas-common-osal-fs-test tests/osal_posix_fs_test.cpp)
  target_link_libraries(nas-common-osal-fs-test PRIVATE nas-common)
  add_test(NAME nas-common-osal-fs-test COMMAND nas-common-osal-fs-test)

  add_executable(nas-common-osal-block-test tests/osal_linux_block_test.cpp)
  target_link_libraries(nas-common-osal-block-test PRIVATE nas-common)
  add_test(NAME nas-common-osal-block-test COMMAND nas-common-osal-block-test)

  add_executable(nas-common-osal-net-test tests/osal_linux_net_test.cpp)
  target_link_libraries(nas-common-osal-net-test PRIVATE nas-common)
  add_test(NAME nas-common-osal-net-test COMMAND nas-common-osal-net-test)

  add_executable(nas-common-event-bus-test tests/local_event_bus_test.cpp)
  target_link_libraries(nas-common-event-bus-test PRIVATE nas-common)
  add_test(NAME nas-common-event-bus-test COMMAND nas-common-event-bus-test)

  add_executable(nas-common-stderr-logger-test tests/stderr_logger_test.cpp)
  target_link_libraries(nas-common-stderr-logger-test PRIVATE nas-common)
  add_test(NAME nas-common-stderr-logger-test COMMAND nas-common-stderr-logger-test)

  add_executable(nas-common-config-store-test tests/inmemory_config_store_test.cpp)
  target_link_libraries(nas-common-config-store-test PRIVATE nas-common)
  add_test(NAME nas-common-config-store-test COMMAND nas-common-config-store-test)
endif()
