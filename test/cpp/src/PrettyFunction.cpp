#include <cpp.h>
#include <iostream>

// Helper to debug differences in function name strings across compilers

enum Color {
    Red, Green, Blue
};

template <typename T, typename U>
struct ColorContainer {
    enum Value {
        Zero, One, Two
    };
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

void PrettyFunction_component(void) {
    pretty_type<Position>();
    test_assert(true);
}

void PrettyFunction_enum(void) {
    using NestedColor = ColorContainer<int, int>::Value;

    static_assert(flecs::_::enum_constant_is_valid<Color, Color::Red>(),
        "valid enum constants must be detected");
    static_assert(!flecs::_::enum_constant_is_valid<Color, (Color)3>(),
        "invalid enum constants must be rejected");
    static_assert(
        flecs::_::enum_constant_is_valid<
            NestedColor, ColorContainer<int, int>::Zero>(),
        "template enum types with commas in their name must be supported");
    static_assert(
        !flecs::_::enum_constant_is_valid<NestedColor, (NestedColor)3>(),
        "invalid template enum constants must be rejected");

    pretty_enum<Color, Color::Red>();
    pretty_enum<Color, (Color)3>();
    test_assert(true);
}
