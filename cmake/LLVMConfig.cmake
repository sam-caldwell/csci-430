#[=======================================================================[.rst:
# LLVMConfig.cmake - Helper to locate Homebrew LLVM/Clang and tools
#
# Exposes the following cache variables for other CMake modules:
#  - CLANG_EXECUTABLE      : path to clang
#  - CLANGXX_EXECUTABLE    : path to clang++
#  - LLVM_LINK_EXECUTABLE  : path to llvm-link
#  - LLVM_DIS_EXECUTABLE   : path to llvm-dis
#  - LLVM_CONFIG_EXECUTABLE: path to llvm-config (if available)
#
# This script prefers Homebrew's llvm@17 keg and enforces LLVM 17.x by default.
#]=======================================================================]

include_guard(GLOBAL)

option(REQUIRE_LLVM_17 "Require LLVM major version 17.x" ON)

# Build a list of candidate llvm bin paths: prefer llvm@17 first, then generic llvm
set(_llvm_bin_hints)

# Try to discover Homebrew's llvm@17 prefix
set(_llvm_brew17_prefix "")
execute_process(
  COMMAND /bin/bash -lc "brew --prefix llvm@17"
  OUTPUT_VARIABLE _llvm_brew17_prefix
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
if(_llvm_brew17_prefix)
  list(APPEND _llvm_bin_hints "${_llvm_brew17_prefix}/bin")
endif()
# Common Homebrew install locations for llvm@17
list(APPEND _llvm_bin_hints
  "/opt/homebrew/opt/llvm@17/bin"   # Apple Silicon
  "/usr/local/opt/llvm@17/bin"      # Intel macs
)

# Fallback to generic llvm keg
set(_llvm_brew_prefix "")
execute_process(
  COMMAND /bin/bash -lc "brew --prefix llvm"
  OUTPUT_VARIABLE _llvm_brew_prefix
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
if(_llvm_brew_prefix)
  list(APPEND _llvm_bin_hints "${_llvm_brew_prefix}/bin")
endif()
list(APPEND _llvm_bin_hints
  "/opt/homebrew/opt/llvm/bin"
  "/usr/local/opt/llvm/bin"
)

# Discover tools (honor existing cache values)
find_program(CLANG_EXECUTABLE NAMES clang HINTS ${_llvm_bin_hints} NO_CACHE)
find_program(CLANGXX_EXECUTABLE NAMES clang++ HINTS ${_llvm_bin_hints} NO_CACHE)
find_program(LLVM_LINK_EXECUTABLE NAMES llvm-link HINTS ${_llvm_bin_hints} NO_CACHE)
find_program(LLVM_DIS_EXECUTABLE NAMES llvm-dis HINTS ${_llvm_bin_hints} NO_CACHE)
find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config HINTS ${_llvm_bin_hints} NO_CACHE)

# Prefer discovered Homebrew clang for this project if possible
if(CLANG_EXECUTABLE AND CLANGXX_EXECUTABLE)
  if (NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}" CACHE FILEPATH "C compiler" FORCE)
  endif()
  if (NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "${CLANGXX_EXECUTABLE}" CACHE FILEPATH "C++ compiler" FORCE)
  endif()
else()
  message(WARNING "Homebrew clang/clang++ not found. Using default CMake compilers: ${CMAKE_C_COMPILER} / ${CMAKE_CXX_COMPILER}.")
endif()

if(LLVM_LINK_EXECUTABLE)
  set(LLVM_LINK_EXECUTABLE "${LLVM_LINK_EXECUTABLE}" CACHE FILEPATH "llvm-link executable" FORCE)
endif()
if(LLVM_DIS_EXECUTABLE)
  set(LLVM_DIS_EXECUTABLE "${LLVM_DIS_EXECUTABLE}" CACHE FILEPATH "llvm-dis executable" FORCE)
endif()
if(LLVM_CONFIG_EXECUTABLE)
  set(LLVM_CONFIG_EXECUTABLE "${LLVM_CONFIG_EXECUTABLE}" CACHE FILEPATH "llvm-config executable" FORCE)
endif()

# Verify LLVM version (require 17.x by default)
set(_llvm_version_text "")
set(_llvm_version_major "")
if(LLVM_CONFIG_EXECUTABLE)
  execute_process(
    COMMAND "${LLVM_CONFIG_EXECUTABLE}" --version
    OUTPUT_VARIABLE _llvm_version_text
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
elseif(CLANGXX_EXECUTABLE)
  execute_process(
    COMMAND "${CLANGXX_EXECUTABLE}" --version
    OUTPUT_VARIABLE _llvm_version_text
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
endif()

if(_llvm_version_text MATCHES "^([0-9]+)\\.")
  set(_llvm_version_major "${CMAKE_MATCH_1}")
elseif(_llvm_version_text MATCHES "version[ ]*([0-9]+)")
  set(_llvm_version_major "${CMAKE_MATCH_1}")
endif()

if(REQUIRE_LLVM_17)
  if(NOT _llvm_version_major)
    message(WARNING "Unable to detect LLVM version. Expected LLVM 17.x. Ensure Homebrew llvm@17 is installed and on PATH.")
  elseif(NOT _llvm_version_major EQUAL 17)
    message(FATAL_ERROR "LLVM 17.x is required. Detected '${_llvm_version_text}'. Install with 'brew install llvm@17' and ensure $(brew --prefix llvm@17)/bin is first in PATH, or use -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-HomebrewLLVM.cmake")
  endif()
endif()

mark_as_advanced(CLANG_EXECUTABLE CLANGXX_EXECUTABLE LLVM_LINK_EXECUTABLE LLVM_DIS_EXECUTABLE LLVM_CONFIG_EXECUTABLE)
#! File: cmake/LLVMConfig.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Locate Homebrew LLVM/Clang toolchain (preferring llvm@17) and enforce version.
