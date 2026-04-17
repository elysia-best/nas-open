add_library(nas-common STATIC
  src/nas_common.cpp
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
endif()
