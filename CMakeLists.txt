cmake_minimum_required (VERSION 2.8.7)

project (flecs C)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE flecs_SRC "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

add_library(flecs SHARED ${flecs_SRC})
add_library(flecs_static STATIC ${flecs_SRC})

target_include_directories(flecs PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(flecs_static PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h"
)
install(
	TARGETS flecs_static DESTINATION lib
)
install(
	TARGETS flecs DESTINATION lib
)
