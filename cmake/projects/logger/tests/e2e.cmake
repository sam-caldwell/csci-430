# File: cmake/projects/logger/tests/e2e.cmake
# Purpose: Logger end-to-end tests (exercise CLI with log flags)

file(GLOB_RECURSE LOGGER_E2E_TEST_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/logger/e2e/*.cpp
  ${PROJECT_SOURCE_DIR}/test/logger/e2e/*/*.cpp)

add_executable(logger_e2e_tests ${LOGGER_E2E_TEST_SOURCES})

target_include_directories(logger_e2e_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(logger_e2e_tests PRIVATE basic_compiler_lib GTest::gtest_main GTest::gtest)

if (CLANG_EXECUTABLE)
  target_compile_definitions(logger_e2e_tests PRIVATE CLANG_PATH="${CLANG_EXECUTABLE}")
else()
  target_compile_definitions(logger_e2e_tests PRIVATE CLANG_PATH="clang")
endif()

gtest_discover_tests(logger_e2e_tests PROPERTIES LABELS e2e FAIL_REGULAR_EXPRESSION "warning:;WARNING:" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
