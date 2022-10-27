#include <cpp_api.h>
#include <iostream>

// Helper to debug differences in function name strings across compilers

enum Color {
    Red, Green, Blue
};

template <typename T>
static size_t pretty_type() {
    std::cout << ECS_FUNC_NAME << std::endl;
    return 0;
}

template <typename E, E C>
static size_t pretty_enum() {
    std::cout << ECS_FUNC_NAME << std::endl;
    return 0;
}

void PrettyFunction_component() {
    pretty_type<Position>();
    test_assert(true);
}

void PrettyFunction_enum() {
    pretty_enum<Color, Color::Red>();
    pretty_enum<Color, (Color)3>();
    test_assert(true);
}
