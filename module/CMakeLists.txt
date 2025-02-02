add_library(flecs-module)

target_sources(flecs-module PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS ${PROJECT_SOURCE_DIR}/module
    FILES flecs-module.cpp
)

target_compile_features(flecs-module PUBLIC cxx_std_20)
target_link_libraries(flecs-module PRIVATE flecs)
