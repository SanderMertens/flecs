#include "flecs_private.h"

static
int compare_entity(
    ecs_entity_t e1, 
    void *ptr1, 
    ecs_entity_t e2, 
    void *ptr2) 
{
    return e1 - e2;
}

static
int rank_phase(
    ecs_world_t *world,
    ecs_entity_t rank_component,
    ecs_type_t type) 
{
    return ecs_type_get_entity_for_xor(world, type, rank_component);
}

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
    EcsTrigger *ct,
    int32_t count)
{
    ecs_component_data_t *cdata_array = ecs_vector_first(world->component_data);
    ecs_assert(cdata_array != NULL, ECS_INTERNAL_ERROR, NULL);
    EcsTrigger *el = NULL;

    int i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = ct[i].component;
        ecs_component_data_t *cdata = &cdata_array[e];
        switch(ct[i].kind) {
        case EcsOnAdd:
            el = ecs_vector_add(&cdata->on_add, EcsTrigger);
            break;
        case EcsOnRemove:
            el = ecs_vector_add(&cdata->on_remove, EcsTrigger);
            break;
        case EcsOnSet:
            el = ecs_vector_add(&cdata->on_set, EcsTrigger);
            break;
        }
        
        ecs_assert(el != NULL, ECS_INTERNAL_ERROR, NULL);

        *el = ct[i];
        el->self = entities[i];

        ecs_trace_1("trigger #[green]%s#[normal] created for component #[red]%s",
            ct[i].kind == EcsOnAdd
                ? "OnAdd"
                : ct[i].kind == EcsOnRemove
                    ? "OnRemove"
                    : "OnSet", ecs_get_name(world, e));
    }
}

static
void EcsOnSetTrigger(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsTrigger, ct, 1);
    ecs_component_set_intern(rows->world, rows->entities, ct, rows->count);
}

static
void EcsOnSetComponentLifecycle(
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

static 
void EcsOnAddPipeline(
    ecs_rows_t *rows)
{
    ecs_world_t *world = rows->world;
    ecs_entity_t *entities = rows->entities;

    int32_t i;
    for (i = rows->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        ecs_sig_t sig = { 0 };

#ifndef NDEBUG
        const EcsType *type_ptr = ecs_get_ptr(world, pipeline, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        char *str = ecs_type_str(world, type_ptr->normalized);
        ecs_trace_1("pipeline #[green]%s#[normal] created with #[red][%s]",
            ecs_get_name(world, pipeline), str);
        free(str);
#endif
        ecs_trace_push();

        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, EEcsColSystem, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ECS_XOR | pipeline, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EEcsInactive, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EEcsDisabledIntern, 0);

        ecs_query_t *query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_sort(world, query, 0, compare_entity);
        ecs_query_sort_types(world, query, pipeline, rank_phase);

        ecs_set(world, pipeline, EcsPipelineQuery, { .query = query });

        ecs_trace_pop();
    }
}

static 
void EcsOnSetSystem(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsSystem, sys, 1);

    ecs_world_t *world = rows->world;
    ecs_entity_t *entities = rows->entities;

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_init_system(world, entities[i], ecs_get_name(world, e), sys[i].phase, 
            sys[i].action, sys[i].signature);
    }
}

void ecs_init_system_builtins(
    ecs_world_t *world)
{
    ecs_set(world, EcsPreFrame, EcsName, {"EcsPreFrame"});
    ecs_set(world, EcsOnLoad, EcsName, {"EcsOnLoad"});
    ecs_set(world, EcsPostLoad, EcsName, {"EcsPostLoad"});
    ecs_set(world, EcsPreUpdate, EcsName, {"EcsPreUpdate"});
    ecs_set(world, EcsOnUpdate, EcsName, {"EcsOnUpdate"});
    ecs_set(world, EcsOnValidate, EcsName, {"EcsOnValidate"});
    ecs_set(world, EcsPostUpdate, EcsName, {"EcsPostUpdate"});
    ecs_set(world, EcsPreStore, EcsName, {"EcsPreStore"});
    ecs_set(world, EcsOnStore, EcsName, {"EcsOnStore"});
    ecs_set(world, EcsPostFrame, EcsName, {"EcsPostFrame"});

    ECS_TRIGGER(world, EcsOnAddPipeline, EcsOnAdd, EcsPipeline, 0);
    ECS_TRIGGER(world, EcsOnSetSystem, EcsOnSet, EcsSystem, 0);

    // ECS_WATCH(world, EcsSystemEnabled, EcsSystem, !EcsDisabled, !EcsDisabledIntern);
    // ECS_WATCH(world, EcsSystemDisabled, EcsSystem, EcsDisabled || EcsDisabledIntern);

    world->builtin_pipeline = ecs_new_pipeline(world, "EcsBuiltinPipeline",
        "EcsPreFrame, EcsOnLoad, EcsPostLoad, EcsPreUpdate, EcsOnUpdate,"
        " EcsOnValidate, EcsPostUpdate, EcsPreStore, EcsOnStore, EcsPostFrame");
}

void ecs_init_builtins(
    ecs_world_t *world)
{
    /* Bootstrap the on_set trigger for EcsTrigger. After this we'll be
     * able to set triggers for components */
    EcsTrigger tr = {
        .kind = EcsOnSet,
        .action = EcsOnSetTrigger,
        .component = EEcsTrigger
    };

    ecs_entity_t e = ecs_new(world, 0);
    ecs_set(world, e, EcsName, {"EcsSetTrigger"});
    ecs_add(world, e, EcsTrigger);
    ecs_component_set_intern(world, &e, &tr, 1);

    /* From here on we can use ecs_set to register component triggers */

    /* Register the OnSet trigger for EcsComponentLifecycle */
    ECS_TRIGGER(world, EcsOnSetComponentLifecycle, EcsOnSet, EcsComponentLifecycle, NULL);

    /* From here on we can use ecs_set to register component lifecycle */

    /* Set component callbacks for EcsParent */
    ecs_set(world, EEcsParent, EcsComponentLifecycle, {
        .ctor = ecs_parent_ctor,
        .dtor = ecs_parent_dtor,
        .copy = ecs_parent_copy,
        .move = ecs_parent_move
    });

    /* Initialize system builtins */
    ecs_init_system_builtins(world);     

    /* Initialize timer feature */
    ecs_init_timer_builtins(world);
}