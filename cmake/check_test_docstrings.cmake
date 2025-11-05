# Usage: cmake --build . --target check_test_docstrings
add_custom_target(check_test_docstrings
        COMMAND ${CMAKE_COMMAND}
            -DPROJECT_ROOT=${CMAKE_SOURCE_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/tools/check_test_docstrings.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking test docstrings and single-TEST constraint"
        VERBATIM
)
