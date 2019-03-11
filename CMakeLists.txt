cmake_minimum_required (VERSION 2.6)

project (reflecs)

file(GLOB reflecs_SRC "src/*.c")

include_directories(".")

add_library(reflecs SHARED ${reflecs_SRC})
