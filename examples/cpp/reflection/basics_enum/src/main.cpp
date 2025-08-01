#include <basics_enum.h>
#include <iostream>

enum Color {
    Red,
    Green,
    Blue
};

struct TypeWithEnum {
    Color color;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register components with reflection data
    ecs.component<Color>()
        .constant("Red", Red)
        .constant("Green", Green)
        .constant("Blue", Blue);

    ecs.component<TypeWithEnum>()
        .member<Color>("color");

    // Create entity with TypeWithEnum as usual
    flecs::entity e = ecs.entity()
        .set<TypeWithEnum>({Green});

    // Convert TypeWithEnum component to flecs expression string
    const TypeWithEnum& c = e.get<TypeWithEnum>();
    std::cout << ecs.to_expr(&c).c_str() << "\n"; // {color: Green}
}
