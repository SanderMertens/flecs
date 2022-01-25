#include <hello_world.h>
#include <stdio.h>

// Component types
struct Position {
    double x;
    double y;
};

struct Velocity {
    double x;
    double y;
};

// Tag types
struct Eats { };
struct Apples { };

int main(int, char *[]) {
    // Create the world
    flecs::world ecs;

    // Register system
    ecs.system<Position, Velocity>()
        .each([](Position& p, Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    // Create an entity with name Bob, add Position and food preference
    auto Bob = ecs.entity("Bob")
        .set(Position{0, 0})
        .set(Velocity{1, 2})
        .add<Eats, Apples>();

    // Show us what you got
    printf("%s's got: [%s]\n", Bob.name().c_str(), Bob.type().str().c_str());

    // Run systems twice. Usually this function is called once per frame
    ecs.progress();
    ecs.progress();

    // See if Bob has moved (he has)
    const Position *p = Bob.get<Position>();
    printf("Bob's position is {%f, %f}\n", p->x, p->y);
}
