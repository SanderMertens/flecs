#include <direct_access.h>

typedef struct {
    double x, y;
} Position, Velocity;

int main(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, PV, Position, Velocity);

    /* Use the direct access API to set the Position and Velocity components.
     * When using regular ecs_set operations, each operation would have to
     * retrieve the underlying storage context for the entity, which can add
     * significant overhead, especially when initializing large numbers of
     * components and entities. 
     *
     * The direct access API enables an application to interact with the storage
     * context directly which gets rids of the redundant lookups. */

    /* Find the table for Position, Velocity components, and find the columns
     * for the Position and Velocity components. */
    ecs_table_t *table = ecs_table_from_type(world, ecs_type(PV));
    int32_t pos_column = ecs_table_find_column(table, ecs_id(Position));
    int32_t vel_column = ecs_table_find_column(table, ecs_id(Velocity));

    /* Create a new entity id */
    ecs_entity_t e1 = ecs_new_id(world);

    /* Find the record for the entity. A record stores in which table, and where
     * in that table an entity is stored. Since the entity does not yet exist,
     * this operation will create a record for the entity. */
    ecs_record_t *r1 = ecs_record_find(world, e1);

    /* Insert the entity into the table. This will update the record so that the
     * storage now knows where to find the entity. */
    ecs_table_insert(world, table, e1, r1);

    /* Initialize Position and Velocity values for the entity by looking up the
     * pointers for the entity in each respective column */
    Position *p_mut = ecs_record_get_column(r1, pos_column, sizeof(Position));
    p_mut->x = 10;
    p_mut->y = 20;

    Velocity *v_mut = ecs_record_get_column(r1, vel_column, sizeof(Velocity));
    v_mut->x = 1;
    v_mut->y = 1;

    /* Initialize a second entity for the same table. We can reuse the existing
     * table pointer and column indices to reduces overhead */
    ecs_entity_t e2 = ecs_new_id(world); 
    ecs_record_t *r2 = ecs_record_find(world, e2);
    ecs_table_insert(world, table, e2, r2);

    p_mut = ecs_record_get_column(r2, pos_column, sizeof(Position));
    p_mut->x = 20;
    p_mut->y = 30;

    v_mut = ecs_record_get_column(r2, vel_column, sizeof(Velocity));
    v_mut->x = 2;
    v_mut->y = 2;

    /* Print values to the console using regular functions */
    const Position *p = ecs_get(world, e1, Position);
    const Velocity *v = ecs_get(world, e1, Velocity);
    printf("e1: Position{%f, %f}, Velocity{%f, %f}\n", p->x, p->y, v->x, v->y);
    
    p = ecs_get(world, e2, Position);
    v = ecs_get(world, e2, Velocity);
    printf("e2: Position{%f, %f}, Velocity{%f, %f}\n", p->x, p->y, v->x, v->y);

    return ecs_fini(world);
}
