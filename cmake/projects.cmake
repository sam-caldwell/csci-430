# File: cmake/projects.cmake
# (c) 2025 Sam Caldwell.  All Rights Reserved.
# Purpose: Central registry delegating to modular project definitions.

include(cmake/projects/clang_tidy_docstring.cmake)

include(cmake/projects/hello_world.cmake)
include(cmake/projects/hello_world/tests/unit.cmake)

include(cmake/projects/logger.cmake)
include(cmake/projects/logger/tests/unit.cmake)
include(cmake/projects/logger/tests/integration.cmake)
include(cmake/projects/logger/tests/e2e.cmake)

include(cmake/projects/basic_compiler.cmake)
include(cmake/projects/basic_compiler/tests/unit.cmake)
include(cmake/projects/basic_compiler/tests/integration.cmake)
include(cmake/projects/basic_compiler/tests/e2e.cmake)

# clang-tidy-docstring tests
include(cmake/projects/clang_tidy_docstring/tests/unit.cmake)

# Provide an ordered ctest target (unit -> integration -> e2e)
add_custom_target(ordered_ctest
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L unit
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L integration
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L e2e
  DEPENDS basic_compiler_unit_tests basic_compiler_integration_tests basic_compiler_e2e_tests hello_world_tests logger_unit_tests logger_integration_tests logger_e2e_tests
  USES_TERMINAL
)

# Ensure the docstring validator is built (and runs) before any project targets.
# Apply to known targets if present; future projects can extend similarly.
foreach(_tgt
        hello_world hello_core
        logger_lib
        basic_compiler_lib basic_compiler basic_compiler_bc basic_compiler_ir
        hello_world_tests logger_unit_tests logger_integration_tests logger_e2e_tests
        basic_compiler_unit_tests basic_compiler_integration_tests basic_compiler_e2e_tests)
  if (TARGET ${_tgt})
    add_dependencies(${_tgt} clang_tidy_docstring)
  endif()
endforeach()
