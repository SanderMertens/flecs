cmake_minimum_required(VERSION 3.16)

# do not rely on cmake presets
# and use target configuration

set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "")
set(CMAKE_C_FLAGS_MINSIZEREL "")

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set(CMAKE_CXX_FLAGS_MINSIZEREL "")

function(target_default_compile_options_c THIS)

    set_target_properties(${THIS} PROPERTIES
            LINKER_LANGUAGE C
            C_STANDARD 99
            C_STANDARD_REQUIRED ON)

    if (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:-g -O0 -DDEBUG>
                $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:-g -O2 -DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:-Os -DNDEBUG>)

    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:/Zi /Od /Ob0 /RTC1>
                $<$<CONFIG:RELEASE>:/O2 /Ob2 /DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:/Zi /O2 /Ob1 /DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:/O1 /Ob1 /DNDEBUG>)

    else ()

        message(WARNING
                "No Options specified for ${CMAKE_C_COMPILER_ID}. "
                "Consider using one of the following compilers: Clang, GNU, MSVC.")

    endif ()

endfunction()

function(target_default_compile_options_cxx THIS)

    set_target_properties(${THIS} PROPERTIES
            LINKER_LANGUAGE CXX
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED ON)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:-g -O0 -DDEBUG>
                $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:-g -O2 -DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:-Os -DNDEBUG>)

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:/Zi /Od /Ob0 /RTC1>
                $<$<CONFIG:RELEASE>:/O2 /Ob2 /DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:/Zi /O2 /Ob1 /DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:/O1 /Ob1 /DNDEBUG>)

    else ()

        message(WARNING
                "No Options specified for ${CMAKE_CXX_COMPILER_ID}. "
                "Consider using one of the following compilers: Clang, GNU, MSVC.")

    endif ()

endfunction()
