#include "flecs_private.h"

static
void ecs_parent_ctor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
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
    const ecs_entity_t *entities,
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
    const ecs_entity_t *dst_entities,
    const ecs_entity_t *src_entities,
    void *dst,
    const void *src,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *dst_array = dst;
    const EcsParent *src_array = src;

    ecs_parent_dtor(world, component, dst_entities, dst, size, count, ctx);    

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
    const ecs_entity_t *dst_entities,
    const ecs_entity_t *src_entities,
    void *dst,
    void *src,
    size_t size,
    int32_t count,
    void *ctx)
{
    EcsParent *dst_array = dst;
    EcsParent *src_array = src;

    ecs_parent_dtor(world, component, dst_entities, dst, size, count, ctx);

    int i;
    for (i = 0; i < count; i ++) {
        dst_array[i].child_tables = src_array[i].child_tables;
        src_array[i].child_tables = NULL;
    }
}

static
void ecs_component_set_intern(
    ecs_world_t *world,
    const ecs_entity_t *entities,
    EcsComponentTrigger *ct,
    int32_t count)
{
    ecs_component_data_t *cdata_array = ecs_vector_first(world->component_data);
    ecs_assert(cdata_array != NULL, ECS_INTERNAL_ERROR, NULL);
    EcsComponentTrigger *el = NULL;

    int i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_component_data_t *cdata = &cdata_array[e];
        switch(ct[i].kind) {
        case EcsOnAdd:
            el = ecs_vector_add(&cdata->on_add, EcsComponentTrigger);
            break;
        case EcsOnRemove:
            el = ecs_vector_add(&cdata->on_remove, EcsComponentTrigger);
            break;
        case EcsOnSet:
            el = ecs_vector_add(&cdata->on_set, EcsComponentTrigger);
            break;
        }
        
        ecs_assert(el != NULL, ECS_INTERNAL_ERROR, NULL);

        *el = ct[i];

        ecs_trace_1("trigger #[green]%s#[normal] created for component #[red]%s",
            ct[i].kind == EcsOnAdd
                ? "OnAdd"
                : ct[i].kind == EcsOnRemove
                    ? "OnRemove"
                    : "OnSet", ecs_get_name(world, e));
    }
}

static
void ecs_component_trigger_set(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsComponentTrigger, ct, 1);
    ecs_component_set_intern(rows->world, rows->entities, ct, rows->count);
}

static
void ecs_component_lifecycle_set(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsComponentLifecycle, cl, 1);

    ecs_world_t *world = rows->world;
    ecs_component_data_t *cdata_array = ecs_vector_first(world->component_data);
    ecs_assert(cdata_array != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        cdata_array[e].lifecycle = cl[i];

        ecs_trace_1("component #[green]%s#[normal] lifecycle callbacks set",
            ecs_get_name(world, e));        
    }
}

void ecs_init_builtins(
    ecs_world_t *world)
{
    /* Bootstrap the on_set trigger for EcsComponentTrigger. After this we'll be
     * able to set triggers for components */
    EcsComponentTrigger tr = {
        .kind = EcsOnSet,
        .callback = ecs_component_trigger_set
    };

    ecs_entity_t e = EEcsComponentTrigger;
    ecs_add(world, e, EcsComponentTrigger);
    ecs_component_set_intern(world, &e, &tr, 1);

    /* From here on we can use ecs_set to register component triggers */

    /* Register the OnSet trigger for EcsComponentLifecycle */
    ecs_set(world, EEcsComponentLifecycle, EcsComponentTrigger, {
        .kind = EcsOnSet,
        .callback = ecs_component_lifecycle_set
    });

    /* From here on we can use ecs_set to register component lifecycle */

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