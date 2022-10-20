#include <system_ctx.h>
#include <iostream>

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

double sqr(double value) {
    return value * value;
}

double distance_sqr(const Position& p1, const Position& p2) {
    return sqr(p2.x - p1.x) + sqr(p2.y - p1.y);
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
            CollisionQuery *q = it.ctx<CollisionQuery>();
            flecs::entity e1 = it.entity(i);

            q->each([&](flecs::entity e2, const Position& p2, const Radius& r2) {
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
                double d_sqr = distance_sqr(p1, p2);
                double r_sqr = sqr(r1.value + r2.value);
                if (r_sqr > d_sqr) {
                    std::cout << e1 << " and " << e2 << " collided!\n";
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
    //  526 and 529 collided!
    //  527 and 534 collided!
    //  529 and 531 collided!
    //  533 and 535 collided!
}
