#! File: cmake/Toolchain-HomebrewLLVM.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Optional toolchain to force Homebrew LLVM compilers and tools.
#!          Also prefers Ninja when available for faster builds.
# Optional toolchain to force Homebrew LLVM

# Attempt to locate Homebrew's LLVM 17 keg and set compilers accordingly.
if(NOT DEFINED CMAKE_C_COMPILER OR NOT DEFINED CMAKE_CXX_COMPILER)
  # Prefer llvm@17
  execute_process(
    COMMAND /bin/bash -lc "brew --prefix llvm@17"
    OUTPUT_VARIABLE _brew_llvm_prefix
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(NOT _brew_llvm_prefix)
    if(EXISTS "/opt/homebrew/opt/llvm@17/bin/clang")
      set(_brew_llvm_prefix "/opt/homebrew/opt/llvm@17")
    elseif(EXISTS "/usr/local/opt/llvm@17/bin/clang")
      set(_brew_llvm_prefix "/usr/local/opt/llvm@17")
    endif()
  endif()

  # Fallback to generic llvm if @17 not present
  if(NOT _brew_llvm_prefix)
    execute_process(
      COMMAND /bin/bash -lc "brew --prefix llvm"
      OUTPUT_VARIABLE _brew_llvm_prefix
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(NOT _brew_llvm_prefix)
      if(EXISTS "/opt/homebrew/opt/llvm/bin/clang")
        set(_brew_llvm_prefix "/opt/homebrew/opt/llvm")
      elseif(EXISTS "/usr/local/opt/llvm/bin/clang")
        set(_brew_llvm_prefix "/usr/local/opt/llvm")
      endif()
    endif()
  endif()

  if(_brew_llvm_prefix)
    set(CMAKE_C_COMPILER  "${_brew_llvm_prefix}/bin/clang" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${_brew_llvm_prefix}/bin/clang++" CACHE FILEPATH "C++ compiler" FORCE)
    # Make LLVM tools easily discoverable
    set(ENV{PATH} "${_brew_llvm_prefix}/bin:$ENV{PATH}")
  else()
    message(WARNING "Homebrew LLVM not found in toolchain file. Using default compilers.")
  endif()
endif()

# Prefer Ninja if available
find_program(_ninja_exe NAMES ninja)
if(_ninja_exe)
  set(CMAKE_MAKE_PROGRAM "${_ninja_exe}" CACHE FILEPATH "Build tool" FORCE)
endif()
