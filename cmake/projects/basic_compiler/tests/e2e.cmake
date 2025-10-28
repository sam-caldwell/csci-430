# File: cmake/projects/basic_compiler/tests/e2e.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Basic compiler end-to-end tests (compile & run programs).

# Collect all E2E test sources under test/basic_compiler/e2e
file(GLOB BASIC_COMPILER_E2E_TEST_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/basic_compiler/e2e/*.cpp)

add_executable(basic_compiler_e2e_tests ${BASIC_COMPILER_E2E_TEST_SOURCES})

target_include_directories(basic_compiler_e2e_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(basic_compiler_e2e_tests PRIVATE basic_compiler_lib GTest::gtest_main)
add_dependencies(basic_compiler_e2e_tests hello_world)

if (CLANG_EXECUTABLE)
  target_compile_definitions(basic_compiler_e2e_tests PRIVATE CLANG_PATH="${CLANG_EXECUTABLE}")
else()
  target_compile_definitions(basic_compiler_e2e_tests PRIVATE CLANG_PATH="clang")
endif()

gtest_discover_tests(basic_compiler_e2e_tests PROPERTIES LABELS e2e)
