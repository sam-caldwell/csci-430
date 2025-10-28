# CSCI-430 Development Environment

This repository is configured for modern C/C++ development using Homebrew LLVM (clang/clang++), CMake, and Ninja.
It is optimized for CLion but works equally well from the command line.

## Overview

| Tool            | Version      |
|-----------------|--------------|
| CMake           | 3.24.2       |
| Clang / Clang++ | 17.0.2       |
| LLVM            | 17.0.2       |
| Ninja           | 1.13.2       |
| OS              | macOS 12.3.1 |
| CPU             | Apple M1 Max |
| Xcode           | 13.3         |
| C               | 23           |
| C++             | 23           |

- Artifacts: Each target writes to `build/<target>/`:
    - Executable: `build/<target>/<target>`
    - Linked LLVM: `build/<target>/<target>.bc` and `build/<target>/<target>.ll`
    - Per-source: `build/<target>/*.bc` and `*.ll`
- Makefile helpers:
  - `make build`,
  - `make clean`,
  - `make test`,
  - `make version`

## Prerequisites

- macOS with Homebrew
- Install tools:
    - `brew install llvm@17 cmake ninja zip`
- Optional IDE: JetBrains CLion

> ***Tip:*** Ensure Homebrew LLVM 17 is preferred in PATH when running outside CMake toolchain files:
> CMake does this for you automatically.

export PATH="$(brew --prefix llvm@17)/bin:$PATH"

## Project Layout

- `CMakeLists.txt`: Lightweight entry; includes modules and project list.
- `cmake/` (modular CMake):
    - `LLVMConfig.cmake`: Finds and prefers Homebrew LLVM 17 tools (`clang`, `clang++`, `llvm-link`, `llvm-dis`), and
      enforces LLVM 17.x.
    - `BuildConfig.cmake`: Language standards, compile commands, common warnings.
    - `ProjectHelpers.cmake`: Provides `build_project(target sources...)` wrapper that emits all artifacts into
      `build/<target>/`.
    - `projects.cmake`: Central registry of targets using `build_project(...)`.
    - `Toolchain-HomebrewLLVM.cmake`: Optional toolchain file to force Homebrew LLVM + Ninja.
- `src/`: Course “hello_world” C target.
- `include/`: Public headers (e.g., `hello_world.h`).
- `test/`: GoogleTest unit/integration tests.
- `assignments/`: Assignment targets.
- `discussions/`: Notes, design docs, and example code.
- `build/`: Artifact directory created by builds (git-ignored).

## First Target (C)

- Target: `hello_world`
- Source: `src/main.c`
- Behavior: prints "Hello, World" (with quotes) and exits.

Run after a build:

./build/hello_world/hello_world

## Makefile Targets

- `make build`: Configures (first run) and builds all targets with CMake/Ninja.
- `make clean`: Deletes `build/` and recreates it.
- `make version`: Shows versions for CMake, clang/clang++, llvm (via `llvm-config` and brew), Ninja, OS, CPU, and Xcode.
- `make test`: Runs unit tests via CTest/GoogleTest.

You can override variables:

make build BUILD_DIR=cmake-build-release CONFIG=Release

Defaults:

- `BUILD_DIR=cmake-build-debug`
- `GENERATOR=Ninja`
- `TOOLCHAIN=cmake/Toolchain-HomebrewLLVM.cmake`

## CLI Build (manual)

Run `make build`

-or-

Run CMake directly:
- `cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-HomebrewLLVM.cmake`
- `cmake --build cmake-build-debug`

Artifacts will appear under `build/<target>/` per target.

## CLion Setup

1) Open the repository in CLion.
2) In CMake settings:
    - Generator: `Ninja`.
    - CMake toolchain file (recommended): `cmake/Toolchain-HomebrewLLVM.cmake`.
3) Build the project. Artifacts for each target appear under `build/<target>/`.
4) Create Run/Debug configurations for executables in `build/<target>/` (or rely on CLion default target selection).

## Adding a New Target

Register a target in `cmake/projects.cmake` using the helper:

build_project(myprog assignments/myprog/main.cpp)

This will create:

- Executable: `build/myprog/myprog`
- Linked LLVM: `build/myprog/myprog.bc` and `build/myprog/myprog.ll`
- Per-source LLVM: `build/myprog/*.bc` and `build/myprog/*.ll`

If you need include paths or definitions, set them on the target after the `build_project(...)` call, e.g.:

target_include_directories(myprog PRIVATE assignments/myprog/include)
target_compile_definitions(myprog PRIVATE FOO=1)

## Tests

- Place tests under `test/` and register a test target in `cmake/projects.cmake`, for example:

  build_project(mytests test/my_test.cpp src/some_impl.c)
  target_include_directories(mytests PRIVATE ${PROJECT_SOURCE_DIR}/include)
  target_link_libraries(mytests PRIVATE GTest::gtest_main)
  gtest_discover_tests(mytests)

Run tests with: `make test`

## Verifying the Environment

Run: `make version`
> This prints CMake, Clang/LLVM, Ninja, OS, CPU, and Xcode info. Confirm that clang/clang++ show “Homebrew” and 
> versions match `brew list --versions llvm`.

## LLVM 17 Requirement

- This project requires LLVM 17.x (Clang 17). CMake will fail configuration if a different major version is detected.
- Install and prefer Homebrew LLVM 17:

      brew install llvm@17
      export PATH="$(brew --prefix llvm@17)/bin:$PATH"

- If you have multiple versions installed, use the provided toolchain file to force LLVM 17:

      -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-HomebrewLLVM.cmake

## Troubleshooting

- Wrong compiler picked (Apple clang): Use the toolchain file or prepend Homebrew LLVM 17 to PATH.
- Stale cache after tool changes: delete `cmake-build-*` and reconfigure.
- Missing `llvm-link` or `llvm-dis`: ensure `brew install llvm@17` and that `$(brew --prefix llvm@17)/bin` is on PATH,
  or use the toolchain file.
