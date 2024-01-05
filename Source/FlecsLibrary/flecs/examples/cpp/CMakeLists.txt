cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0063 NEW)

project(flecs_cpp_examples LANGUAGES CXX)

set(FLECS_DIR ../..)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(${FLECS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/flecs)

include(../../cmake/target_default_compile_options.cmake)
include(../../cmake/target_default_compile_warnings.cmake)
include(../../cmake/target_default_compile_functions.cmake)

list_targets(EXAMPLES)

foreach (EXAMPLE ${EXAMPLES})
    create_target_cxx(${EXAMPLE} "")
    create_target_cxx(${EXAMPLE} "static")
endforeach ()
