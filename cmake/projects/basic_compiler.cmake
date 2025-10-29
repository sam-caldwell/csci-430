# File: cmake/projects/basic_compiler.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Define the GW-BASIC compiler library and CLI executable.

# Auto-discover core sources (all but main.cpp)
file(GLOB_RECURSE BASIC_COMPILER_CORE_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/basic_compiler/lexer/*.cpp
  ${PROJECT_SOURCE_DIR}/src/basic_compiler/parser/*.cpp
  ${PROJECT_SOURCE_DIR}/src/basic_compiler/codegenerator/*.cpp
  ${PROJECT_SOURCE_DIR}/src/basic_compiler/optimizer/*.cpp
  ${PROJECT_SOURCE_DIR}/src/basic_compiler/compiler/*.cpp
)

add_library(basic_compiler_lib STATIC ${BASIC_COMPILER_CORE_SOURCES})
target_include_directories(basic_compiler_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Ensure hello_world builds first as a bootstrap sanity check
add_dependencies(basic_compiler_lib hello_world)

# Build the CLI as a project with IR/BC artifacts for all sources (auto-discovered plus main)
build_project(basic_compiler ${BASIC_COMPILER_CORE_SOURCES} ${PROJECT_SOURCE_DIR}/src/basic_compiler/main.cpp)
target_include_directories(basic_compiler PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Enforce hello_world to build before the compiler and its IR/BC artifacts
add_dependencies(basic_compiler hello_world)
add_dependencies(basic_compiler_bc hello_world)
add_dependencies(basic_compiler_ir hello_world)

if (CLANG_EXECUTABLE)
  target_compile_definitions(basic_compiler PRIVATE CLANG_PATH="${CLANG_EXECUTABLE}")
else()
  target_compile_definitions(basic_compiler PRIVATE CLANG_PATH="clang")
endif()
