#include <include/systemmetadata.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

/* Display metadata that can be obtained from system callback */
void Metadata(EcsRows *rows) {
    void *row;

    printf("Running system '%s'\n", ecs_id(rows->world, rows->system));
    printf("delta_time = %f\n", rows->delta_time);

    int i;
    for (i = 0; i < rows->column_count; i ++) {
        EcsEntity component = ecs_component(rows, i);
        printf("column %d: %s\n", i, ecs_id(rows->world, component));
    }

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        printf("process entity %lld\n", entity);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components and type */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);
    ECS_TYPE(world, Object, Position, Speed);

    /* Register the Metadata system. */
    ECS_SYSTEM(world, Metadata, EcsOnFrame, Position, Speed);

    /* Create entity with Position and Speed */
    ecs_new(world, Object_h);

    /* Call ecs_progress */
    ecs_progress(world, 0.5);

    /* Cleanup the world. */
    return ecs_fini(world);
}
