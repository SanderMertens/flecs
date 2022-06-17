cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0063 NEW)

project(flecs LANGUAGES C)

option(FLECS_STATIC_LIBS "Build static flecs lib" ON)
option(FLECS_PIC "Compile static flecs lib with position-independent-code (PIC)" ON)
option(FLECS_SHARED_LIBS "Build shared flecs lib" ON)
option(FLECS_DEVELOPER_WARNINGS "Enable more warnings" OFF)

if(NOT FLECS_STATIC_LIBS AND NOT FLECS_SHARED_LIBS)
    message(FATAL_ERROR "At least one of FLECS_STATIC_LIBS or FLECS_SHARED_LIBS options must be enabled")
endif()

# include utilities for compiler options and warnings

include(cmake/target_default_compile_warnings.cmake)
include(cmake/target_default_compile_options.cmake)

# collect all include and source files
# (use the manual list to improve ide support)

file(GLOB_RECURSE INC include/*.h include/*.hpp)

file(GLOB_RECURSE SRC src/*.c)

set(FLECS_TARGETS "")

# build the shared library
if(FLECS_SHARED_LIBS)
    add_library(flecs SHARED ${INC} ${SRC})
    add_library(flecs::flecs ALIAS flecs)

    target_default_compile_options_c(flecs)
    if(FLECS_DEVELOPER_WARNINGS)
        target_default_compile_warnings_c(flecs)
    endif()

    if(WIN32)
        target_link_libraries(flecs wsock32 ws2_32)
    endif()

    target_include_directories(flecs PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

    list(APPEND FLECS_TARGETS flecs)
endif()

# build the static library
if(FLECS_STATIC_LIBS)
    add_library(flecs_static STATIC ${INC} ${SRC})
    add_library(flecs::flecs_static ALIAS flecs_static)

    target_default_compile_options_c(flecs_static)
    if(FLECS_DEVELOPER_WARNINGS)
        target_default_compile_warnings_c(flecs_static)
    endif()

    if(WIN32)
        target_link_libraries(flecs_static wsock32 ws2_32)
    endif()

    if(FLECS_PIC)
        set_property(TARGET flecs_static PROPERTY POSITION_INDEPENDENT_CODE ON)
    endif()

    target_include_directories(flecs_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
    target_compile_definitions(flecs_static PUBLIC flecs_STATIC)

    list(APPEND FLECS_TARGETS flecs_static)
endif()

# define the install steps
include(GNUInstallDirs)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl")

install(TARGETS ${FLECS_TARGETS}
        EXPORT flecs-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT flecs-export
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flecs
        NAMESPACE flecs::
        FILE flecs-config.cmake)
