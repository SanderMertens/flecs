#include <simple_triggers.h>

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

/* This system will be called when Position is removed */
void RemovePosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("Position removed -> {%f, %f}\n",
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register Position component */
    ECS_COMPONENT(world, Position);

    /* Register two systems that are executed when Position is added or removed
     * to entities. */
    ECS_TRIGGER(world, AddPosition, EcsOnAdd, Position);
    ECS_TRIGGER(world, RemovePosition, EcsOnRemove, Position);

    /* Create new entity with Position. This triggers the OnAdd system. */
    ecs_entity_t e = ecs_new(world, Position);

    /* Remove Position. This will trigger the OnRemove system. */
    ecs_remove(world, e, Position);

    /* Add Position again. This will retrigger the OnAdd system */
    ecs_add(world, e, Position);

    /* Add Position again. This does not retrigger the OnAdd system since the
     * entity already has Position */
    ecs_add(world, e, Position);

    /* Cleanup: will invoke OnRemove system */
    return ecs_fini(world);
}
