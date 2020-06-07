cmake_minimum_required (VERSION 2.8.7)

project (flecs C)

file(GLOB flecs_SRC "src/*.c")

add_library(flecs SHARED ${flecs_SRC})
add_library(flecs_static STATIC ${flecs_SRC})

target_include_directories(flecs PUBLIC "include")
target_include_directories(flecs_static PUBLIC "include")

include_directories("include")

add_definitions(-DFLECS_IMPL)

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
	TARGETS flecs DESTINATION lib
)
