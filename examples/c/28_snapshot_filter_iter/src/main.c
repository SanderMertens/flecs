#include <snapshot_filter_iter.h>

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

    /* Take a snapshot of the current state */
    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_filter_t f = {
        .include = ecs_type(Movable),
        .include_kind = EcsMatchAll
    };

    /* Delete matching entities from world to prove we're using snapshot data */
    ecs_bulk_delete(world, &f);

    /* Create iterator for snapshot */
    ecs_iter_t it = ecs_snapshot_iter(s, &f);

    /* Iterate it, progress entities in snapshot */
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

        /* If we want to display the name of the entities we can't use the
         * ecs_get_name function, since it tries to get the id from the world, and
         * we just deleted the entities from the world. Therefore, we should get
         * the name from the snapshot directly */
        int32_t id_index = ecs_type_index_of(table_type, ecs_entity(EcsName));

        /* Get pointers to the Position and Velocity columns with the obtained
         * column indices */
        Position *p = ecs_table_column(&it, p_index);
        Velocity *v = ecs_table_column(&it, v_index);
        EcsName *id = ecs_table_column(&it, id_index);

        /* Now we can iterate the component data as usual */
        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            printf("moved %s to {%f, %f}\n", id[i].value, p[i].x, p[i].y);
        }
    }
    
    /* Restore snapshot */
    ecs_snapshot_restore(world, s);

    /* Cleanup */
    return ecs_fini(world);
}
