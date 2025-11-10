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

# Common warnings for Clang (treat warnings as errors)
if (CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    "$<$<COMPILE_LANGUAGE:C>:-Wall;-Wextra;-Wpedantic;-Werror>"
  )
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-Werror>"
  )
endif()

# On Linux with Clang, prefer libc++ to avoid libstdc++ ABI/header conflicts.
if (UNIX AND NOT APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Apply libc++ only to C++ compilation to avoid warnings on C files.
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>")
  add_link_options(-stdlib=libc++)
endif()

# On macOS with Homebrew LLVM, prefer linking against Homebrew's libc++ runtime.
# Avoid regex in if() (CMake does not short-circuit evaluation reliably).
if(APPLE)
  set(_hb_llvm FALSE)
  string(FIND "${CMAKE_CXX_COMPILER}" "/opt/homebrew/opt/llvm/bin/clang++" _idx1)
  string(FIND "${CMAKE_CXX_COMPILER}" "/usr/local/opt/llvm/bin/clang++" _idx2)
  if(NOT _idx1 EQUAL -1 OR NOT _idx2 EQUAL -1)
    set(_hb_llvm TRUE)
  endif()
  if(_hb_llvm)
    get_filename_component(_clang_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(_llvm_root "${_clang_bin_dir}" DIRECTORY)
    set(_llvm_lib "${_llvm_root}/lib")
    set(_llvm_lib_cxx "${_llvm_root}/lib/c++")
    # Add library search path and runtime rpath for Homebrew LLVM libs
    if(EXISTS "${_llvm_lib}")
      add_link_options("-L${_llvm_lib}")
      add_link_options("-Wl,-rpath,${_llvm_lib}")
      # Explicitly prefer libc++ and c++abi from the same toolchain if present
      if(EXISTS "${_llvm_lib}/libc++.dylib")
        add_link_options("${_llvm_lib}/libc++.dylib")
      endif()
      if(EXISTS "${_llvm_lib}/libc++abi.dylib")
        add_link_options("${_llvm_lib}/libc++abi.dylib")
      endif()
    endif()
    if(EXISTS "${_llvm_lib_cxx}")
      add_link_options("-L${_llvm_lib_cxx}")
      add_link_options("-Wl,-rpath,${_llvm_lib_cxx}")
    endif()
  endif()
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
