# File: cmake/projects/clang_tidy_docstring/tests/unit.cmake
# Purpose: Unit tests for clang-tidy-docstring library

file(GLOB CLANG_TIDY_DOCSTRING_UNIT_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/clang-tidy-docstring/unit/*.cpp)

add_executable(clang_tidy_docstring_unit_tests ${CLANG_TIDY_DOCSTRING_UNIT_SOURCES})

target_include_directories(clang_tidy_docstring_unit_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(clang_tidy_docstring_unit_tests PRIVATE clang_tidy_docstring_lib GTest::gtest_main GTest::gtest)

gtest_discover_tests(clang_tidy_docstring_unit_tests PROPERTIES LABELS unit FAIL_REGULAR_EXPRESSION "warning:;WARNING:")
