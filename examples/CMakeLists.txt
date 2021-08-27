# Help needed with updating the cmake file to new directory structure!

cmake_minimum_required(VERSION 3.5)

project(flecs_example LANGUAGES C CXX)

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/flecs)

include(../cmake/target_default_compile_options.cmake)
include(../cmake/target_default_compile_warnings.cmake)

macro(SUB_DIR_LIST RESULT CURRENT_DIR)

    file(GLOB CHILDREN LIST_DIRECTORIES true RELATIVE ${CURRENT_DIR} ${CURRENT_DIR}/*)
    set(DIRLIST "")

    foreach (CHILD ${CHILDREN})
        if (IS_DIRECTORY ${CURRENT_DIR}/${CHILD})
            list(APPEND DIRLIST ${CHILD})
        endif ()
    endforeach ()

    set(${RESULT} ${DIRLIST})

endmacro()

get_filename_component(PARENT_DIR ../ ABSOLUTE)
macro(CREATE_TARGET NAME PARENT)

    set(TARGET_NAME example_${PARENT}_${NAME})
    set(TARGET_NAME_STATIC example_${PARENT}_${NAME}_static)
    set(TARGET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PARENT}/${NAME})
    set(SRC_FILES "")
    aux_source_directory(${TARGET_PATH}/src/ SRC_FILES)

    add_executable(${TARGET_NAME} ${SRC_FILES})
    add_executable(${TARGET_NAME_STATIC} ${SRC_FILES})

    if (${PARENT} STREQUAL c)
        target_default_compile_options_c(${TARGET_NAME})
        target_default_compile_warnings_c(${TARGET_NAME_STATIC})
    else ()
        target_default_compile_options_cxx(${TARGET_NAME})
        target_default_compile_warnings_cxx(${TARGET_NAME_STATIC})
    endif ()

    target_include_directories(${TARGET_NAME} PUBLIC ${PARENT_DIR}/include)
    target_include_directories(${TARGET_NAME_STATIC} PUBLIC ${PARENT_DIR}/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${TARGET_PATH}/include)
    target_include_directories(${TARGET_NAME_STATIC} PUBLIC ${TARGET_PATH}/include)
    target_link_libraries(${TARGET_NAME} flecs)
    target_link_libraries(${TARGET_NAME_STATIC} flecs_static)

endmacro()

SUB_DIR_LIST(C_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/c)
foreach (C_SUBDIR ${C_SUBDIRS})
    CREATE_TARGET(${C_SUBDIR} c)
endforeach ()

SUB_DIR_LIST(CPP_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
foreach (CPP_SUBDIR ${CPP_SUBDIRS})
    CREATE_TARGET(${CPP_SUBDIR} cpp)
endforeach ()
