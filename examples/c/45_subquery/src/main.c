#include <subquery.h>

/* Subqueries allow applications to narrow down the potential set of tables to
 * match with to a small size, which makes creation and deletion of queries 
 * efficient enough to do in the main loop. Additionally, subqueries have a
 * lower adminstrative footprint than regular queries, which reduces the amount
 * of memory that is required to store them. */

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create three entities with different sets of components */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Velocity);

    /* Create a parent query that subscribes for all entities with Position */
    ecs_query_t *q_parent = ecs_query_new(world, "Position");

    /* Create a subquery that selects the subset of entities matched by q_parent
     * that have the Velocity component. */
    ecs_query_t *q_sub = ecs_subquery_new(world, q_parent, "Velocity");

    /* Iterate the subquery. Note that only e2 is matched, since it has both
     * Position and Velocity */
    ecs_iter_t it = ecs_query_iter(q_sub);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            printf("%s matched\n", ecs_get_name(world, it.entities[i]));
        }
    }

    /* The subquery could also have subscribed itself for both Position and
     * Velocity, in which case it would still only have matched against the
     * entities of the parent query, but the result would have been harder to
     * see. */

    /* Cleanup */
    return ecs_fini(world);
}
