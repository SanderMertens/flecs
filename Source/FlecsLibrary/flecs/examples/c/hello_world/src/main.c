#include <hello_world.h>
#include <stdio.h>

typedef struct {
    double x;
    double y;
} Position, Velocity;

/* Move system implementation. System callbacks may be called multiple times,
 * as entities are grouped in tables by which components they have, and each 
 * table has its own set of component arrays. */
void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    /* Print the set of components for the iterated over entities */
    char *type_str = ecs_table_str(it->world, it->table);
    printf("Move entities with [%s]\n", type_str);
    ecs_os_free(type_str);

    /* Iterate entities for the current table */
    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    /* Create the world */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Register system */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Register tags (components without a size) */
    ECS_TAG(world, Eats);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    /* Create an entity with name Bob, add Position and food preference */
    ecs_entity_t Bob = ecs_new_entity(world, "Bob");
    ecs_set(world, Bob, Position, {0, 0});
    ecs_set(world, Bob, Velocity, {1, 2});
    ecs_add_pair(world, Bob, Eats, Apples);

    /* Run systems twice. Usually this function is called once per frame */
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    /* See if Bob has moved (he has) */
    const Position *p = ecs_get(world, Bob, Position);
    printf("Bob's position is {%f, %f}\n", p->x, p->y);

    return ecs_fini(world);

    // Output
    //  Move entities with [Position, Velocity, (Identifier,Name), (Eats,Apples)]
    //  Move entities with [Position, Velocity, (Identifier,Name), (Eats,Apples)]
    //  Bob's position is {2.000000, 4.000000}
}
