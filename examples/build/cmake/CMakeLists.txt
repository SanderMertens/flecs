cmake_minimum_required (VERSION 2.8.7)

project (cmake_example)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory (flecs)

add_executable(cmake_example main.cpp)
add_executable(cmake_example_static main.cpp)

target_link_libraries(cmake_example flecs)
target_link_libraries(cmake_example_static flecs_static)
