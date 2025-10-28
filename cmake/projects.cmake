# File: cmake/projects.cmake
# (c) 2025 Sam Caldwell.  All Rights Reserved.
# Purpose: Central registry of assignment/example targets for this course.
#          Add new targets with build_project(target sources...).
# List all targets to build here using build_project(target sources...)

# Primary C program (first project)
build_project(hello_world src/main.c src/hello_world.c)
target_include_directories(hello_world PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Unit tests (GoogleTest)
add_library(hello_core STATIC src/hello_world.c)
target_include_directories(hello_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(hello_world_tests test/test_hello_world.cpp)
target_include_directories(hello_world_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hello_world_tests PRIVATE hello_core GTest::gtest_main)
gtest_discover_tests(hello_world_tests)
