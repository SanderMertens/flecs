#include <filter_iter.h>

typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

typedef float Mass;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e1 = 
    ecs_set(world, 0, EcsName, {"e1"});
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {1, 1});

    ecs_entity_t e2 = 
    ecs_set(world, 0, EcsName, {"e2"});
    ecs_set(world, e2, Position, {10, 20});
    ecs_set(world, e2, Velocity, {1, 1});

    /* Create a filter for the Movable type, which includes Position and 
    * Velocity. Make sure to only match tables that include both Position and
    * Velocity. */
    ecs_iter_t it = ecs_filter_iter(world, &(ecs_filter_t){
        .include = ecs_type(Movable),
        .include_kind = EcsMatchAll
    });

    while (ecs_filter_next(&it)) {
        /* Even though we have a it ptr, we can't use it as we normally would
         * in a system with ecs_column. This is because a filter has no well
         * defined indices for the components being matched with. To obtain the
         * component data when using a filter, we have to retrieve the component
         * arrays directly from the table. */
        
        /* Get type of table we're currently iterating over. The order in which
         * components appear in the type is the same as the order in which the
         * components are stored on the table. */
        ecs_type_t table_type = ecs_iter_type(&it);

        /* Retrieve the column indices for both the Position and Velocity
         * columns by finding their position in the table type */
        int32_t p_index = ecs_type_index_of(table_type, ecs_entity(Position));
        int32_t v_index = ecs_type_index_of(table_type, ecs_entity(Velocity));

        /* Get pointers to the Position and Velocity columns with the obtained
         * column indices */
        Position *p = ecs_table_column(&it, p_index);
        Velocity *v = ecs_table_column(&it, v_index);

        /* Now we can iterate the component data as usual */
        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;

            printf("moved %s to {%f, %f}\n", 
                ecs_get_name(world, it.entities[i]), p[i].x, p[i].y);
        }
    }

    /* Cleanup */
    return ecs_fini(world);
}
