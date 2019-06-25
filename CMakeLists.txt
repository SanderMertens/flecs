cmake_minimum_required (VERSION 2.6)

project (flecs C)

file(GLOB flecs_SRC "src/*.c")

include_directories("include")

add_compile_definitions(FLECS_IMPL)

add_library(flecs_static STATIC ${flecs_SRC})
add_library(flecs_shared SHARED ${flecs_SRC})

target_compile_definitions(flecs_static PRIVATE FLECS_STATIC)


install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h"
)
install(
	TARGETS flecs_static DESTINATION lib
)
install(
	TARGETS flecs_shared DESTINATION lib
)
