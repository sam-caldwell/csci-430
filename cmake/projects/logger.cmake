# File: cmake/projects/logger.cmake
# Purpose: Define the logger library target

file(GLOB LOGGER_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/logger/*.cpp)

add_library(logger_lib STATIC ${LOGGER_SOURCES})
target_include_directories(logger_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

