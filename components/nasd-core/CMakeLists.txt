add_library(nasd-core STATIC
  src/plugin_loader.cpp
  src/server.cpp
)

add_library(nas::daemon_core ALIAS nasd-core)

target_compile_features(nasd-core PUBLIC cxx_std_23)

target_include_directories(nasd-core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nasd-core PUBLIC nas-common)
target_link_libraries(nasd-core PRIVATE ${CMAKE_DL_LIBS})

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nasd-core-plugin-loader-test tests/plugin_loader_test.cpp)
  target_link_libraries(nasd-core-plugin-loader-test PRIVATE nasd-core)
  add_test(NAME nasd-core-plugin-loader-test COMMAND nasd-core-plugin-loader-test)
endif()
