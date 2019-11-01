cmake_minimum_required (VERSION 2.8.7)

project (flecs C)

file(GLOB flecs_SRC "src/*.c")

include_directories("include")

add_definitions(-DFLECS_IMPL)

add_library(flecs_static STATIC ${flecs_SRC})
add_library(flecs_shared SHARED ${flecs_SRC})

if (TARGET flecs_static)
	add_definitions(-DPRIVATE -DFLECS_STATIC)
endif()

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h"
)
install(
	TARGETS flecs_static DESTINATION lib
)
install(
	TARGETS flecs_shared DESTINATION lib
)
