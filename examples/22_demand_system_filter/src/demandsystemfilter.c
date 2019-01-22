#include <include/demandsystemfilter.h>

/* This function is invoked when the ecs_progress function is called. */
void ListEntities(EcsRows *rows) {
    void *row;

    /* This will loop over the 10 entities we created in the main function. */
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

        /* The ecs_source function obtains the entity from which the
         * column was resolved. Our column specifies IndexTag, which
         * has only been added to our index entities, thus ecs_source(0) will
         * return the index entity. */
        EcsEntity index = ecs_entity(rows, row, 0);
        printf("Entity %llu (%s)\n", entity, ecs_id(rows->world, index));
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register a tag so we can identify our index */
    ECS_TAG(world, IndexTag);
    ECS_TAG(world, Object);

    /* Add both the tag and EcsComponent to an Index family which we'll use to
     * create two entities that will subdivide our data. */
    ECS_FAMILY(world, Index, IndexTag, EcsContainer);

    /* List all the entities with a Position component on demand */
    ECS_SYSTEM(world, ListEntities, EcsOnDemand, CONTAINER.IndexTag, Object);

    /* Create 10 entities with Object family */
    EcsEntity handles[10];
    ecs_new_w_count(world, Object_h, 10, handles);

    /* Create two entities with EcsComponent to subdivide the entity list */
    EcsEntity index_a = ecs_new(world, Index_h);
    ecs_set(world, index_a, EcsId, {"Index A"});
    EcsEntity index_b = ecs_new(world, Index_h);
    ecs_set(world, index_b, EcsId, {"Index B"});

    /* Add 6 entities to index a, 4 entities to index b */
    int i;
    for (i = 0; i < 10; i ++) {
        if (i < 6) {
            ecs_add(world, handles[i], index_a);
        } else {
            ecs_add(world, handles[i], index_b);
        }
    }

    /* List all entities */
    printf("-- All entities:\n");
    ecs_run(
        world,           /* The world */
        ListEntities_h,  /* The system handle */
        1.0,             /* delta_time */
        NULL);           /* Userdata (can be accessed through rows->param) */

    printf("\n-- Only index a:\n");
    ecs_run_w_filter(
        world,           /* The world */
        ListEntities_h,  /* The system handle */
        1.0,             /* delta_time */
        0,               /* Offset */
        0,               /* Limit */
        index_a,         /* Component filter */
        NULL);           /* Userdata (can be accessed through rows->param) */

    printf("\n-- Only index b:\n");
    ecs_run_w_filter(
        world,           /* The world */
        ListEntities_h,  /* The system handle */
        1.0,             /* delta_time */
        0,               /* Offset */
        0,               /* Limit */
        index_b,         /* Component filter */
        NULL);           /* Userdata (can be accessed through rows->param) */

    /* Cleanup the world. */
    return ecs_fini(world);
}
