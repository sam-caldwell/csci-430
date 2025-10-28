# File: cmake/projects/basic_compiler/tests/integration.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Basic compiler integration tests (IR structure).

# Collect all integration test sources under test/basic_compiler/integration
file(GLOB BASIC_COMPILER_INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/basic_compiler/integration/*.cpp)

add_executable(basic_compiler_integration_tests ${BASIC_COMPILER_INTEGRATION_TEST_SOURCES})

target_include_directories(basic_compiler_integration_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(basic_compiler_integration_tests PRIVATE basic_compiler_lib GTest::gtest_main)
# Ensure hello_world builds first
add_dependencies(basic_compiler_integration_tests hello_world)

gtest_discover_tests(basic_compiler_integration_tests PROPERTIES LABELS integration)
