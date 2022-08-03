
macro(list_targets RESULT)
    file(GLOB_RECURSE FILES LIST_DIRECTORIES true RELATIVE ${CUR_DIR} *)
    set(TARGET_LIST "")

    foreach (FILE ${FILES})
        if (IS_DIRECTORY ${CUR_DIR}/${FILE})
            if (EXISTS ${CUR_DIR}/${FILE}/project.json)
                message(STATUS "Found example ${FILE}")
                list(APPEND TARGET_LIST ${FILE})
            endif()
        endif ()
    endforeach ()

    set(${RESULT} ${TARGET_LIST})
endmacro()

macro(create_target TARGET CONFIG)
    if (CONFIG STREQUAL "")
        set(TARGET_POSTFIX "")
    else()
        set(TARGET_POSTFIX "_${CONFIG}")
    endif()

    set(TARGET_NAME "${TARGET}${TARGET_POSTFIX}")
    string(REPLACE "/" "_" TARGET_NAME "${TARGET_NAME}")
    string(REPLACE "\\" "_" TARGET_NAME "${TARGET_NAME}")
    set(TARGET_DIR "${CUR_DIR}/${TARGET}")

    file(GLOB SRC ${TARGET_DIR}/src/*.c*)
    add_executable(${TARGET_NAME} ${SRC})

    target_include_directories(${TARGET_NAME} PUBLIC ${FLECS_DIR}/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${TARGET_DIR}/include)
    target_link_libraries(${TARGET_NAME} flecs${TARGET_POSTFIX})
    endmacro()

function(create_target_c TARGET CONFIG)
    create_target("${TARGET}" "${CONFIG}")
    target_default_compile_options_c(${TARGET_NAME})
    target_default_compile_warnings_c(${TARGET_NAME})
endfunction()

function(create_target_cxx TARGET CONFIG)
    create_target("${TARGET}" "${CONFIG}")
    target_default_compile_options_cxx(${TARGET_NAME})
    target_default_compile_warnings_cxx(${TARGET_NAME})
endfunction()
