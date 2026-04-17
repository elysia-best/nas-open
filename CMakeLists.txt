cmake_minimum_required(VERSION 3.21)
project(nas-open VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

option(NAS_BUILD_TESTS "Build nas-open tests" ON)

add_subdirectory(components/nas-common)
add_subdirectory(components/nasd-core)
add_subdirectory(components/nas-gateway)
add_subdirectory(components/nas-cli)
add_subdirectory(components/nas-storage)
add_subdirectory(components/nas-monitor)
