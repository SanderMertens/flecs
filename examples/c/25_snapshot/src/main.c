#include <snapshot.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

void Move(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("Move  {%f, %f}\n", p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_entity_t e = 
    ecs_set(world, 0, Position, {0, 0});
    ecs_set(world, e, Velocity, {1, 2});

    /* Take a snapshot that records the current state of the entity */
    printf("Take snapshot\n");
    ecs_snapshot_t *s = ecs_snapshot_take(world);

    /* Progress the world a few times, updates position */
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    /* Restore snapshot. The snapshot cannot be used anymore after restoring.
     * If an application still wants to use this snapshot after restoring, a new
     * snapshot can be taken. */
    printf("Restore snapshot\n");
    ecs_snapshot_restore(world, s);

    /* Progress the world again, note that the state has been restored */
    ecs_progress(world, 0);

    /* Cleanup */
    return ecs_fini(world);
}
