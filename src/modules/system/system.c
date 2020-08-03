#include "flecs.h"

#ifdef FLECS_SYSTEM

#include "../../private_api.h"
#include "system.h"

/* Global type variables */
ECS_TYPE_DECL(EcsComponentLifecycle);
ECS_TYPE_DECL(EcsTrigger);
ECS_TYPE_DECL(EcsSystem);
ECS_TYPE_DECL(EcsTickSource);
ECS_TYPE_DECL(EcsSignatureExpr);
ECS_TYPE_DECL(EcsSignature);
ECS_TYPE_DECL(EcsQuery);
ECS_TYPE_DECL(EcsIterAction);
ECS_TYPE_DECL(EcsContext);

static
ecs_on_demand_in_t* get_in_component(
    ecs_map_t *component_map,
    ecs_entity_t component)
{
    ecs_on_demand_in_t *in = ecs_map_get(
        component_map, ecs_on_demand_in_t, component);
    if (!in) {
        ecs_on_demand_in_t in_value = {0};
        ecs_map_set(component_map, component, &in_value);
        in = ecs_map_get(component_map, ecs_on_demand_in_t, component);
        ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return in;
}

static
void activate_in_columns(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_map_t *component_map,
    bool activate)
{
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsIn) {
            ecs_on_demand_in_t *in = get_in_component(
                component_map, columns[i].is.component);

            ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

            in->count += activate ? 1 : -1;

            ecs_assert(in->count >= 0, ECS_INTERNAL_ERROR, NULL);

            /* If this is the first system that registers the in component, walk
             * over all already registered systems to enable them */
            if (in->systems && 
               ((activate && in->count == 1) || 
                (!activate && !in->count))) 
            {
                ecs_on_demand_out_t **out = ecs_vector_first(in->systems, ecs_on_demand_out_t*);
                int32_t s, in_count = ecs_vector_count(in->systems);

                for (s = 0; s < in_count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        ecs_remove_entity(world, out[s]->system, EcsDisabledIntern);
                    } else if (!activate && !out[s]->count) {
                        ecs_add_entity(world, out[s]->system, EcsDisabledIntern);             
                    }
                }
            }
        }
    }    
}

static
void register_out_column(
    ecs_map_t *component_map,
    ecs_entity_t component,
    ecs_on_demand_out_t *on_demand_out)
{
    ecs_on_demand_in_t *in = get_in_component(component_map, component);
    ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

    on_demand_out->count += in->count;
    ecs_on_demand_out_t **elem = ecs_vector_add(&in->systems, ecs_on_demand_out_t*);
    *elem = on_demand_out;
}

static
void register_out_columns(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data)
{
    ecs_query_t *query = system_data->query;
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, out_count = 0, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsOut) {
            if (!system_data->on_demand) {
                system_data->on_demand = ecs_os_malloc(sizeof(ecs_on_demand_out_t));
                ecs_assert(system_data->on_demand != NULL, ECS_OUT_OF_MEMORY, NULL);

                system_data->on_demand->system = system;
                system_data->on_demand->count = 0;
            }

            /* If column operator is NOT and the inout kind is [out], the system
             * explicitly states that it will create the component (it is not
             * there, yet it is an out column). In this case it doesn't make
             * sense to wait until [in] columns get activated (matched with
             * entities) since the component is not there yet. Therefore add it
             * to the on_enable_components list, so this system will be enabled
             * when a [in] column is enabled, rather than activated */
            ecs_map_t *component_map;
            if (columns[i].oper_kind == EcsOperNot) {
                component_map = world->on_enable_components;
            } else {
                component_map = world->on_activate_components;
            }

            register_out_column(
                component_map, columns[i].is.component, 
                system_data->on_demand);

            out_count ++;
        }
    }

    /* If there are no out columns in the on-demand system, the system will
     * never be enabled */
    ecs_assert(out_count != 0, ECS_NO_OUT_COLUMNS, ecs_get_name(world, system));
}

static
void invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    const EcsSystem *system_data,
    ecs_system_status_t status)
{
    ecs_system_status_action_t action = system_data->status_action;
    if (action) {
        action(world, system, status, system_data->status_ctx);
    }
}

static
void mark_columns_dirty(
    ecs_iter_t *it,
    EcsSystem *system_data)
{
    ecs_table_t *table = it->table;
    if (table && table->dirty_state) {
        ecs_query_t *q = system_data->query;
        int32_t i, count = ecs_vector_count(q->sig.columns);
        ecs_sig_column_t *columns = ecs_vector_first(
            q->sig.columns, ecs_sig_column_t);

        for (i = 0; i < count; i ++) {
            if (columns[i].inout_kind != EcsIn) {
                int32_t table_column = it->columns[i];
                if (table_column > 0) {
                    table->dirty_state[table_column - 1] ++;
                }
            }
        }
    }
}

/* Invoked when system becomes active or inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    if (activate) {
        ecs_remove_entity(world, system, EcsInactive);
    }

    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data || !system_data->query) {
        return;
    }

    /* If system contains in columns, signal that they are now in use */
    activate_in_columns(
        world, system_data->query, world->on_activate_components, activate);

    /* Invoke system status action */
    invoke_status_action(world, system, system_data, 
        activate ? EcsSystemActivated : EcsSystemDeactivated);

    ecs_trace_2("system #[green]%s#[reset] %s", 
        ecs_get_name(world, system), 
        activate ? "activated" : "deactivated");
}

/* Actually enable or disable system */
void ecs_enable_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data,
    bool enabled)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    ecs_query_t *query = system_data->query;
    if (!query) {
        return;
    }

    if (ecs_vector_count(query->tables)) {
        /* Only (de)activate system if it has non-empty tables. */
        ecs_system_activate(world, system, enabled);
        system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    }

    /* Enable/disable systems that trigger on [in] enablement */
    activate_in_columns(
        world, 
        query, 
        world->on_enable_components, 
        enabled);
    
    /* Invoke action for enable/disable status */
    invoke_status_action(
        world, system, system_data,
        enabled ? EcsSystemEnabled : EcsSystemDisabled);
}

void ecs_init_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_iter_action_t action,
    ecs_query_t *query,
    void *ctx)
{
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    /* Add & initialize the EcsSystem component */
    bool is_added = false;
    EcsSystem *sptr = ecs_get_mut(world, system, EcsSystem, &is_added);
    ecs_assert(sptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!is_added) {
        ecs_assert(sptr->query == query, ECS_INVALID_PARAMETER, NULL);
    } else {
        memset(sptr, 0, sizeof(EcsSystem));
        sptr->query = query;
        sptr->entity = system;
        sptr->tick_source = 0;
        sptr->time_spent = 0;
        sptr->has_out_columns = false;
    }

    /* Sanity check to make sure creating the query didn't add any additional
     * tags or components to the system */
    sptr->action = action;
    sptr->ctx = ctx;

    /* Only run this code when the system is created for the first time */
    if (is_added) {
        /* If tables have been matched with this system it is active, and we
        * should activate the in-columns, if any. This will ensure that any
        * OnDemand systems get enabled. */
        if (ecs_vector_count(query->tables)) {
            ecs_system_activate(world, system, true);
        } else {
            /* If system isn't matched with any tables, mark it as inactive. This
            * causes it to be ignored by the main loop. When the system matches
            * with a table it will be activated. */
            ecs_add_entity(world, system, EcsInactive);
        }

        /* If system is enabled, trigger enable components */
        activate_in_columns(world, query, world->on_enable_components, true);

        /* Check if all non-table column constraints are met. If not, disable
        * system (system will be enabled once constraints are met) */
        if (!ecs_sig_check_constraints(world, &query->sig)) {
            ecs_add_entity(world, system, EcsDisabledIntern);
        }

        /* If the query has a OnDemand system tag, register its [out] columns */
        if (ecs_has_entity(world, system, EcsOnDemand)) {
            sptr = ecs_get_mut(world, system, EcsSystem, NULL);

            register_out_columns(world, system, sptr);
            ecs_assert(sptr->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

            /* If there are no systems currently interested in any of the [out]
            * columns of the on demand system, disable it */
            if (!sptr->on_demand->count) {
                ecs_add_entity(world, system, EcsDisabledIntern);
            }        
        }

        /* Check if system has out columns */
        int32_t i, count = ecs_vector_count(query->sig.columns);
        ecs_sig_column_t *columns = ecs_vector_first(
                query->sig.columns, ecs_sig_column_t);
        
        for (i = 0; i < count; i ++) {
            if (columns[i].inout_kind != EcsIn) {
                sptr->has_out_columns = true;
                break;
            }
        }
    }

    ecs_trace_1("system #[green]%s#[reset] created with #[red]%s", 
        ecs_get_name(world, system), query->sig.expr);
}


void ecs_col_system_free(
    EcsSystem *system_data)
{
    ecs_query_free(system_data->query);
}

/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    const EcsType *type_ptr = ecs_get( world, entity, EcsType);
    if (type_ptr) {
        /* If entity is a type, disable all entities in the type */
        ecs_vector_each(type_ptr->normalized, ecs_entity_t, e, {
            ecs_enable(world, *e, enabled);
        });
    } else {
        if (enabled) {
            ecs_remove_entity(world, entity, EcsDisabled);
        } else {
            ecs_add_entity(world, entity, EcsDisabled);
        }
    }
}

void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx)
{
    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->status_action = action;
    system_data->status_ctx = (void*)ctx;

    if (!ecs_has_entity(world, system, EcsDisabled)) {
        /* If system is already enabled, generate enable status. The API 
         * should guarantee that it exactly matches enable-disable 
         * notifications and activate-deactivate notifications. */
        invoke_status_action(world, system, system_data, EcsSystemEnabled);

        /* If column system has active (non-empty) tables, also generate the
         * activate status. */
        if (ecs_vector_count(system_data->query->tables)) {
            invoke_status_action(
                world, system, system_data, EcsSystemActivated);
        }
    }
}

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param,
    bool ran_by_app) 
{
    if (!param) {
        param = system_data->ctx;
    }

    float time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    if (tick_source) {
        const EcsTickSource *tick = ecs_get(
            world, tick_source, EcsTickSource);

        if (tick) {
            time_elapsed = tick->time_elapsed;

            /* If timer hasn't fired we shouldn't run the system */
            if (!tick->tick) {
                return 0;
            }
        } else {
            /* If a timer has been set but the timer entity does not have the
             * EcsTimer component, don't run the system. This can be the result
             * of a single-shot timer that has fired already. Not resetting the
             * timer field of the system will ensure that the system won't be
             * ran after the timer has fired. */
            return 0;
        }
    }

    ecs_time_t time_start;
    bool measure_time = world->measure_system_time;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    /* Used to check if table columns must be marked dirty */
    bool has_out_columns = system_data->has_out_columns;

    /* Prepare the query iterator */
    ecs_iter_t it = ecs_query_iter_page(system_data->query, offset, limit);
    it.world = stage->world;
    it.system = system;
    it.delta_time = delta_time;
    it.delta_system_time = time_elapsed;
    it.world_time = world->stats.world_time_total;
    it.frame_offset = offset;
    
    /* Set param if provided, otherwise use system context */
    if (param) {
        it.param = param;
    } else {
        it.param = system_data->ctx;
    }

    ecs_iter_action_t action = system_data->action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (!filter) {
        if (ran_by_app || world == stage->world) {
            while (ecs_query_next(&it)) {
                action(&it);
                if (has_out_columns) {
                    mark_columns_dirty(&it, system_data);
                }
            }
        } else {
            ecs_thread_t *thread = (ecs_thread_t*)stage->world;
            int32_t total = ecs_vector_count(world->workers);
            int32_t current = thread->index;

            while (ecs_query_next_worker(&it, current, total)) {
                action(&it);
                if (has_out_columns) {
                    mark_columns_dirty(&it, system_data);
                }                
            }
        }

    /* If filter is provided, match each table with the provided filter */
    } else {
        while (ecs_query_next(&it)) {
            ecs_table_t *table = it.table;
            if (!ecs_table_match_filter(world, table, filter)) {
                continue;
            }

            action(&it);
            if (has_out_columns) {
                mark_columns_dirty(&it, system_data);
            }            
        }        
    }

    if (measure_time) {
        system_data->time_spent += (float)ecs_time_measure(&time_start);
    }
    
    system_data->invoke_count ++;

    return it.interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool in_progress = ecs_staging_begin(world);

    EcsSystem *system_data = (EcsSystem*)ecs_get(
        world, system, EcsSystem);
    assert(system_data != NULL);

    ecs_entity_t interrupted_by = ecs_run_intern(
        world, stage, system, system_data, delta_time, offset, limit, 
        filter, param, true);

    /* If world wasn't in progress when we entered this function, we need to
     * merge and reset the in_progress value */
    ecs_staging_end(world, in_progress);

    return interrupted_by;
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, NULL, param);
}

void ecs_run_monitor(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_matched_query_t *monitor,
    ecs_entities_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{
    ecs_query_t *query = monitor->query;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t system = query->system;
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!system_data->action) {
        return;
    }

    ecs_iter_t it = {0};
    ecs_query_set_iter( world, stage, query, &it, 
        monitor->matched_table_index, row, count);

    it.world = stage->world;
    it.triggered_by = components;
    it.param = system_data->ctx;

    if (entities) {
        it.entities = entities;
    }

    it.system = system;
    system_data->action(&it);
}

/* Generic constructor to initialize a component to 0 */
static
void sys_ctor_init_zero(
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
    memset(ptr, 0, size * (size_t)count);
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
            invoke_status_action(world, e, ptr, EcsSystemDeactivated);
        }

        /* Invoke Disabled action for enabled systems */
        if (!ecs_has_entity(world, e, EcsDisabled) && 
            !ecs_has_entity(world, e, EcsDisabledIntern)) 
        {
            invoke_status_action(world, e, ptr, EcsSystemDisabled);
        }           

        ecs_os_free(cur->on_demand);
    }
}

/* Register a trigger for a component */
static
EcsTrigger* trigger_find_or_create(
    ecs_vector_t **triggers,
    ecs_entity_t entity)
{
    ecs_vector_each(*triggers, EcsTrigger, trigger, {
        if (trigger->self == entity) {
            return trigger;
        }
    });

    EcsTrigger *result = ecs_vector_add(triggers, EcsTrigger);
    return result;
}

static
void trigger_set(
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
            el = trigger_find_or_create(&cdata->on_add, entities[i]);
            break;
        case EcsOnRemove:
            el = trigger_find_or_create(&cdata->on_remove, entities[i]);
            break;
        default:
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
            break;
        }
        
        ecs_assert(el != NULL, ECS_INTERNAL_ERROR, NULL);

        *el = ct[i];
        el->self = entities[i];

        ecs_notify_tables_of_component_actions(world, e, cdata);

        ecs_trace_1("trigger #[green]%s#[normal] created for component #[red]%s",
            ct[i].kind == EcsOnAdd
                ? "OnAdd"
                : "OnRemove", ecs_get_name(world, e));
    }
}

static
void OnSetTrigger(
    ecs_iter_t *it)
{
    EcsTrigger *ct = ecs_column(it, EcsTrigger, 1);
    
    trigger_set(it->world, it->entities, ct, it->count);
}

static
void OnSetTriggerCtx(
    ecs_iter_t *it)
{
    EcsTrigger *ct = ecs_column(it, EcsTrigger, 1);
    EcsContext *ctx = ecs_column(it, EcsContext, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ct[i].ctx = (void*)ctx[i].ctx;
    }

    trigger_set(it->world, it->entities, ct, it->count);    
}

/* System that registers component lifecycle callbacks */
static
void OnSetComponentLifecycle(
    ecs_iter_t *it)
{
    EcsComponentLifecycle *cl = ecs_column(it, EcsComponentLifecycle, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_set_component_actions_w_entity(world, e, &cl[i]);   
    }
}

/* Disable system when EcsDisabled is added */
static 
void DisableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_column(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], false);
    }
}

/* Enable system when EcsDisabled is removed */
static
void EnableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_column(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], true);
    }
}

/* Parse a signature expression into the ecs_sig_t data structure */
static
void CreateSignature(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsSignatureExpr *signature = ecs_column(it, EcsSignatureExpr, 1);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
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
void CreateQuery(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsSignature *signature = ecs_column(it, EcsSignature, 1);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
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
void CreateSystem(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsQuery *query = ecs_column(it, EcsQuery, 1);
    EcsIterAction *action = ecs_column(it, EcsIterAction, 2);
    EcsContext *ctx = ecs_column(it, EcsContext, 3);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = entities[i];
        void *ctx_ptr = NULL;
        if (ctx) {
            ctx_ptr = (void*)ctx[i].ctx;
        }

        ecs_init_system(world, e, action[i].action, query[i].query, ctx_ptr);
    }
}

static
void bootstrap_set_system(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_iter_action_t action)
{
    ecs_sig_t sig = {0};
    ecs_entity_t sys = ecs_set(world, 0, EcsName, {.value = name});
    ecs_add_entity(world, sys, EcsOnSet);
    ecs_sig_init(world, name, expr, &sig);
    ecs_query_t *query = ecs_query_new_w_sig(world, sys, &sig);
    ecs_init_system(world, sys, action, query, NULL);
}

ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t tag,
    const char *signature,
    ecs_iter_action_t action)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    const char *e_name = ecs_name_from_symbol(world, name);
    
    ecs_entity_t result = ecs_lookup_w_type(world, name, ecs_type(EcsSignatureExpr));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        if (tag) {
            ecs_add_entity(world, result, tag);
        }

        ecs_set(world, result, EcsSignatureExpr, {signature});
        ecs_set(world, result, EcsIterAction, {action});
    } else {
        EcsSignatureExpr *ptr = ecs_get_mut(world, result, EcsSignatureExpr, NULL);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (strcmp(ptr->expr, signature)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t kind,
    const char *component_name,
    ecs_iter_action_t action)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t component = ecs_lookup_fullpath(world, component_name);
    ecs_assert(component != 0, ECS_INVALID_COMPONENT_ID, component_name);

    const char *e_name = ecs_name_from_symbol(world, name);
    
    ecs_entity_t result = ecs_lookup_w_type(world, name, ecs_type(EcsTrigger));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        ecs_set(world, result, EcsTrigger, {
            .kind = kind,
            .action = action,
            .component = component,
            .ctx = NULL
        });
    } else {
        EcsTrigger *ptr = ecs_get_mut(world, result, EcsTrigger, NULL);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->kind != kind) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->component != component) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->action != action) {
            ptr->action = action;
        }
    }

    return result;
}

void FlecsSystemImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_component(world, EcsComponentLifecycle);
    ecs_bootstrap_component(world, EcsTrigger);
    ecs_bootstrap_component(world, EcsSystem);
    ecs_bootstrap_component(world, EcsTickSource);
    ecs_bootstrap_component(world, EcsSignatureExpr);
    ecs_bootstrap_component(world, EcsSignature);
    ecs_bootstrap_component(world, EcsQuery);
    ecs_bootstrap_component(world, EcsIterAction);
    ecs_bootstrap_component(world, EcsContext);

    ecs_bootstrap_tag(world, EcsOnAdd);
    ecs_bootstrap_tag(world, EcsOnRemove);
    ecs_bootstrap_tag(world, EcsOnSet);
    ecs_bootstrap_tag(world, EcsUnSet);

    ecs_bootstrap_tag(world, EcsDisabledIntern);
    ecs_bootstrap_tag(world, EcsInactive);

    /* Put EcsOnDemand and EcsMonitor in flecs.core so they can be looked up
     * without using the flecs.systems prefix */
    ecs_entity_t old_scope = ecs_set_scope(world, EcsFlecsCore);
    ecs_bootstrap_tag(world, EcsOnDemand);
    ecs_bootstrap_tag(world, EcsMonitor);
    ecs_set_scope(world, old_scope);

    ECS_TYPE_IMPL(EcsComponentLifecycle);
    ECS_TYPE_IMPL(EcsTrigger);
    ECS_TYPE_IMPL(EcsSystem);
    ECS_TYPE_IMPL(EcsTickSource);
    ECS_TYPE_IMPL(EcsSignatureExpr);
    ECS_TYPE_IMPL(EcsSignature);
    ECS_TYPE_IMPL(EcsQuery);
    ECS_TYPE_IMPL(EcsIterAction);
    ECS_TYPE_IMPL(EcsContext);

    /* Bootstrap ctor and dtor for EcsSystem */
    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, ecs_entity(EcsSystem));
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);
    c_info->lifecycle.ctor = sys_ctor_init_zero;
    c_info->lifecycle.dtor = ecs_colsystem_dtor;

    /* Create systems necessary to create systems */
    bootstrap_set_system(world, "CreateSignature", "SignatureExpr", CreateSignature);
    bootstrap_set_system(world, "CreateQuery", "Signature, IterAction", CreateQuery);
    bootstrap_set_system(world, "CreateSystem", "Query, IterAction, ?Context", CreateSystem);

    /* From here we can create systems */

    /* Register OnSet system for EcsComponentLifecycle */
    ECS_SYSTEM(world, OnSetComponentLifecycle, EcsOnSet, ComponentLifecycle, SYSTEM:Hidden);

    /* Register OnSet system for triggers */
    ECS_SYSTEM(world, OnSetTrigger, EcsOnSet, Trigger, SYSTEM:Hidden);

    /* System that sets ctx for a trigger */
    ECS_SYSTEM(world, OnSetTriggerCtx, EcsOnSet, Trigger, Context, SYSTEM:Hidden);

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, DisableSystem, EcsMonitor, System, Disabled || DisabledIntern, SYSTEM:Hidden);
    ECS_SYSTEM(world, EnableSystem, EcsMonitor, System, !Disabled, !DisabledIntern, SYSTEM:Hidden);
}

#endif
