#   cmake --build . --target add_test_docstrings
# or standalone:
#   cmake -DPROJECT_ROOT="$PWD" -P cmake/tools/add_test_docstrings.cmake
#
add_custom_target(add_test_docstrings
        COMMAND ${CMAKE_COMMAND} -Wdev -Werror=dev
        -DPROJECT_ROOT=${CMAKE_SOURCE_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/tools/add_test_docstrings.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Inserting missing test docstrings"
        VERBATIM
)
