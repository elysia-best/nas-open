#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "nas::nas-common" for configuration "Debug"
set_property(TARGET nas::nas-common APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(nas::nas-common PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libnas-common.a"
  )

list(APPEND _cmake_import_check_targets nas::nas-common )
list(APPEND _cmake_import_check_files_for_nas::nas-common "${_IMPORT_PREFIX}/lib/libnas-common.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
