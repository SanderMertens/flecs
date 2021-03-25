// Some compilers will flag $Game as an identifier with a $ symbol. This is not
// correct, as $Game is stringified by the ECS_SYSTEM macro.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#include <singleton.h>

typedef struct Game {
    int score;
} Game;

typedef struct {
    double x, y;
} Position;

void KeepScore(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    // The singleton component can be retrieved as a regular column
    Game *g = ecs_term(it, Game, 2);

    for (int i = 0; i < it->count; i ++) {
        if (p[i].x > 1) {
            // Use as pointer, since it's a single value
            g->score ++;
        }
    }
}

int main(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Game);

    // Systems can request a singleton component by prefixing the name with $
    ECS_SYSTEM(world, KeepScore, EcsOnUpdate, Position, $Game);

    // Singleton components can simply be set on the world
    ecs_singleton_set(world, Game, {10});

    // Similarly, they can be retrieved from the world as well
    const Game *g = ecs_singleton_get(world, Game);
    printf("Score: %d\n", g->score);

    // Create a few dummy entities
    ecs_set(world, 0, Position, {0, 1});
    ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, 0, Position, {2, 3});

    // Run system
    ecs_progress(world, 0);

    printf("Score: %d\n", g->score);

    return ecs_fini(world);
}
