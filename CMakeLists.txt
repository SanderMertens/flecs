cmake_minimum_required(VERSION 3.5)

project(flecs LANGUAGES C)

# include utilities for compiler options and warnings

include(cmake/target_default_compile_warnings.cmake)
include(cmake/target_default_compile_options.cmake)

# collect all include and source files
# (use the manual list to improve ide support)

set(INC
        include/flecs/addons/bulk.h
        include/flecs/addons/dbg.h
        include/flecs/addons/direct_access.h
        include/flecs/addons/module.h
        include/flecs/addons/queue.h
        include/flecs/addons/reader_writer.h
        include/flecs/addons/snapshot.h
        include/flecs/addons/stats.h
        include/flecs/modules/pipeline.h
        include/flecs/modules/system.h
        include/flecs/modules/timer.h
        include/flecs/private/api_defines.h
        include/flecs/private/api_support.h
        include/flecs/private/api_types.h
        include/flecs/private/bitset.h
        include/flecs/private/entity_index.h
        include/flecs/private/log.h
        include/flecs/private/map.h
        include/flecs/private/sparse.h
        include/flecs/private/strbuf.h
        include/flecs/private/switch_list.h
        include/flecs/private/vector.h
        include/flecs.h
        include/flecs/bake_config.h
        include/flecs/flecs.hpp
        include/flecs/os_api.h
        include/flecs/type.h)

set(SRC
        src/addons/bulk.c
        src/addons/dbg.c
        src/addons/direct_access.c
        src/addons/module.c
        src/addons/queue.c
        src/addons/reader.c
        src/addons/snapshot.c
        src/addons/stats.c
        src/addons/writer.c
        src/modules/pipeline/pipeline.c
        src/modules/pipeline/pipeline.h
        src/modules/pipeline/worker.c
        src/modules/system/system.c
        src/modules/system/system.h
        src/modules/system/system_dbg.c
        src/modules/timer.c
        src/api_support.c
        src/bitset.c
        src/bootstrap.c
        src/entity.c
        src/filter.c
        src/hash.c
        src/hierarchy.c
        src/iter.c
        src/log.c
        src/map.c
        src/misc.c
        src/os_api.c
        src/private_api.h
        src/private_types.h
        src/query.c
        src/signature.c
        src/sparse.c
        src/stage.c
        src/strbuf.c
        src/switch_list.c
        src/table.c
        src/table_graph.c
        src/type.c
        src/vector.c
        src/world.c)

# build the shared library

add_library(flecs SHARED ${INC} ${SRC})

target_default_compile_options_c(flecs)
target_default_compile_warnings_c(flecs)

target_include_directories(flecs PUBLIC include)

# build the static library

add_library(flecs_static STATIC ${INC} ${SRC})

target_default_compile_options_c(flecs_static)
target_default_compile_warnings_c(flecs_static)

target_include_directories(flecs_static PUBLIC include)
target_compile_definitions(flecs_static PUBLIC flecs_STATIC)

# define the install steps

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp")

install(TARGETS flecs
        DESTINATION lib)

install(TARGETS flecs_static
        DESTINATION lib)
