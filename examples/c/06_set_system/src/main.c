#include <set_system.h>

typedef struct Position {
    float x;
    float y;
} Position;

/* This system will be called when Position is added */
void AddPosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    for (int i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
        printf("Position added\n");
    }
}

/* This system will be called when Position is set */
void SetPosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("Position set -> {%f, %f}\n",
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register Position component */
    ECS_COMPONENT(world, Position);

    /* Register system that is invoked when Position is added */
    ECS_SYSTEM(world, AddPosition, EcsOnAdd, Position);

    /* Register system that is invoked when a value is assigned to Position.
     * There are different conditions under which an OnSet system is triggerd.
     * This example demonstrates how OnSet is called after an OnAdd system, and
     * after calling ecs_set. */
    ECS_SYSTEM(world, SetPosition, EcsOnSet, Position);

    /* Create new entity with Position. Because we have an OnAdd system, flecs
     * assumes a valid value will be assigned to Position, and therefore the
     * OnSet system is invoked directly after the OnAdd system. */
    ecs_entity_t e = ecs_new(world, Position);

    /* Set Position to a new value (invokes OnSet system) */
    ecs_set(world, e, Position, {20, 30});

    /* Set Position to a new value again (invokes OnSet system) */
    ecs_set(world, e, Position, {30, 40});

    /* Cleanup: will invoke OnRemove system */
    return ecs_fini(world);
}
