# File: cmake/projects/logger/tests/unit.cmake
# Purpose: Logger unit tests

file(GLOB LOGGER_UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/logger/unit/*.cpp)

add_executable(logger_unit_tests ${LOGGER_UNIT_TEST_SOURCES})

target_include_directories(logger_unit_tests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(logger_unit_tests PRIVATE logger_lib GTest::gtest_main GTest::gtest)

gtest_discover_tests(logger_unit_tests PROPERTIES LABELS unit)

