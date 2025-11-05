#! File: cmake/toolchain.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Unified toolchain to prefer LLVM/Clang across macOS (Homebrew) and Linux CI.

# macOS (Homebrew): Prefer Homebrew LLVM, if present.
if(APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER OR NOT DEFINED CMAKE_CXX_COMPILER)
    # Prefer llvm@17 when available
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
endif()

# Linux CI: Prefer Clang 17 and link against libc++ when available.
if(UNIX AND NOT APPLE)
  # Prefer the versioned LLVM 17 bin dir first on PATH so find_program() picks it up.
  if(EXISTS "/usr/lib/llvm-17/bin")
    set(ENV{PATH} "/usr/lib/llvm-17/bin:$ENV{PATH}")
  endif()

  if(NOT DEFINED CMAKE_C_COMPILER OR NOT DEFINED CMAKE_CXX_COMPILER)
    if(EXISTS "/usr/bin/clang-17" AND EXISTS "/usr/bin/clang++-17")
      set(CMAKE_C_COMPILER  "/usr/bin/clang-17"   CACHE FILEPATH "C compiler" FORCE)
      set(CMAKE_CXX_COMPILER "/usr/bin/clang++-17" CACHE FILEPATH "C++ compiler" FORCE)
    endif()
  endif()

  # Ensure all C++ code builds and links against libc++ when using LLVM 17 layout.
  set(_LLVM17_LIB "/usr/lib/llvm-17/lib")
  if(EXISTS "${_LLVM17_LIB}")
    set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-stdlib=libc++ -L${_LLVM17_LIB} -Wl,-rpath,${_LLVM17_LIB}")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "-stdlib=libc++ -L${_LLVM17_LIB} -Wl,-rpath,${_LLVM17_LIB}")

    # Helpful cache vars for downstream tools
    if(EXISTS "/usr/lib/llvm-17/bin/llvm-link")
      set(LLVM_LINK_EXECUTABLE "/usr/lib/llvm-17/bin/llvm-link" CACHE FILEPATH "llvm-link executable" FORCE)
    endif()
    if(EXISTS "/usr/lib/llvm-17/bin/llvm-dis")
      set(LLVM_DIS_EXECUTABLE  "/usr/lib/llvm-17/bin/llvm-dis"  CACHE FILEPATH "llvm-dis executable"  FORCE)
    endif()
    set(LLVM_PREFIX "/usr/lib/llvm-17" CACHE PATH "LLVM 17 prefix" FORCE)
  endif()

  # Help CMake try-compile steps avoid linking an executable when probing.
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

# Do not override CMAKE_MAKE_PROGRAM here; the generator determines the build tool.
