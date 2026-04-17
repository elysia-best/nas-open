add_library(nas-gateway STATIC
  src/rest_bridge.cpp
  src/auth_middleware.cpp
  src/ws_pusher.cpp
  src/rate_limiter.cpp
)

add_library(nas::gateway ALIAS nas-gateway)

target_compile_features(nas-gateway PUBLIC cxx_std_23)

target_include_directories(nas-gateway
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nas-gateway PUBLIC nas-common)

if(BUILD_TESTING AND NAS_BUILD_TESTS)
  add_executable(nas-gateway-rate-limiter-test tests/rate_limiter_test.cpp)
  target_link_libraries(nas-gateway-rate-limiter-test PRIVATE nas-gateway)
  add_test(NAME nas-gateway-rate-limiter-test COMMAND nas-gateway-rate-limiter-test)

  add_executable(nas-gateway-auth-test tests/auth_middleware_test.cpp)
  target_link_libraries(nas-gateway-auth-test PRIVATE nas-gateway)
  add_test(NAME nas-gateway-auth-test COMMAND nas-gateway-auth-test)

  add_executable(nas-gateway-rest-bridge-test tests/rest_bridge_test.cpp)
  target_link_libraries(nas-gateway-rest-bridge-test PRIVATE nas-gateway)
  add_test(NAME nas-gateway-rest-bridge-test COMMAND nas-gateway-rest-bridge-test)

  add_executable(nas-gateway-ws-pusher-test tests/ws_pusher_test.cpp)
  target_link_libraries(nas-gateway-ws-pusher-test PRIVATE nas-gateway)
  add_test(NAME nas-gateway-ws-pusher-test COMMAND nas-gateway-ws-pusher-test)
endif()
