#include <snapshot_w_filter.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

typedef float Mass;

void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("Move '%s' {%f, %f}\n", 
            ecs_get_id(rows->world, rows->entities[i]), p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_entity_t e1 = 
    ecs_set(world, 0, EcsId, {"E1"});
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 =
    ecs_set(world, 0, EcsId, {"E2"});
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, Velocity, {1, 2});
    ecs_set(world, e2, Mass, {10});

    /* Take a snapshot that records the current state of the entity. Filter out
     * any entities that have the 'Mass' component. */
    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .exclude = ecs_type(Mass)
    });

    /* Progress the world a few times, updates position */
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    /* Restore snapshot. This will only restore the state for entity E1. */
    printf("Restore snapshot\n");
    ecs_snapshot_restore(world, s);

    /* Progress the world again, note that the state of E1 has been restored */
    ecs_progress(world, 0);

    /* Cleanup */
    return ecs_fini(world);
}