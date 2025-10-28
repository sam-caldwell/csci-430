#! File: cmake/Testing.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Configure testing with GoogleTest and CTest. Tries Homebrew first,
#!          then falls back to FetchContent to download gtest if needed.

include_guard(GLOBAL)
include(CTest)
enable_testing()

# Try to find an existing GTest (e.g., via Homebrew)
find_package(GTest QUIET)

if(NOT GTest_FOUND)
  include(FetchContent)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

