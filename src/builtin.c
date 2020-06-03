#include "flecs_private.h"

static
void ctor_init_zero(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entities;
    (void)ctx;
    memset(ptr, 0, size * count);
}

static
void ecs_colsystem_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)component;
    (void)ctx;
    (void)size;

    EcsColSystem *system_data = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        EcsColSystem *cur = &system_data[i];
        ecs_entity_t e = entities[i];

        /* Invoke Deactivated action for active systems */
        if (cur->query && ecs_vector_count(cur->query->tables)) {
            ecs_invoke_status_action(world, e, ptr, EcsSystemDeactivated);
        }

        /* Invoke Disabled action for enabled systems */
        if (!ecs_has_entity(world, e, EcsDisabled) && 
            !ecs_has_entity(world, e, EcsDisabledIntern)) 
        {
            ecs_invoke_status_action(world, e, ptr, EcsSystemDisabled);
        }           

        ecs_os_free(cur->on_demand);
        ecs_vector_free(cur->jobs);
    }
}


static
void ecs_component_set_intern(
    ecs_world_t *world,
    const ecs_entity_t *entities,
    EcsTrigger *ct,
    int32_t count)
{
    EcsTrigger *el = NULL;

    int i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = ct[i].component;

        ecs_c_info_t *cdata = ecs_get_or_create_c_info(world, e);
        switch(ct[i].kind) {
        case EcsOnAdd:
            el = ecs_vector_add(&cdata->on_add, EcsTrigger);
            break;
        case EcsOnRemove:
            el = ecs_vector_add(&cdata->on_remove, EcsTrigger);
            break;
        default:
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
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
    EcsTrigger *ct = ecs_column(rows, EcsTrigger, 1);
    
    ecs_component_set_intern(rows->world, rows->entities, ct, rows->count);
}

static
void EcsOnSetComponentLifecycle(
    ecs_rows_t *rows)
{
    EcsComponentLifecycle *cl = ecs_column(rows, EcsComponentLifecycle, 1);
    ecs_world_t *world = rows->world;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, e);

        c_info->lifecycle = cl[i];

        ecs_trace_1("component #[green]%s#[normal] lifecycle callbacks set",
            ecs_get_name(world, e));        
    }
}

static 
void EcsOnSetSystem(
    ecs_rows_t *rows)
{
    EcsSystem *sys = ecs_column(rows, EcsSystem, 1);

    ecs_world_t *world = rows->world;
    ecs_entity_t *entities = rows->entities;

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_init_system(world, entities[i], ecs_get_name(world, e), 
            sys[i].action, sys[i].signature);
    }
}

static 
void EcsDisableSystem(
    ecs_rows_t *rows)
{
    EcsColSystem *system_data = ecs_column(rows, EcsColSystem, 1);

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_enable_system(
            rows->world, rows->entities[i], &system_data[i], false);
    }
}

static
void EcsEnableSystem(
    ecs_rows_t *rows)
{
    EcsColSystem *system_data = ecs_column(rows, EcsColSystem, 1);

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_enable_system(
            rows->world, rows->entities[i], &system_data[i], true);
    }
}

void ecs_init_builtins(
    ecs_world_t *world)
{
    /* Bootstrap trigger tags */
    ecs_set(world, EcsOnAdd, EcsName, {"EcsOnAdd"});
    ecs_set(world, EcsOnRemove, EcsName, {"EcsOnRemove"});
    ecs_set(world, EcsOnSet, EcsName, {"EcsOnSet"});

    /* Bootstrap ctor and dtor for EcsColSystem */
    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, ecs_entity(EcsColSystem));
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);
    c_info->lifecycle.ctor = ctor_init_zero;
    c_info->lifecycle.dtor = ecs_colsystem_dtor;

    /* Bootstrap OnSet system for EcsSystem */
    ecs_entity_t set_system = ecs_set(world, 0, EcsName, {"EcsOnSetSystem"});
    ecs_add_entity(world, set_system, EcsOnSet);
    ecs_init_system(world, set_system, "EcsOnSetSystem", EcsOnSetSystem, "EcsSystem");

    /* From here we can create systems */

    /* Register OnSet system for EcsComponentLifecycle */
    ECS_SYSTEM(world, EcsOnSetComponentLifecycle, EcsOnSet, EcsComponentLifecycle);

    /* Register OnSet system for triggers */
    ECS_SYSTEM(world, EcsOnSetTrigger, EcsOnSet, EcsTrigger);

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, EcsDisableSystem, EcsMonitor, EcsColSystem, EcsDisabled || EcsDisabledIntern);
    ECS_SYSTEM(world, EcsEnableSystem, EcsMonitor, EcsColSystem, !EcsDisabled, !EcsDisabledIntern);

    /* Initialize pipeline builtins */
    ecs_init_pipeline_builtins(world);

    /* Initialize timer feature */
    ecs_init_timer_builtins(world);
}
