add_library(nas-cli-lib STATIC
  src/cmd_router.cpp
  src/app.cpp
)

add_library(nas::cli ALIAS nas-cli-lib)

target_compile_features(nas-cli-lib PUBLIC cxx_std_23)

target_include_directories(nas-cli-lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nas-cli-lib PUBLIC nas-common)

add_executable(nas-cli src/main.cpp)
target_link_libraries(nas-cli PRIVATE nas-cli-lib)

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nas-cli-cmd-router-test tests/cmd_router_test.cpp)
  target_link_libraries(nas-cli-cmd-router-test PRIVATE nas-cli-lib)
  add_test(NAME nas-cli-cmd-router-test COMMAND nas-cli-cmd-router-test)
endif()
