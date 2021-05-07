#include <singleton.h>
#include <iostream>

struct Game {
    int score;
};

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Singleton components can simply be set on the world
    ecs.set<Game>({ 10 });

    // Similarly, they can be retrieved from the world as well
    const Game *g = ecs.get<Game>();
    std::cout << "Score: " << g->score << std::endl;

    // Systems can request a singleton components. Note that we need to mark the
    // component as InOut, as any component that is not stored on the entity
    // itself is readonly by default.
    ecs.system<Position>()
        .term<Game>().inout(flecs::InOut).singleton()
        .iter([](flecs::iter it, Position* p) {
            // The singleton component can be retrieved as a regular column
            auto game = it.term<Game>(2); // 2, because Position is 1

            for (auto i : it) {
                if (p[i].x > 1) {
                    // Use as pointer, since it's a single value
                    game->score ++;
                }
            }
        });

    // Create a few dummy entities
    ecs.entity().set<Position>({0, 1});
    ecs.entity().set<Position>({1, 2});
    ecs.entity().set<Position>({2, 3});

    // Run system
    ecs.progress();

    // Print score again (should be 1)
    std::cout << "Score: " << g->score << std::endl;
}
