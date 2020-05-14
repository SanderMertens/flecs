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
        if (!ecs_has_entity(world, e, EEcsDisabled) && 
            !ecs_has_entity(world, e, EEcsDisabledIntern)) 
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

static 
void EcsDisableSystem(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsColSystem, system_data, 1);

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
    ECS_COLUMN(rows, EcsColSystem, system_data, 1);

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_enable_system(
            rows->world, rows->entities[i], &system_data[i], true);
    }
}

void ecs_init_system_builtins(
    ecs_world_t *world)
{
    /* -- System creation / enabling / disabling infrastructure -- */

    /* When EcsSystem is set, a new system will be created */
    ECS_TRIGGER(world, EcsOnSetSystem, EcsOnSet, EcsSystem, 0);

    /* Initialize EcsColSystem to zero when created */
    ecs_set(world, ecs_entity(EcsColSystem), EcsComponentLifecycle, {
        .ctor = ctor_init_zero,
        .dtor = ecs_colsystem_dtor
    });

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, EcsDisableSystem, 0, EcsColSystem, EcsDisabled || EcsDisabledIntern, SYSTEM.EcsMonitor);
    ECS_SYSTEM(world, EcsEnableSystem, 0, EcsColSystem, !EcsDisabled, !EcsDisabledIntern, SYSTEM.EcsMonitor);
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

    /* Initialize pipeline builtins */
    ecs_init_pipeline_builtins(world);

    /* Initialize system builtins */
    ecs_init_system_builtins(world);     

    /* Initialize timer feature */
    ecs_init_timer_builtins(world);
}