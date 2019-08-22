#include <hierarchy_api.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D WorldPosition;
typedef Vector2D Velocity;

/* Implement a simple move system */
void Move(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

        /* Print something to the console so we can see the system is being
         * invoked */
        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_id(rows->world, rows->entities[i]),
            p[i].x, p[i].y);
    }
}

/* Implement a system that transforms world coordinates hierarchically. If the 
 * CASCADE column is set, it points to the world coordinate of the parent. This
 * will be used to then transform Position to WorldPosition of the child.
 * If the CASCADE column is not set, the system matched a root. In that case,
 * just assign the Position to the WorldPosition. */
void Transform(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(rows, WorldPosition, parent_wp, 1);
    ECS_COLUMN(rows, WorldPosition, wp, 2);
    ECS_COLUMN(rows, Position, p, 3);

    if (!parent_wp) {
        for (int i = 0; i < rows->count; i ++) {
            wp[i].x = p[i].x;
            wp[i].y = p[i].y;

            /* Print something to the console so we can see the system is being
            * invoked */
            printf("%s transformed to {.x = %f, .y = %f} <<root>>\n",
                ecs_get_id(rows->world, rows->entities[i]),
                wp[i].x, wp[i].y);
        }
    } else {
        for (int i = 0; i < rows->count; i ++) {
            wp[i].x = parent_wp->x + p[i].x;
            wp[i].y = parent_wp->y + p[i].y;

            /* Print something to the console so we can see the system is being
            * invoked */
            printf("%s transformed to {.x = %f, .y = %f} <<child>>\n",
                ecs_get_id(rows->world, rows->entities[i]),
                wp[i].x, wp[i].y);
        }
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, WorldPosition);
    ECS_COMPONENT(world, Velocity);

    /* Move entities with Position and Velocity */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Transform local coordinates to world coordinates. A CASCADE column
     * guarantees that entities are evaluated breadth-first, according to the
     * hierarchy. This system will depth-sort based on parents that have the
     * WorldPosition component. */
    ECS_SYSTEM(world, Transform, EcsOnUpdate, CASCADE.WorldPosition, WorldPosition, Position);

    /* Create root of the hierachy which moves around */
    ecs_entity_t Root = ecs_new(world, 0);
    ecs_set(world, Root, EcsId, {"Root"});
    ecs_add(world, Root, WorldPosition);
    ecs_set(world, Root, Position, {0, 0});
    ecs_set(world, Root, Velocity, {1, 2});

        ecs_entity_t Child1 = ecs_new_child(world, Root, 0);
        ecs_set(world, Child1, EcsId, {"Child1"});
        ecs_add(world, Child1, WorldPosition);
        ecs_set(world, Child1, Position, {100, 100});

            ecs_entity_t GChild1 = ecs_new_child(world, Child1, 0);
            ecs_set(world, GChild1, EcsId, {"GChild1"});
            ecs_add(world, GChild1, WorldPosition);
            ecs_set(world, GChild1, Position, {1000, 1000});

        ecs_entity_t Child2 = ecs_new_child(world, Root, 0);
        ecs_set(world, Child2, EcsId, {"Child2"});
        ecs_add(world, Child2, WorldPosition);
        ecs_set(world, Child2, Position, {100, 100});

            ecs_entity_t GChild2 = ecs_new_child(world, Child1, 0);
            ecs_set(world, GChild2, EcsId, {"GChild1"});
            ecs_add(world, GChild2, WorldPosition);
            ecs_set(world, GChild2, Position, {1000, 1000});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0)) {
        printf("----\n");
    }

    /* Cleanup */
    return ecs_fini(world);
}