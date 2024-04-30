project(flecs_tests LANGUAGES C CXX)

if(NOT BAKE_DIRECTORY)
  set(BAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bake")
else()
  set(BAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../${BAKE_DIRECTORY}")
endif()

if(NOT IS_DIRECTORY "${BAKE_DIRECTORY}")
  message(FATAL_ERROR "The CMake tests require a bake repository "
                      "at '${BAKE_DIRECTORY}'.")
endif()

macro(add_flecs_test DIRECTORY)
  get_filename_component(TEST_NAME_WE ${DIRECTORY} NAME_WE)

  file(
    GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    LIST_DIRECTORIES false
    "${DIRECTORY}/*.cpp" "${DIRECTORY}/*.c" "${DIRECTORY}/*.h")

  foreach(CURRENT_FLECS_TARGET IN LISTS FLECS_TARGETS)
    set(TEST_NAME "test_${TEST_NAME_WE}_${CURRENT_FLECS_TARGET}")
    set(CTEST_NAME "flecs_${TEST_NAME_WE}_test_${CURRENT_FLECS_TARGET}")

    message(STATUS "Adding test ${TEST_NAME}")

    add_executable("${TEST_NAME}" ${TEST_SOURCES})

    target_link_libraries("${TEST_NAME}" PUBLIC bake_base
                                                ${CURRENT_FLECS_TARGET})

    if(IS_DIRECTORY "${DIRECTORY}/include")
      target_include_directories(${TEST_NAME} PUBLIC "${DIRECTORY}/include")
    endif()

    set_target_properties(
      ${TEST_NAME}
      PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-j 1"
                 VS_DEBUGGER_ENVIRONMENT
                 "BAKE_TEST_INPLACE=1\nBAKE_VERBOSITY=OK")

    add_test(NAME ${CTEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)

  endforeach()
endmacro()

# Polyfill for bake
file(
  GLOB_RECURSE BAKE_SOURCES CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  "${BAKE_DIRECTORY}/src/*.c"
  "${BAKE_DIRECTORY}/src/*.h"
  "${BAKE_DIRECTORY}/drivers/test/src/*.c"
  "${BAKE_DIRECTORY}/drivers/test/src/*.h"
  "${BAKE_DIRECTORY}/util/src/*.c"
  "${BAKE_DIRECTORY}/util/src/*.h")

if(WIN32)
  list(FILTER BAKE_SOURCES EXCLUDE REGEX "src/posix")
else()
  list(FILTER BAKE_SOURCES EXCLUDE REGEX "src/win")
endif()

message(STATUS ${BAKE_DIRECTORY})

add_library(bake_base SHARED ${BAKE_SOURCES})

target_include_directories(
  bake_base PUBLIC "${BAKE_DIRECTORY}/include" "${BAKE_DIRECTORY}/util/include"
                   "${BAKE_DIRECTORY}/drivers/test/include")

target_compile_definitions(
  bake_base
  PUBLIC __BAKE__=1
  PRIVATE bake_test_EXPORTS=1 UT_IMPL=1
  _XOPEN_SOURCE=600)

if(WIN32)
  target_link_libraries(bake_base PUBLIC Dbghelp Shlwapi Ws2_32)
endif()
if(UNIX AND NOT APPLE)
  target_link_libraries(bake_base PUBLIC m)
endif()

add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/core")
add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/addons")
add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/query")
add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/meta")
add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/collections")
add_flecs_test("${CMAKE_CURRENT_LIST_DIR}/cpp")
