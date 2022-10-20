#include <system_ctx.h>
#include <iostream>
#include <math.h>

// Applications can pass context data to a system. A common use case where this
// comes in handy is when a system needs to iterate more than one query. The
// following example shows how to pass a custom query into a system for a simple
// collision detection example.

struct Position {
    double x, y;
};

struct Radius {
    double value;
};

double distance(const Position& p1, const Position& p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

double randd(int max) {
    return static_cast<double>((rand() % max));
}

using CollisionQuery = flecs::query<const Position, const Radius>;

int main(int, char *[]) {
    flecs::world ecs;

    CollisionQuery q_collide = ecs.query<const Position, const Radius>();

    auto sys = ecs.system<const Position, const Radius>("Collide")
        .ctx(&q_collide)
        .each([](flecs::iter& it, size_t i, const Position& p1, const Radius& r1) {
            CollisionQuery *q_collide = it.ctx<CollisionQuery>();
            flecs::entity e1 = it.entity(i);

            q_collide->each([&](flecs::entity e2, const Position& p2, const Radius& r2) {
                if (e1 == e2) {
                    // don't collide with self
                    return;
                }

                if (e1 > e2) {
                    // Simple trick to prevent collisions from being detected
                    // twice with the entities reversed.
                    return;
                }

                // Check for collision
                double d = distance(p1, p2);
                double r = r1.value + r2.value;
                if (r > d) {
                    std::cout << e1 << " (r: " << r1.value << ") and " 
                        << e2 << " (r: " << r2.value << ") collided! "
                        << "(d = " << d << "\n";
                }
            });
        });

    // Create a few test entities
    for (int i = 0; i < 10; i ++) {
        ecs.entity()
            .set<Position>({ randd(100), randd(100) })
            .set<Radius>({ randd(10) + 1 });
    }

    // Run the system
    sys.run();

    // Output
    //  526 (r: 4) and 529 (r: 6) collided! (d = 9.21954
    //  527 (r: 3) and 534 (r: 10) collided! (d = 3.60555
    //  529 (r: 6) and 531 (r: 10) collided! (d = 14.3178
    //  533 (r: 8) and 535 (r: 6) collided! (d = 11.4018
}
