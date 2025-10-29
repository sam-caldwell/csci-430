# File: cmake/projects/hello_world.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Define the C hello_world targets and artifacts layout.

# Primary C program (hello_world) and unit-testable core library

# Build the CLI as a project and emit IR/BC artifacts
build_project(hello_world ${PROJECT_SOURCE_DIR}/src/hello_world/app.c ${PROJECT_SOURCE_DIR}/src/hello_world/main.c)
target_include_directories(hello_world PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Unit-testable core library built from the same implementation
add_library(hello_core STATIC ${PROJECT_SOURCE_DIR}/src/hello_world/main.c)
target_include_directories(hello_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
set_target_properties(hello_core PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/hello_world"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/hello_world"
  OUTPUT_NAME "hello_core"
)
