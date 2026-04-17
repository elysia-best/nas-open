add_library(nas-monitor STATIC
  src/smart_poller.cpp
  src/cloud_sync.cpp
  src/hw_inventory.cpp
  src/diag_report.cpp
)

add_library(nas::monitor ALIAS nas-monitor)

target_compile_features(nas-monitor PUBLIC cxx_std_23)

target_include_directories(nas-monitor
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nas-monitor PUBLIC nas-common)

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nas-monitor-components-test tests/components_test.cpp)
  target_link_libraries(nas-monitor-components-test PRIVATE nas-monitor)
  add_test(NAME nas-monitor-components-test COMMAND nas-monitor-components-test)
endif()
