# File: cmake/projects/hello_world/tests/unit.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Hello-world unit tests.

# Collect all unit test sources under test/hello_world/unit
file(GLOB HELLO_WORLD_UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/hello_world/unit/*.cpp)

add_executable(hello_world_tests ${HELLO_WORLD_UNIT_TEST_SOURCES})

target_include_directories(hello_world_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(hello_world_tests PRIVATE hello_core GTest::gtest_main)

gtest_discover_tests(hello_world_tests PROPERTIES LABELS unit)
