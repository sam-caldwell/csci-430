# File: cmake/projects/logger/tests/integration.cmake
# Purpose: Logger integration tests (exercise via compiler phases)

file(GLOB_RECURSE LOGGER_INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/logger/integration/*.cpp
  ${PROJECT_SOURCE_DIR}/test/logger/integration/*/*.cpp)

add_executable(logger_integration_tests ${LOGGER_INTEGRATION_TEST_SOURCES})

target_include_directories(logger_integration_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(logger_integration_tests PRIVATE basic_compiler_lib GTest::gtest_main GTest::gtest)

gtest_discover_tests(logger_integration_tests PROPERTIES LABELS integration)
