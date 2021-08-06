#include <merge_world.h>

typedef struct {
    double x, y;
} Position, Velocity;

typedef struct {
    double value;
} Mass;

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Mass);

ecs_world_t* create_world(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Mass);

    return world;
}

int main(void) {
    ecs_world_t *src = create_world();
    ecs_world_t *dst = create_world();

    /* This example will demonstrate how to populate a world, and then copy data
     * from a source world to a destination world using the direct access API. 
     * This allows applications to asynchronously load data into a temporary
     * storage. With the data loaded in the right format, it can be cheaply
     * moved to a destination world. While the API uses low-level primitives and
     * as a result is somewhat verbose, it represents the fastest way of copying
     * data between worlds, and offers a high degree of flexibility. 
     *
     * It should be noted that this is a "safety off" API, as incorrectly
     * updating the columns of a table can corrupt the store. Applications are
     * encouraged to wrap the direct access API into higher level operations.
     *
     * On a high level, the application needs to make sure that after the 
     * operation, all destination table columns are of the same size, and that the
     * entity records in the destination world point to the new table. The
     * application needs to also ensure that the source world/table is left in a 
     * consistent state.
     */


    /* -- Step 1: Populate source world with entities */
    ecs_entity_t e1 = ecs_set(src, 0, Position, {10, 20});
    ecs_set(src, e1, Velocity, {1, 1});

    ecs_entity_t e2 = ecs_set(src, 0, Position, {20, 30});
    ecs_set(src, e2, Velocity, {2, 2});


    /* -- Step 2: Obtain the table from the source world */
    ecs_table_t *src_table = ecs_table_from_str(src, "Position, Velocity");
    int32_t src_pos_column = ecs_table_find_column(src_table, ecs_id(Position));
    int32_t src_vel_column = ecs_table_find_column(src_table, ecs_id(Velocity));


    /* -- Step 3: find or create the table in the destination world. 
     * Copy values to a table in the destination world. To demonstrate the
     * flexibility of the API, the data will be moved to a table that does not
     * have the same number of columns as the source table. */    
    ecs_table_t *dst_table = ecs_table_from_str(dst, "Position, Velocity, Mass");
    int32_t dst_pos_column = ecs_table_find_column(dst_table, ecs_id(Position));
    int32_t dst_vel_column = ecs_table_find_column(dst_table, ecs_id(Velocity));
    int32_t dst_mass_column = ecs_table_find_column(dst_table, ecs_id(Mass));
    

    /* -- Step 4: In addition to data columns, a table also has a column with
     * entity ids and cached pointers to records. We need to move the entity ids
     * to the destination table, as well as making sure that the source table 
     * records no longer point to the table */
    ecs_vector_t *entities = ecs_table_get_entities(src_table);
    ecs_vector_t *records = ecs_table_get_records(src_table);

    /* Clear src records so that they no longer point to the src table */
    ecs_records_clear(records);


    /* -- Step 5: Although the dst table is guaranteed to be empty, an
     * application should check if the table contains data already and free it
     * to prevent memory leaks */
    ecs_table_delete_column(dst, dst_table, dst_pos_column, NULL);
    ecs_table_delete_column(dst, dst_table, dst_vel_column, NULL);

    /* Also free entities & record vectors */
    ecs_vector_t *dst_entities = ecs_table_get_entities(dst_table);
    ecs_vector_t *dst_records = ecs_table_get_records(dst_table);
    if (dst_entities) ecs_vector_free(dst_entities);
    if (dst_records) ecs_vector_free(dst_records);


    /* -- Step 6: Assign entities & record vectors to destination world. Also
     * make sure that the records are updated to point to the dst table. */
    ecs_records_update(dst, entities, records, dst_table);
    ecs_table_set_entities(dst_table, entities, records);
    ecs_table_set_entities(src_table, NULL, NULL);


    /* -- Step 7: Set the columns from the source table to the dst table */
    ecs_vector_t *src_pos = ecs_table_get_column(src_table, src_pos_column);
    ecs_table_set_column(dst, dst_table, dst_pos_column, src_pos);
    ecs_table_set_column(src, src_table, src_pos_column, NULL);

    ecs_vector_t *src_vel = ecs_table_get_column(src_table, src_vel_column);
    ecs_table_set_column(dst, dst_table, dst_vel_column, src_vel);
    ecs_table_set_column(src, src_table, src_vel_column, NULL);


    /* -- Step 8: We need to ensure that the Mass column has the correct number
     * of elements. By providing NULL to the column parameter the function will
     * automatically create (or resize) a vector that is of the correct size. */
    ecs_table_set_column(dst, dst_table, dst_mass_column, NULL);


    /* Done! Query the destination table to verify entitities have been 
     * correctly transferred. */

    ecs_query_t *q = ecs_query_new(dst, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(q);
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        Velocity *v = ecs_term(&it, Velocity, 2);

        for (int i = 0; i < it.count; i ++) {
            printf("%u: Position{%f, %f}, Velocity{%f, %f}\n",
                (uint32_t)it.entities[i], p[i].x, p[i].y, v[i].x, v[i].y);
        }
    }

    /* Ensure that the entities have the correct type. This relies on the
     * records being correctly updated. */
    char *type_str = ecs_type_str(dst, ecs_get_type(dst, e1));
    printf("e1: [%s]\n", type_str); 
    ecs_os_free(type_str);
        
    type_str = ecs_type_str(dst, ecs_get_type(dst, e2));
    printf("e2: [%s]\n", type_str); 
    ecs_os_free(type_str);

    return ecs_fini(dst), ecs_fini(src);
}
