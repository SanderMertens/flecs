#include "flecs_private.h"

/* Global type variables */
ecs_type_t ecs_type(EcsComponent);
ecs_type_t ecs_type(EcsComponentLifecycle);
ecs_type_t ecs_type(EcsTrigger);
ecs_type_t ecs_type(EcsType);
ecs_type_t ecs_type(EcsModule);
ecs_type_t ecs_type(EcsPrefab);
ecs_type_t ecs_type(EcsSystem);
ecs_type_t ecs_type(EcsName);
ecs_type_t ecs_type(EcsHidden);
ecs_type_t ecs_type(EcsDisabled);
ecs_type_t ecs_type(EcsDisabledIntern);
ecs_type_t ecs_type(EcsInactive);
ecs_type_t ecs_type(EcsOnDemand);
ecs_type_t ecs_type(EcsMonitor);
ecs_type_t ecs_type(EcsPipeline);
ecs_type_t ecs_type(EcsPipelineQuery);
ecs_type_t ecs_type(EcsTimer);
ecs_type_t ecs_type(EcsRateFilter);
ecs_type_t ecs_type(EcsTickSource);
ecs_type_t ecs_type(EcsSignatureExpr);
ecs_type_t ecs_type(EcsSignature);
ecs_type_t ecs_type(EcsQuery);
ecs_type_t ecs_type(EcsViewAction);
ecs_type_t ecs_type(EcsContext);

/* Generic constructor to initialize a component to 0 */
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

/* System destructor */
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

    EcsSystem *system_data = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        EcsSystem *cur = &system_data[i];
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

/* Register a trigger for a component */
static
void ecs_trigger_set_intern(
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
    ecs_view_t *view)
{
    EcsTrigger *ct = ecs_column(view, EcsTrigger, 1);
    ecs_trigger_set_intern(view->world, view->entities, ct, view->count);
}

/* System that registers component lifecycle callbacks */
static
void EcsOnSetComponentLifecycle(
    ecs_view_t *view)
{
    EcsComponentLifecycle *cl = ecs_column(view, EcsComponentLifecycle, 1);
    ecs_world_t *world = view->world;

    int i;
    for (i = 0; i < view->count; i ++) {
        ecs_entity_t e = view->entities[i];
        ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, e);

        c_info->lifecycle = cl[i];

        ecs_trace_1("component #[green]%s#[normal] lifecycle callbacks set",
            ecs_get_name(world, e));        
    }
}

/* Disable system when EcsDisabled is added */
static 
void EcsDisableSystem(
    ecs_view_t *view)
{
    EcsSystem *system_data = ecs_column(view, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < view->count; i ++) {
        ecs_enable_system(
            view->world, view->entities[i], &system_data[i], false);
    }
}

/* Enable system when EcsDisabled is removed */
static
void EcsEnableSystem(
    ecs_view_t *view)
{
    EcsSystem *system_data = ecs_column(view, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < view->count; i ++) {
        ecs_enable_system(
            view->world, view->entities[i], &system_data[i], true);
    }
}

/* Parse a signature expression into the ecs_sig_t data structure */
static
void EcsCreateSignature(
    ecs_view_t *view) 
{
    ecs_world_t *world = view->world;
    ecs_entity_t *entities = view->entities;

    EcsSignatureExpr *signature = ecs_column(view, EcsSignatureExpr, 1);
    
    int32_t i;
    for (i = 0; i < view->count; i ++) {
        ecs_entity_t e = entities[i];
        const char *name = ecs_get_name(world, e);

        /* Parse the signature and add the result to the entity */
        EcsSignature sig = {0};
        ecs_sig_init(world, name, signature[0].expr, &sig.signature);
        ecs_set_ptr(world, e, EcsSignature, &sig);

        /* If sig has FromSystem columns, add components to the entity */
        ecs_vector_each(sig.signature.columns, ecs_sig_column_t, column, {
            if (column->from_kind == EcsFromSystem) {
                ecs_add_entity(world, e, column->is.component);
            }
        });    
    }
}

/* Create a query from a signature */
static
void EcsCreateQuery(
    ecs_view_t *view) 
{
    ecs_world_t *world = view->world;
    ecs_entity_t *entities = view->entities;

    EcsSignature *signature = ecs_column(view, EcsSignature, 1);
    
    int32_t i;
    for (i = 0; i < view->count; i ++) {
        ecs_entity_t e = entities[i];

        if (!ecs_has(world, e, EcsQuery)) {
            EcsQuery query = {0};
            query.query = ecs_query_new_w_sig(world, e, &signature[i].signature);
            ecs_set_ptr(world, e, EcsQuery, &query);
        }
    }
}

/* Create a system from a query and an action */
static
void EcsCreateSystem(
    ecs_view_t *view)
{
    ecs_world_t *world = view->world;
    ecs_entity_t *entities = view->entities;

    EcsQuery *query = ecs_column(view, EcsQuery, 1);
    EcsViewAction *action = ecs_column(view, EcsViewAction, 2);
    EcsContext *ctx = ecs_column(view, EcsContext, 3);
    
    int32_t i;
    for (i = 0; i < view->count; i ++) {
        ecs_entity_t e = entities[i];
        void *ctx_ptr = NULL;
        if (ctx) {
            ctx_ptr = ctx[i].ctx;
        }

        ecs_init_system(world, e, action[i].action, query[i].query, ctx_ptr);
    }
}

static
void bootstrap_set_system(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_view_action_t action)
{
    ecs_sig_t sig = {0};
    ecs_entity_t sys = ecs_set(world, 0, EcsName, {name});
    ecs_add_entity(world, sys, EcsOnSet);
    ecs_sig_init(world, name, expr, &sig);
    ecs_query_t *query = ecs_query_new_w_sig(world, sys, &sig);
    ecs_init_system(world, sys, action, query, NULL);
}

/* Initialize builtin systems */
void ecs_init_builtins(
    ecs_world_t *world)
{
    /* Bootstrap trigger tags */
    ecs_set(world, EcsOnAdd, EcsName, {"EcsOnAdd"});
    ecs_set(world, EcsOnRemove, EcsName, {"EcsOnRemove"});
    ecs_set(world, EcsOnSet, EcsName, {"EcsOnSet"});

    /* Bootstrap ctor and dtor for EcsSystem */
    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, ecs_entity(EcsSystem));
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);
    c_info->lifecycle.ctor = ctor_init_zero;
    c_info->lifecycle.dtor = ecs_colsystem_dtor;

    /* Create systems necessary to create systems */
    bootstrap_set_system(world, "EcsCreateSignature", "EcsSignatureExpr", EcsCreateSignature);
    bootstrap_set_system(world, "EcsCreateQuery", "EcsSignature, EcsViewAction", EcsCreateQuery);
    bootstrap_set_system(world, "EcsCreateSystem", "EcsQuery, EcsViewAction, ?EcsContext", EcsCreateSystem);

    /* From here we can create systems */

    /* Register OnSet system for EcsComponentLifecycle */
    ECS_SYSTEM(world, EcsOnSetComponentLifecycle, EcsOnSet, EcsComponentLifecycle);

    /* Register OnSet system for triggers */
    ECS_SYSTEM(world, EcsOnSetTrigger, EcsOnSet, EcsTrigger);

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, EcsDisableSystem, EcsMonitor, EcsSystem, EcsDisabled || EcsDisabledIntern);
    ECS_SYSTEM(world, EcsEnableSystem, EcsMonitor, EcsSystem, !EcsDisabled, !EcsDisabledIntern);

    /* Initialize pipeline builtins */
    ecs_init_pipeline_builtins(world);

    /* Initialize timer feature */
    ecs_init_timer_builtins(world);
}


/* -- Bootstrapping -- */

/** Create type for component */
static
ecs_type_t bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_table_find_or_create(world, &world->stage, 
        &(ecs_entities_t){
            .array = (ecs_entity_t[]){entity},
            .count = 1
        }
    );

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Bootstrap types for builtin components and tags */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = bootstrap_type(world, ecs_entity(EcsComponent));
    ecs_type(EcsComponentLifecycle) = bootstrap_type(world, ecs_entity(EcsComponentLifecycle));
    ecs_type(EcsTrigger) = bootstrap_type(world, ecs_entity(EcsTrigger));
    ecs_type(EcsType) = bootstrap_type(world, ecs_entity(EcsType));
    ecs_type(EcsModule) = bootstrap_type(world, EcsModule);
    ecs_type(EcsPrefab) = bootstrap_type(world, EcsPrefab);
    ecs_type(EcsSystem) = bootstrap_type(world, ecs_entity(EcsSystem));
    ecs_type(EcsName) = bootstrap_type(world, ecs_entity(EcsName));
    ecs_type(EcsHidden) = bootstrap_type(world, EcsHidden);
    ecs_type(EcsDisabled) = bootstrap_type(world, EcsDisabled);
    ecs_type(EcsDisabledIntern) = bootstrap_type(world, EcsDisabledIntern);
    ecs_type(EcsInactive) = bootstrap_type(world, EcsInactive);
    ecs_type(EcsOnDemand) = bootstrap_type(world, EcsOnDemand);
    ecs_type(EcsMonitor) = bootstrap_type(world, EcsMonitor);
    ecs_type(EcsPipeline) = bootstrap_type(world, EcsPipeline);
    ecs_type(EcsPipelineQuery) = bootstrap_type(world, ecs_entity(EcsPipelineQuery));
    ecs_type(EcsTimer) = bootstrap_type(world, ecs_entity(EcsTimer));
    ecs_type(EcsRateFilter) = bootstrap_type(world, ecs_entity(EcsRateFilter));
    ecs_type(EcsTickSource) = bootstrap_type(world, ecs_entity(EcsTickSource));
    ecs_type(EcsSignatureExpr) = bootstrap_type(world, ecs_entity(EcsSignatureExpr));
    ecs_type(EcsSignature) = bootstrap_type(world, ecs_entity(EcsSignature));
    ecs_type(EcsQuery) = bootstrap_type(world, ecs_entity(EcsQuery));
    ecs_type(EcsViewAction) = bootstrap_type(world, ecs_entity(EcsViewAction));
    ecs_type(EcsContext) = bootstrap_type(world, ecs_entity(EcsContext));   
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. 
 * The reason this table is constructed manually is because it requires the size
 * and alignment of the EcsComponent and EcsName components, which haven't been 
 * created yet */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {ecs_entity(EcsComponent), ecs_entity(EcsName)};
    ecs_entities_t array = {
        .array = entities,
        .count = 2
    };

    ecs_table_t *result = ecs_table_find_or_create(
        world, &world->stage, &array);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, result);

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsLastBuiltin);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsLastBuiltin);

    data->columns = ecs_os_malloc(sizeof(ecs_column_t) * 2);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsLastBuiltin);
    data->columns[0].size = sizeof(EcsComponent);
    data->columns[0].alignment = ECS_ALIGNOF(EcsComponent);
    data->columns[1].data = ecs_vector_new(EcsName, EcsLastBuiltin);
    data->columns[1].size = sizeof(EcsName);
    data->columns[1].alignment = ECS_ALIGNOF(EcsName);

    result->column_count = 2;
    
    return result;
}

/** Bootstrap builtin components and tags */
#define bootstrap_component(world, table, name)\
    _bootstrap_component(world, table, ecs_entity(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

#define bootstrap_tag(world, name)\
    ecs_set(world, name, EcsName, {#name})

static
void _bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *id,
    size_t size,
    size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Create record in entity index */
    ecs_record_t *record = ecs_eis_get_or_create(&world->stage, entity);
    record->table = table;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_append(world, table, data, entity, record);
    record->row = index + 1;

    /* Set size and id */
    EcsComponent *c_info = ecs_vector_first(columns[0].data, EcsComponent);
    EcsName *id_data = ecs_vector_first(columns[1].data, EcsName);
    
    c_info[index].size = size;
    c_info[index].alignment = alignment;
    id_data[index] = id;
}

void ecs_bootstrap(
    ecs_world_t *world)
{
    /* Create table that will hold components (EcsComponent, EcsName) */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    ecs_trace_1("initialize builtin components");
    ecs_trace_push();

    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsComponentLifecycle);
    bootstrap_component(world, table, EcsTrigger);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsSystem);
    bootstrap_component(world, table, EcsName);
    bootstrap_component(world, table, EcsPipelineQuery);
    bootstrap_component(world, table, EcsTimer);
    bootstrap_component(world, table, EcsRateFilter);
    bootstrap_component(world, table, EcsTickSource);
    bootstrap_component(world, table, EcsSignatureExpr);
    bootstrap_component(world, table, EcsSignature);
    bootstrap_component(world, table, EcsQuery);
    bootstrap_component(world, table, EcsViewAction);
    bootstrap_component(world, table, EcsContext);

    world->stats.last_component_id = EcsLastBuiltin;
    world->stats.last_id = EcsWorld;
    world->stats.min_id = 0;
    world->stats.max_id = 0;

    bootstrap_types(world);

    bootstrap_tag(world, EcsModule);
    bootstrap_tag(world, EcsPipeline);
    bootstrap_tag(world, EcsPrefab);
    bootstrap_tag(world, EcsHidden);
    bootstrap_tag(world, EcsDisabled);
    bootstrap_tag(world, EcsDisabledIntern);
    bootstrap_tag(world, EcsInactive);
    bootstrap_tag(world, EcsOnDemand);
    bootstrap_tag(world, EcsMonitor);

    ecs_trace_pop();

    /* Initialize EcsWorld */
    ecs_set(world, EcsWorld, EcsName, {"EcsWorld"});
    ecs_assert(ecs_get_name(world, EcsWorld) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "EcsWorld") == EcsWorld, ECS_INTERNAL_ERROR, NULL);

    /* Initialize EcsSingleton */
    ecs_set(world, EcsSingleton, EcsName, {"$"});
    ecs_assert(ecs_get_name(world, EcsSingleton) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "$") == EcsSingleton, ECS_INTERNAL_ERROR, NULL);

    ecs_trace_1("initialize builtins");
    ecs_trace_push();
    ecs_init_builtins(world);
    ecs_trace_pop();
}
