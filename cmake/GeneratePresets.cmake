#! File: cmake/GeneratePresets.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Generate CMakePresets.json in the source tree to standardize
#!          out-of-source build directories for IDEs and CLI.

include_guard(GLOBAL)

set(_gen "${CMAKE_GENERATOR}")
if(NOT _gen)
  set(_gen "Ninja")
endif()

set(_tool "${CMAKE_TOOLCHAIN_FILE}")
if(NOT _tool)
  set(_tool "cmake/Toolchain-HomebrewLLVM.cmake")
endif()

set(GENERATOR "${_gen}")
set(TOOLCHAIN "${_tool}")

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/CMakePresets.json.in"
  "${CMAKE_SOURCE_DIR}/CMakePresets.json"
  @ONLY
)

message(STATUS "Wrote CMakePresets.json with generator='${GENERATOR}' and toolchain='${TOOLCHAIN}'")

