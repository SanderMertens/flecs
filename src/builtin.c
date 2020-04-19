#include "flecs_private.h"

static
void EcsSetComponentLifecycle(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponentLifecycle, cl, 1);

    ecs_world_t *world = rows->world;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t component = rows->entities[i];
        ecs_assert(component < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
        ecs_component_data_t *cd = ecs_vector_first(world->component_data);
        cd[component].lifecycle = cl[i];
    }
}

static
void ecs_parent_ctor(
    ecs_world_t *world,
    ecs_entity_t component,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *array = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        array[i].child_tables = NULL;
    }
}

static
void ecs_parent_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *array = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        ecs_vector_free(array[i].child_tables);
    }
}

static
void ecs_parent_copy(
    ecs_world_t *world,
    ecs_entity_t component,
    void *dst,
    const void *src,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *dst_array = dst;
    const EcsParent *src_array = src;

    ecs_parent_dtor(world, component, dst, size, count, ctx);    

    int i;
    for (i = 0; i < count; i ++) {
        dst_array[i].child_tables = ecs_vector_copy(
            src_array[i].child_tables, ecs_table_t *);
    }
}

static
void ecs_parent_move(
    ecs_world_t *world,
    ecs_entity_t component,
    void *dst,
    void *src,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *dst_array = dst;
    EcsParent *src_array = src;

    ecs_parent_dtor(world, component, dst, size, count, ctx);

    int i;
    for (i = 0; i < count; i ++) {
        dst_array[i].child_tables = src_array[i].child_tables;
        src_array[i].child_tables = NULL;
    }
}

void ecs_init_builtins(
    ecs_world_t *world)
{
    /* Set component lifecycle callbacks */
    ecs_new_system(world, "EcsSetComponentLifecycle", EcsOnSet,
        "[in] EcsComponentLifecycle", EcsSetComponentLifecycle);

    /* Set component callbacks for EcsParent */
    ecs_set(world, EEcsParent, EcsComponentLifecycle, {
        .ctor = ecs_parent_ctor,
        .dtor = ecs_parent_dtor,
        .copy = ecs_parent_copy,
        .move = ecs_parent_move
    }); 

    /* Initialize timer feature */
    ecs_init_timer_builtins(world);
}