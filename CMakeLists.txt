cmake_minimum_required (VERSION 2.6)

project (flecs)

file(GLOB flecs_SRC "src/*.c")

include_directories(".")

add_library(flecs SHARED ${flecs_SRC})
