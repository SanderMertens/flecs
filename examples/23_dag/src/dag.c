#include <include/dag.h>

/* This example demonstrates a technique to create a DAG (directed acyclic graph).
 * The graph will build a tree with players, platoons, regions and units:
 *
 * - A unit belongs to a platoon (a platoon can have 0-n units)
 * - A platoon is in a region (a region can have 0-n platoons)
 * - A platoon is controlled by a player (a player can control 0-n platoons)
 *
 * After the DAG is constructed, a method is demonstrated to create extremely
 * fast intersections of the collection of units, subdivided by platoon, region and
 * and player.
 *
 * Because of the way the DAG is constructed, units are organized in tables that
 * are subdivided by platoon, and platoons are organized in tables that are
 * subdivided by player and region. The only thing a system has to do in order
 * to determine whether a table has entities containing to a parent, is checking
 * whether the the parent is part of the table components.
 *
 * This is an O(n) check where n is the number of components. N is typically
 * a very low number, especially when compared to the number of entities in a
 * table.
 */

/* Units have a Position and Health */
typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef int32_t Health;

/* Add parents as components to child entities. This will split up entities
 * across different tables, which allows for making quick intersections based
 * on player, region or platoon */
static
void adopt(EcsWorld *world, EcsEntity object, EcsEntity parent) {
    ecs_stage_add(world, object, parent);
    ecs_commit(world, object);
}

/* Create entities with the EcsComponent component.  This allows entities to be
 * added to other entities just like normal components- with the exception that
 * they don't add data. */
static
EcsEntity create(EcsWorld *world, EcsEntity parent, const char *id, EcsEntity type) {
    EcsEntity result = ecs_new(world, type);
    ecs_stage_add(world, result, EcsComponent_h);
    if (parent)
        ecs_stage_add(world, result, parent);
    ecs_set(world, result, EcsId, id);
    ecs_commit(world, result);
    return result;
}

static
void ListUnits(EcsRows *rows) {
    EcsWorld *world = rows->world;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsEntity platoon = ecs_source(rows, 0);
        Position *p = ecs_data(rows, row, 1);
        Health *h = ecs_data(rows, row, 2);

        printf("Unit %llu, %s, position (%d,%d), health %d\n",
            entity, ecs_id(world, platoon), p->x, p->y, *h);
    }
}

static
void ListPlatoons(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity ListUnits_h = ecs_component(rows, 3);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsEntity player = ecs_source(rows, 0);
        EcsEntity region = ecs_source(rows, 1);

        printf("%s, %s, %s:\n",
            ecs_id(world, entity),
            ecs_id(world, region),
            ecs_id(world, player));

        ecs_run(world, ListUnits_h, rows->delta_time, entity, NULL);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Health);

    /* Register tags for players, platoons and regions */
    ECS_TAG(world, Player);
    ECS_TAG(world, Region);
    ECS_TAG(world, Platoon);

    /* Override all components from prefab in Unit family. The prefab is just
     * used here to provide default values for unit components. */
    ECS_PREFAB(world, UnitPrefab, Position, Health);
    ECS_FAMILY(world, Unit, UnitPrefab, Position, Health);
    ecs_set(world, UnitPrefab_h, Position, {0, 0});
    ecs_set(world, UnitPrefab_h, Health, 100);

    /* On-demand system that lists all units. In addition to Position and Health
     * which are the "normal" components found on a unit, units also have the
     * Platoon entity. With CONTAINER.Platoon, we select the "Platoon" component
     * from this platoon entity. Since "Platoon" is a TAG and not a CONTAINER
     * it does not contain any data. The reason we still add it to the signature
     * is so we get access to the platoon entity.
     *
     * We can get access to the platoon entity with ecs_source in the system
     * callback, which gives us the entity on which the Platoon component was
     * found, which in this case is the platoon to which the unit belongs. */
    ECS_SYSTEM(world, ListUnits, EcsOnDemand, CONTAINER.Platoon, Position, Health);

    /* On-demand system that lists all platoons. This system selects all
     * entities that have the "Platoon" tag. In addition, it references the
     * Player and Region TAGs in the same way ListUnit does for platoons. This
     * gives the system access to the player and region the platoon belongs to.
     *
     * Additionally, the ListUnits handle is passed to the system, so it can
     * invoke the ListUnits on-demand system to list the entities that match
     * with the platoon. */
    ECS_SYSTEM(world, ListPlatoons, EcsOnDemand, CONTAINER.Player, CONTAINER.Region, Platoon, ID.ListUnits);

    /* Create players, platoons and regions */
    EcsEntity player1 = create(world, 0, "Player 1", Player_h);
    EcsEntity player2 = create(world, 0, "Player 2", Player_h);

    EcsEntity region1 = create(world, 0, "Region 1", Region_h);
    EcsEntity region2 = create(world, 0, "Region 2", Region_h);

    EcsEntity platoon1 = create(world, player1, "Platoon 1", Platoon_h);
    EcsEntity platoon2 = create(world, player1, "Platoon 2", Platoon_h);
    EcsEntity platoon3 = create(world, player2, "Platoon 3", Platoon_h);

    /* Add platoons to regions, in addition to players */
    adopt(world, platoon1, region1);
    adopt(world, platoon2, region2);
    adopt(world, platoon3, region2);

    /* Create units in platoons */
    int i;
    for (i = 0; i < 5; i ++) create(world, platoon1, NULL, Unit_h);
    for (i = 0; i < 5; i ++) create(world, platoon2, NULL, Unit_h);
    for (i = 0; i < 5; i ++) create(world, platoon3, NULL, Unit_h);

    /* List all units */
    printf("-- All units\n");
    ecs_run(world, ListUnits_h, 1.0, 0, NULL);

    /* Only list units for platoon 2 */
    printf("\n-- Platoon 2 filter\n");
    ecs_run(world, ListUnits_h, 1.0, platoon2, NULL);

    /* Only list units/platoons for player 1 */
    printf("\n-- Player 1 filter\n");
    ecs_run(world, ListPlatoons_h, 1.0, player1, NULL);

    /* Only list units/platoons for region 2 */
    printf("\n-- Region 2 filter\n");
    ecs_run(world, ListPlatoons_h, 1.0, region2, NULL);

    /* Cleanup the world. */
    return ecs_fini(world);
}
