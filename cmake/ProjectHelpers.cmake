#! File: cmake/ProjectHelpers.cmake
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Provide helper functions/macros including build_project(target sources...).
#!          Builds executables and LLVM artifacts into build/<target>/.
include_guard(GLOBAL)

# build_project(target src1 [src2 ...])
# - Creates an executable target and emits artifacts into build/<target>/
# - Artifacts: binary, per-source .bc and .ll, linked <target>.bc and <target>.ll
function(build_project target)
  if (NOT ARGN)
    message(FATAL_ERROR "build_project(${target}): no sources provided")
  endif()

  if(NOT CLANG_EXECUTABLE OR NOT CLANGXX_EXECUTABLE OR NOT LLVM_LINK_EXECUTABLE OR NOT LLVM_DIS_EXECUTABLE)
    message(FATAL_ERROR "LLVM/Clang tools not found. Ensure 'llvm' is installed and cmake/LLVMConfig.cmake ran.")
  endif()

  set(_sources ${ARGN})
  add_executable(${target} ${_sources})

  # Output directory as requested
  set(_outdir "${PROJECT_SOURCE_DIR}/build/${target}")
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${_outdir}"
    LIBRARY_OUTPUT_DIRECTORY "${_outdir}"
    OUTPUT_NAME "${target}"
  )

  # Collect generator expressions for target properties to pass to clang invocations
  # Use CPATH to convey include directories to avoid argument quoting issues.
  set(_cpath_expr "$<$<BOOL:$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>>:$<JOIN:$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>,:>>")
  set(_def_expr   "$<$<BOOL:$<TARGET_PROPERTY:${target},COMPILE_DEFINITIONS>>:-D$<JOIN:$<TARGET_PROPERTY:${target},COMPILE_DEFINITIONS>, -D>>")
  set(_opt_expr   "$<$<BOOL:$<TARGET_PROPERTY:${target},COMPILE_OPTIONS>>:$<JOIN:$<TARGET_PROPERTY:${target},COMPILE_OPTIONS>, >>")

  # Per-source bitcode and IR
  set(_bc_files)
  set(_ll_files)
  foreach(_src IN LISTS _sources)
      get_filename_component(_abs_src "${_src}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
      get_filename_component(_base "${_src}" NAME_WE)
      get_filename_component(_ext "${_src}" EXT)
      string(TOLOWER "${_ext}" _ext_l)

      # Create a unique, stable module identifier from the relative source path
      set(_mod_id "${_src}")
      string(REGEX REPLACE "[^A-Za-z0-9]+" "_" _mod_id "${_mod_id}")

      if(_ext_l STREQUAL ".c")
        set(_compiler "${CLANG_EXECUTABLE}")
        # Ensure C standard is respected for bitcode generation. Map c23->c2x for Clang 17.
        if(CMAKE_C_EXTENSIONS)
          if(CMAKE_C_STANDARD EQUAL 23)
            set(_std_flag "-std=gnu2x")
          else()
            set(_std_flag "-std=gnu${CMAKE_C_STANDARD}")
          endif()
        else()
          if(CMAKE_C_STANDARD EQUAL 23)
            set(_std_flag "-std=c2x")
          else()
            set(_std_flag "-std=c${CMAKE_C_STANDARD}")
          endif()
        endif()
      else()
        set(_compiler "${CLANGXX_EXECUTABLE}")
        # Ensure C++ standard (e.g., C++23) is respected for bitcode generation
        if(CMAKE_CXX_EXTENSIONS)
          set(_std_flag "-std=gnu++${CMAKE_CXX_STANDARD}")
        else()
          set(_std_flag "-std=c++${CMAKE_CXX_STANDARD}")
        endif()
      endif()

      set(_bc "${_outdir}/${_mod_id}.module.bc")
      set(_ll "${_outdir}/${_mod_id}.module.ll")

      add_custom_command(
        OUTPUT "${_bc}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_outdir}"
        COMMAND ${CMAKE_COMMAND} -E env CPATH=${_cpath_expr} "${_compiler}" ${_std_flag} -emit-llvm -c "${_abs_src}" -o "${_bc}" ${_def_expr} ${_opt_expr}
        DEPENDS "${_abs_src}"
        COMMENT "Generating LLVM bitcode ${_bc}"
        VERBATIM
      )

      add_custom_command(
        OUTPUT "${_ll}"
        COMMAND "${LLVM_DIS_EXECUTABLE}" "${_bc}" -o "${_ll}"
        DEPENDS "${_bc}"
        COMMENT "Disassembling ${_bc} to ${_ll}"
        VERBATIM
      )

      list(APPEND _bc_files "${_bc}")
      list(APPEND _ll_files "${_ll}")
  endforeach()

  # Linked bitcode and IR for the whole target
  set(_linked_bc "${_outdir}/${target}.bc")
  set(_linked_ll "${_outdir}/${target}.ll")

    add_custom_command(
      OUTPUT "${_linked_bc}"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${_outdir}"
      COMMAND "${LLVM_LINK_EXECUTABLE}" -o "${_linked_bc}" ${_bc_files}
      DEPENDS ${_bc_files}
      COMMENT "Linking LLVM bitcode for ${target} -> ${_linked_bc}"
      VERBATIM
    )

    add_custom_command(
      OUTPUT "${_linked_ll}"
      COMMAND "${LLVM_DIS_EXECUTABLE}" "${_linked_bc}" -o "${_linked_ll}"
      DEPENDS "${_linked_bc}"
      COMMENT "Generating linked LLVM IR for ${target} -> ${_linked_ll}"
      VERBATIM
    )

    # Always build these artifacts with the default target
  add_custom_target("${target}_bc" ALL DEPENDS ${_bc_files} "${_linked_bc}")
  add_custom_target("${target}_ir" ALL DEPENDS ${_ll_files} "${_linked_ll}")
  add_dependencies(${target} "${target}_bc" "${target}_ir")
endfunction()
