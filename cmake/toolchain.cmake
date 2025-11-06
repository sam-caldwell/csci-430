#! File: cmake/toolchain.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Unified toolchain to prefer LLVM/Clang across macOS (Homebrew) and Linux CI.

# macOS (Homebrew): Prefer Homebrew LLVM, if present.
if(APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER OR NOT DEFINED CMAKE_CXX_COMPILER)
    # Require llvm@17 via Homebrew
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
    if(NOT _brew_llvm_prefix)
      message(FATAL_ERROR "llvm@17 not found (Homebrew). Please install: brew install llvm@17")
    endif()
    set(CMAKE_C_COMPILER  "${_brew_llvm_prefix}/bin/clang" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${_brew_llvm_prefix}/bin/clang++" CACHE FILEPATH "C++ compiler" FORCE)
    set(ENV{PATH} "${_brew_llvm_prefix}/bin:$ENV{PATH}")
  endif()
endif()

# Linux CI: Prefer versioned LLVM toolchains and link against libc++ when available.
if(UNIX AND NOT APPLE)
  # Pin to LLVM 17 only
  set(_LLVM_CANDIDATES 17)
  set(_FOUND_LLVM_BIN "")
  foreach(v ${_LLVM_CANDIDATES})
    if(EXISTS "/usr/lib/llvm-${v}/bin")
      set(_FOUND_LLVM_BIN "/usr/lib/llvm-${v}/bin")
      break()
    endif()
  endforeach()
  if(_FOUND_LLVM_BIN)
    set(ENV{PATH} "${_FOUND_LLVM_BIN}:$ENV{PATH}")
  endif()

  # Prefer versioned clang/clang++ if present
  if(NOT DEFINED CMAKE_C_COMPILER OR NOT DEFINED CMAKE_CXX_COMPILER)
    foreach(v ${_LLVM_CANDIDATES})
      if(EXISTS "/usr/bin/clang-${v}" AND EXISTS "/usr/bin/clang++-${v}")
        set(CMAKE_C_COMPILER  "/usr/bin/clang-${v}"   CACHE FILEPATH "C compiler" FORCE)
        set(CMAKE_CXX_COMPILER "/usr/bin/clang++-${v}" CACHE FILEPATH "C++ compiler" FORCE)
        break()
      endif()
    endforeach()
  endif()

  # Always request libc++; add rpath to detected LLVM libdir when present
  if(NOT CMAKE_CXX_FLAGS_INIT)
    set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
  else()
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -stdlib=libc++")
  endif()
  foreach(v ${_LLVM_CANDIDATES})
    if(EXISTS "/usr/lib/llvm-${v}/lib")
      set(_LLVMLIB "/usr/lib/llvm-${v}/lib")
      set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -stdlib=libc++ -L${_LLVMLIB} -Wl,-rpath,${_LLVMLIB}")
      set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_SHARED_LINKER_FLAGS_INIT} -stdlib=libc++ -L${_LLVMLIB} -Wl,-rpath,${_LLVMLIB}")
      # Helpful cache vars for downstream tools
      if(EXISTS "/usr/lib/llvm-${v}/bin/llvm-link")
        set(LLVM_LINK_EXECUTABLE "/usr/lib/llvm-${v}/bin/llvm-link" CACHE FILEPATH "llvm-link executable" FORCE)
      endif()
      if(EXISTS "/usr/lib/llvm-${v}/bin/llvm-dis")
        set(LLVM_DIS_EXECUTABLE  "/usr/lib/llvm-${v}/bin/llvm-dis"  CACHE FILEPATH "llvm-dis executable"  FORCE)
      endif()
      set(LLVM_PREFIX "/usr/lib/llvm-${v}" CACHE PATH "LLVM prefix" FORCE)
      break()
    endif()
  endforeach()

  # Help CMake try-compile steps avoid linking an executable when probing.
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

# Do not override CMAKE_MAKE_PROGRAM here; the generator determines the build tool.
