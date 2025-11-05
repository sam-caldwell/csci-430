# File: cmake/projects/basic_compiler/tests/unit.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Basic compiler unit tests (lexer/parser).

# Collect all unit test sources under test/basic_compiler/unit
file(GLOB BASIC_COMPILER_UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/basic_compiler/unit/*.cpp)

add_executable(basic_compiler_unit_tests ${BASIC_COMPILER_UNIT_TEST_SOURCES})

target_include_directories(basic_compiler_unit_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(basic_compiler_unit_tests PRIVATE basic_compiler_lib GTest::gtest_main GTest::gtest)

# Suppress expected warnings in unit tests that intentionally redefine access
# specifiers (e.g., `#define private public`) to introspect internals.
# Clang warns with -Wkeyword-macro; silence it for this target only.
target_compile_options(basic_compiler_unit_tests PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wno-keyword-macro>)

gtest_discover_tests(basic_compiler_unit_tests PROPERTIES LABELS unit)
