# File: cmake/projects.cmake
# (c) 2025 Sam Caldwell.  All Rights Reserved.
# Purpose: Central registry delegating to modular project definitions.

include(cmake/projects/hello_world.cmake)
include(cmake/projects/hello_world/tests/unit.cmake)

include(cmake/projects/basic_compiler.cmake)
include(cmake/projects/basic_compiler/tests/unit.cmake)
include(cmake/projects/basic_compiler/tests/integration.cmake)
include(cmake/projects/basic_compiler/tests/e2e.cmake)

# Provide an ordered ctest target (unit -> integration -> e2e)
add_custom_target(ordered_ctest
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L unit
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L integration
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L e2e
  DEPENDS basic_compiler_unit_tests basic_compiler_integration_tests basic_compiler_e2e_tests hello_world_tests
  USES_TERMINAL
)
