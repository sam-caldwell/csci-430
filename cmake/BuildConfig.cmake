#! File: cmake/BuildConfig.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Define project-wide build settings (standards, compile_commands).
#!          Configure common warnings for Clang toolchains.
include_guard(GLOBAL)

# Language standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helpful defaults for IDEs and tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common warnings for Clang
if (CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options("$<$<COMPILE_LANGUAGE:C>:-Wall;-Wextra;-Wpedantic>")
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>")
endif()

# Enforce out-of-source build directory under build/
# Disallow top-level cmake-build-* directories (e.g., cmake-build-debug)
get_filename_component(_csci430_bin_name "${CMAKE_BINARY_DIR}" NAME)
get_filename_component(_csci430_bin_parent "${CMAKE_BINARY_DIR}" DIRECTORY)

# Optional: Code coverage instrumentation for Clang toolchains
option(CODE_COVERAGE "Enable Clang source-based coverage (llvm-cov)" OFF)
if (CODE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling Clang source-based coverage (-fprofile-instr-generate -fcoverage-mapping)")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
  else()
    message(WARNING "CODE_COVERAGE requested but compiler is not Clang; ignoring")
  endif()
endif()
