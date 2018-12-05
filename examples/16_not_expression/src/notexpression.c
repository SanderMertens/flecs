#include <include/notexpression.h>
#include <reflecs/reflecs.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

void PrintStatic(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        printf("Entity '%s' matched\n", ecs_id(rows->world, entity));
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    /* System that prints all static (non-moving) entities */
    ECS_SYSTEM(world, PrintStatic, EcsOnFrame, Position, !Speed);

    /* Create entity with Position and Speed component */
    EcsHandle e1 = ecs_new(world, 0);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Speed, 1);
    ecs_set(world, e1, EcsId, "MovingEntity");

    /* Create entity with just a Position component */
    EcsHandle e2 = ecs_new(world, 0);
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, EcsId, "StaticEntity");

    /* Call ecs_progress. Both entities will be matched by the Move system */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
