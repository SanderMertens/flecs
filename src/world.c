#include "private_api.h"

/* Roles */
const ecs_id_t ECS_CASE =  (ECS_ROLE | (0x7Cull << 56));
const ecs_id_t ECS_SWITCH =  (ECS_ROLE | (0x7Bull << 56));
const ecs_id_t ECS_PAIR =  (ECS_ROLE | (0x7Aull << 56));
const ecs_id_t ECS_OWNED =  (ECS_ROLE | (0x75ull << 56));
const ecs_id_t ECS_DISABLED =  (ECS_ROLE | (0x74ull << 56));

/* Core scopes & entities */
const ecs_entity_t EcsWorld =                 ECS_HI_COMPONENT_ID + 0;
const ecs_entity_t EcsFlecs =                 ECS_HI_COMPONENT_ID + 1;
const ecs_entity_t EcsFlecsCore =             ECS_HI_COMPONENT_ID + 2;
const ecs_entity_t EcsModule =                ECS_HI_COMPONENT_ID + 3;
const ecs_entity_t EcsPrefab =                ECS_HI_COMPONENT_ID + 4;
const ecs_entity_t EcsDisabled =              ECS_HI_COMPONENT_ID + 5;
const ecs_entity_t EcsHidden =                ECS_HI_COMPONENT_ID + 6;

/* Relation properties */
const ecs_entity_t EcsWildcard =              ECS_HI_COMPONENT_ID + 10;
const ecs_entity_t EcsThis =                  ECS_HI_COMPONENT_ID + 11;
const ecs_entity_t EcsTransitive =            ECS_HI_COMPONENT_ID + 12;
const ecs_entity_t EcsFinal =                 ECS_HI_COMPONENT_ID + 13;
const ecs_entity_t EcsTag =                   ECS_HI_COMPONENT_ID + 14;

/* Identifier tags */
const ecs_entity_t EcsName =                  ECS_HI_COMPONENT_ID + 15;
const ecs_entity_t EcsSymbol =                ECS_HI_COMPONENT_ID + 16;

/* Relations */
const ecs_entity_t EcsChildOf =               ECS_HI_COMPONENT_ID + 20;
const ecs_entity_t EcsIsA =                   ECS_HI_COMPONENT_ID + 21;

/* Events */
const ecs_entity_t EcsOnAdd =                 ECS_HI_COMPONENT_ID + 30;
const ecs_entity_t EcsOnRemove =              ECS_HI_COMPONENT_ID + 31;
const ecs_entity_t EcsOnSet =                 ECS_HI_COMPONENT_ID + 32;
const ecs_entity_t EcsUnSet =                 ECS_HI_COMPONENT_ID + 33;
const ecs_entity_t EcsOnDelete =              ECS_HI_COMPONENT_ID + 34;
const ecs_entity_t EcsOnCreateTable =         ECS_HI_COMPONENT_ID + 35;
const ecs_entity_t EcsOnDeleteTable =         ECS_HI_COMPONENT_ID + 36;
const ecs_entity_t EcsOnTableEmpty =          ECS_HI_COMPONENT_ID + 37;
const ecs_entity_t EcsOnTableNonEmpty =       ECS_HI_COMPONENT_ID + 38;
const ecs_entity_t EcsOnCreateTrigger =       ECS_HI_COMPONENT_ID + 39;
const ecs_entity_t EcsOnDeleteTrigger =       ECS_HI_COMPONENT_ID + 40;
const ecs_entity_t EcsOnDeleteObservable =    ECS_HI_COMPONENT_ID + 41;
const ecs_entity_t EcsOnComponentLifecycle =  ECS_HI_COMPONENT_ID + 42;
const ecs_entity_t EcsOnDeleteObject =        ECS_HI_COMPONENT_ID + 43;

/* Actions */
const ecs_entity_t EcsRemove =                ECS_HI_COMPONENT_ID + 50;
const ecs_entity_t EcsDelete =                ECS_HI_COMPONENT_ID + 51;
const ecs_entity_t EcsThrow =                 ECS_HI_COMPONENT_ID + 52;

/* Systems */
const ecs_entity_t EcsOnDemand =              ECS_HI_COMPONENT_ID + 60;
const ecs_entity_t EcsMonitor =               ECS_HI_COMPONENT_ID + 61;
const ecs_entity_t EcsDisabledIntern =        ECS_HI_COMPONENT_ID + 62;
const ecs_entity_t EcsInactive =              ECS_HI_COMPONENT_ID + 63;
const ecs_entity_t EcsPipeline =              ECS_HI_COMPONENT_ID + 64;
const ecs_entity_t EcsPreFrame =              ECS_HI_COMPONENT_ID + 65;
const ecs_entity_t EcsOnLoad =                ECS_HI_COMPONENT_ID + 66;
const ecs_entity_t EcsPostLoad =              ECS_HI_COMPONENT_ID + 67;
const ecs_entity_t EcsPreUpdate =             ECS_HI_COMPONENT_ID + 68;
const ecs_entity_t EcsOnUpdate =              ECS_HI_COMPONENT_ID + 69;
const ecs_entity_t EcsOnValidate =            ECS_HI_COMPONENT_ID + 70;
const ecs_entity_t EcsPostUpdate =            ECS_HI_COMPONENT_ID + 71;
const ecs_entity_t EcsPreStore =              ECS_HI_COMPONENT_ID + 72;
const ecs_entity_t EcsOnStore =               ECS_HI_COMPONENT_ID + 73;
const ecs_entity_t EcsPostFrame =             ECS_HI_COMPONENT_ID + 74;


/* -- Private functions -- */

const ecs_world_t* ecs_get_world(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        return world;
    } else {
        return ((ecs_stage_t*)world)->world;
    }
}

const ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_STAGE_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        return &world->stage;

    } else if (world->magic == ECS_STAGE_MAGIC) {
        return (ecs_stage_t*)world;
    }
    
    return NULL;    
}

ecs_stage_t *flecs_stage_from_world(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_STAGE_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
        return &world->stage;

    } else if (world->magic == ECS_STAGE_MAGIC) {
        ecs_stage_t *stage = (ecs_stage_t*)world;
        *world_ptr = stage->world;
        return stage;
    }
    
    return NULL;
}

/* Evaluate component monitor. If a monitored entity changed it will have set a
 * flag in one of the world's component monitors. Queries can register 
 * themselves with component monitors to determine whether they need to rematch
 * with tables. */
static
void eval_component_monitor(
    ecs_world_t *world)
{
    ecs_relation_monitor_t *rm = &world->monitors;

    if (!rm->is_dirty) {
        return;
    }

    ecs_map_iter_t it = ecs_map_iter(rm->monitor_sets);
    ecs_monitor_set_t *ms;

    while ((ms = ecs_map_next(&it, ecs_monitor_set_t, NULL))) {
        if (!ms->is_dirty) {
            continue;
        }

        if (ms->monitors) {
            ecs_map_iter_t mit = ecs_map_iter(ms->monitors);
            ecs_monitor_t *m;
            while ((m = ecs_map_next(&mit, ecs_monitor_t, NULL))) {
                if (!m->is_dirty) {
                    continue;
                }

                ecs_vector_each(m->queries, ecs_query_t*, q_ptr, {
                    flecs_query_notify(world, *q_ptr, &(ecs_query_event_t) {
                        .kind = EcsQueryTableRematch
                    });
                });

                m->is_dirty = false;
            }
        }

        ms->is_dirty = false;
    }

    rm->is_dirty = false;
}

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id)
{
    ecs_assert(world->monitors.monitor_sets != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    ecs_monitor_set_t *ms = ecs_map_get(world->monitors.monitor_sets, 
        ecs_monitor_set_t, relation);
    if (ms && ms->monitors) {
        ecs_monitor_t *m = ecs_map_get(ms->monitors, 
            ecs_monitor_t, id);
        if (m) {
            m->is_dirty = true;
            ms->is_dirty = true;
            world->monitors.is_dirty = true;
        }
    }
}

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->monitors.monitor_sets != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_monitor_set_t *ms = ecs_map_ensure(
        world->monitors.monitor_sets, ecs_monitor_set_t, relation);
    ecs_assert(ms != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!ms->monitors) {
        ms->monitors = ecs_map_new(ecs_monitor_t, 1);
    }

    ecs_monitor_t *m = ecs_map_ensure(ms->monitors, ecs_monitor_t, id);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);        

    ecs_query_t **q = ecs_vector_add(&m->queries, ecs_query_t*);
    *q = query;
}

static
void monitors_init(
    ecs_relation_monitor_t *rm)
{
    rm->monitor_sets = ecs_map_new(ecs_monitor_t, 0);
    rm->is_dirty = false;
}

static
void monitors_fini(
    ecs_relation_monitor_t *rm)
{
    ecs_map_iter_t it = ecs_map_iter(rm->monitor_sets);
    ecs_monitor_set_t *ms;

    while ((ms = ecs_map_next(&it, ecs_monitor_set_t, NULL))) {
        if (ms->monitors) {
            ecs_map_iter_t mit = ecs_map_iter(ms->monitors);
            ecs_monitor_t *m;
            while ((m = ecs_map_next(&mit, ecs_monitor_t, NULL))) {
                ecs_vector_free(m->queries);
            }

            ecs_map_free(ms->monitors);
        }
    }

    ecs_map_free(rm->monitor_sets);
}

static
void init_store(
    ecs_world_t *world) 
{
    ecs_os_memset(&world->store, 0, ECS_SIZEOF(ecs_store_t));
    
    /* Initialize entity index */
    world->store.entity_index = flecs_sparse_new(ecs_record_t);
    flecs_sparse_set_id_source(world->store.entity_index, &world->stats.last_id);

    /* Initialize root table */
    world->store.tables = flecs_sparse_new(ecs_table_t);

    /* Initialize table map */
    world->store.table_map = flecs_table_hashmap_new();

    /* Initialize one root table per stage */
    flecs_init_root_table(world);
}

static
void clean_tables(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);
        flecs_table_free(world, t);
    }

    /* Free table types separately so that if application destructors rely on
     * a type it's still valid. */
    for (i = 0; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);
        flecs_table_free_type(t);
    }    

    /* Clear the root table */
    if (count) {
        flecs_table_reset(world, &world->store.root);
    }
}

static
void fini_store(ecs_world_t *world) {
    clean_tables(world);
    flecs_sparse_free(world->store.tables);
    flecs_table_free(world, &world->store.root);
    flecs_sparse_clear(world->store.entity_index);
    flecs_hashmap_free(world->store.table_map);
}

/* -- Public functions -- */

ecs_world_t *ecs_mini(void) {
    ecs_os_init();

    ecs_trace_1("bootstrap");
    ecs_log_push();

    if (!ecs_os_has_heap()) {
        ecs_abort(ECS_MISSING_OS_API, NULL);
    }

    if (!ecs_os_has_threading()) {
        ecs_trace_1("threading not available");
    }

    if (!ecs_os_has_time()) {
        ecs_trace_1("time management not available");
    }

    ecs_world_t *world = ecs_os_calloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;
    world->fini_actions = NULL; 

    world->type_info = flecs_sparse_new(ecs_type_info_t);
    world->id_index = ecs_map_new(ecs_id_record_t, 8);
    world->id_triggers = ecs_map_new(ecs_id_trigger_t, 8);

    world->queries = flecs_sparse_new(ecs_query_t);
    world->triggers = flecs_sparse_new(ecs_trigger_t);
    world->observers = flecs_sparse_new(ecs_observer_t);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    world->name_prefix = NULL;

    world->aliases = flecs_string_hashmap_new();
    world->symbols = flecs_string_hashmap_new();

    monitors_init(&world->monitors);

    world->type_handles = ecs_map_new(ecs_entity_t, 0);
    world->on_activate_components = ecs_map_new(ecs_on_demand_in_t, 0);
    world->on_enable_components = ecs_map_new(ecs_on_demand_in_t, 0);

    world->worker_stages = NULL;
    world->workers_waiting = 0;
    world->workers_running = 0;
    world->quit_workers = false;
    world->is_readonly = false;
    world->is_fini = false;
    world->measure_frame_time = false;
    world->measure_system_time = false;
    world->should_quit = false;
    world->locking_enabled = false;
    world->pipeline = 0;

    world->frame_start_time = (ecs_time_t){0, 0};
    if (ecs_os_has_time()) {
        ecs_os_get_time(&world->world_start_time);
    }

    world->stats.target_fps = 0;
    world->stats.last_id = 0;

    world->stats.delta_time_raw = 0;
    world->stats.delta_time = 0;
    world->stats.time_scale = 1.0;
    world->stats.frame_time_total = 0;
    world->stats.system_time_total = 0;
    world->stats.merge_time_total = 0;
    world->stats.world_time_total = 0;
    world->stats.frame_count_total = 0;
    world->stats.merge_count_total = 0;
    world->stats.systems_ran_frame = 0;
    world->stats.pipeline_build_count_total = 0;
    
    world->range_check_enabled = false;

    world->fps_sleep = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    flecs_stage_init(world, &world->stage);
    ecs_set_stages(world, 1);

    init_store(world);

    flecs_bootstrap(world);

    ecs_log_pop();

    return world;
}

ecs_world_t *ecs_init(void) {
    ecs_world_t *world = ecs_mini();

#ifdef FLECS_MODULE_H
    ecs_trace_1("import builtin modules");
    ecs_log_push();
#ifdef FLECS_SYSTEM_H
    ECS_IMPORT(world, FlecsSystem);
#endif
#ifdef FLECS_PIPELINE_H
    ECS_IMPORT(world, FlecsPipeline);
#endif
#ifdef FLECS_TIMER_H
    ECS_IMPORT(world, FlecsTimer);
#endif
    ecs_log_pop();
#endif

    return world;
}

#define ARG(short, long, action)\
    if (i < argc) {\
        if (argv[i][0] == '-') {\
            if (argv[i][1] == '-') {\
                if (long && !strcmp(&argv[i][2], long ? long : "")) {\
                    action;\
                    parsed = true;\
                }\
            } else {\
                if (short && argv[i][1] == short) {\
                    action;\
                    parsed = true;\
                }\
            }\
        }\
    }

ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[])
{
    (void)argc;
    (void)argv;
    return ecs_init();
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);
    world->should_quit = true;
}

bool ecs_should_quit(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->should_quit;
}

static
void on_demand_in_map_fini(
    ecs_map_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_on_demand_in_t *elem;

    while ((elem = ecs_map_next(&it, ecs_on_demand_in_t, NULL))) {
        ecs_vector_free(elem->systems);
    }

    ecs_map_free(map);
}

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    /* If no id is specified, broadcast to all tables */
    if (!id) {
        ecs_sparse_t *tables = world->store.tables;
        int32_t i, count = flecs_sparse_count(tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
            flecs_table_notify(world, table, event);
        }

    /* If id is specified, only broadcast to tables with id */
    } else {
        ecs_id_record_t *r = flecs_get_id_record(world, id);
        if (!r) {
            return;
        }

        ecs_table_record_t *tr;
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            flecs_table_notify(world, tr->table, event);
        }
    }
}

static
void default_ctor(
    ecs_world_t *world, ecs_entity_t component, const ecs_entity_t *entity_ptr,
    void *ptr, size_t size, int32_t count, void *ctx)
{
    (void)world; (void)component; (void)entity_ptr; (void)ctx;
    ecs_os_memset(ptr, 0, flecs_from_size_t(size) * count);
}

static
void default_copy_ctor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, const void *src_ptr,
    size_t size, int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->copy(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
}

static
void default_move_ctor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->move(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
}

static
void default_ctor_w_move_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->move(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

static
void default_move_ctor_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->move_ctor(world, component, callbacks, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

static
void default_move(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->move(world, component, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
}

static
void default_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    (void)callbacks;
    (void)src_entity;

    /* When there is no move, destruct the destination component & memcpy the
     * component to dst. The src component does not have to be destructed when
     * a component has a trivial move. */
    callbacks->dtor(world, component, dst_entity, dst_ptr, size, count, ctx);
    ecs_os_memcpy(dst_ptr, src_ptr, flecs_from_size_t(size) * count);
}

static
void default_move_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    /* If a component has a move, the move will take care of memcpying the data
     * and destroying any data in dst. Because this is not a trivial move, the
     * src component must also be destructed. */
    callbacks->move(world, component, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *lifecycle)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);

    const EcsComponent *component_ptr = ecs_get(world, component, EcsComponent);

    /* Cannot register lifecycle actions for things that aren't a component */
    ecs_assert(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Cannot register lifecycle actions for components with size 0 */
    ecs_assert(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_type_info_t *c_info = flecs_get_or_create_c_info(world, component);
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);

    if (c_info->lifecycle_set) {
        ecs_assert(c_info->component == component, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(c_info->lifecycle.ctor == lifecycle->ctor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.dtor == lifecycle->dtor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.copy == lifecycle->copy, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.move == lifecycle->move, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
    } else {
        c_info->component = component;
        c_info->lifecycle = *lifecycle;
        c_info->lifecycle_set = true;
        c_info->size = component_ptr->size;
        c_info->alignment = component_ptr->alignment;

        /* If no constructor is set, invoking any of the other lifecycle actions 
         * is not safe as they will potentially access uninitialized memory. For 
         * ease of use, if no constructor is specified, set a default one that 
         * initializes the component to 0. */
        if (!lifecycle->ctor && 
            (lifecycle->dtor || lifecycle->copy || lifecycle->move)) 
        {
            c_info->lifecycle.ctor = default_ctor;   
        }

        /* Set default copy ctor, move ctor and merge */
        if (lifecycle->copy && !lifecycle->copy_ctor) {
            c_info->lifecycle.copy_ctor = default_copy_ctor;
        }

        if (lifecycle->move && !lifecycle->move_ctor) {
            c_info->lifecycle.move_ctor = default_move_ctor;
        }

        if (!lifecycle->ctor_move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    if (lifecycle->move_ctor) {
                        /* If an explicit move ctor has been set, use callback 
                         * that uses the move ctor vs. using a ctor+move */
                        c_info->lifecycle.ctor_move_dtor = 
                            default_move_ctor_w_dtor;
                    } else {
                        /* If no explicit move_ctor has been set, use
                         * combination of ctor + move + dtor */
                        c_info->lifecycle.ctor_move_dtor = 
                            default_ctor_w_move_w_dtor;
                    }
                } else {
                    /* If no dtor has been set, this is just a move ctor */
                    c_info->lifecycle.ctor_move_dtor = 
                        c_info->lifecycle.move_ctor;
                }            
            }
        }

        if (!lifecycle->move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    c_info->lifecycle.move_dtor = default_move_w_dtor;
                } else {
                    c_info->lifecycle.move_dtor = default_move;
                }
            } else {
                if (lifecycle->dtor) {
                    c_info->lifecycle.move_dtor = default_dtor;
                }
            }
        }

        /* Broadcast to all tables since we need to register a ctor for every
         * table that uses the component as itself, as predicate or as object.
         * The latter is what makes selecting the right set of tables complex,
         * as it depends on the predicate of a pair whether the object is used
         * as the component type or not. 
         * A more selective approach requires a more expressive notification
         * framework. */
        flecs_notify_tables(world, 0, &(ecs_table_event_t) {
            .kind = EcsTableComponentInfo,
            .component = component
        });
    }
}

bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);

    const ecs_type_info_t *c_info = flecs_get_c_info(world, component);
    return (c_info != NULL) && c_info->lifecycle_set;
}

void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_action_elem_t *elem = ecs_vector_add(&world->fini_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;
}

void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_action_elem_t *elem = ecs_vector_add(&stage->post_frame_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;    
}

/* Unset data in tables */
static
void fini_unset_tables(
    ecs_world_t *world)
{
    ecs_sparse_t *tables = world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
        flecs_table_remove_actions(world, table);
    }
}

/* Invoke fini actions */
static
void fini_actions(
    ecs_world_t *world)
{
    ecs_vector_each(world->fini_actions, ecs_action_elem_t, elem, {
        elem->action(world, elem->ctx);
    });

    ecs_vector_free(world->fini_actions);
}

/* Cleanup component lifecycle callbacks & systems */
static
void fini_component_lifecycle(
    ecs_world_t *world)
{
    flecs_sparse_free(world->type_info);
}

/* Cleanup queries */
static
void fini_queries(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *query = flecs_sparse_get_dense(world->queries, ecs_query_t, 0);
        ecs_query_fini(query);
    }
    flecs_sparse_free(world->queries);
}

static
void fini_observers(
    ecs_world_t *world)
{
    flecs_sparse_free(world->observers);
}

/* Cleanup stages */
static
void fini_stages(
    ecs_world_t *world)
{
    flecs_stage_deinit(world, &world->stage);
    ecs_set_stages(world, 0);
}

/* Cleanup id index */
static
void fini_id_index(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(world->id_index);
    ecs_id_record_t *r;
    while ((r = ecs_map_next(&it, ecs_id_record_t, NULL))) {
        ecs_map_free(r->table_index);
    }

    ecs_map_free(world->id_index);
}

static
void fini_id_triggers(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(world->id_triggers);
    ecs_id_trigger_t *t;
    while ((t = ecs_map_next(&it, ecs_id_trigger_t, NULL))) {
        ecs_map_free(t->on_add_triggers);
        ecs_map_free(t->on_remove_triggers);
        ecs_map_free(t->on_set_triggers);
        ecs_map_free(t->un_set_triggers);
    }
    ecs_map_free(world->id_triggers);
    flecs_sparse_free(world->triggers);
}

/* Cleanup aliases & symbols */
static
void fini_aliases(
    ecs_hashmap_t *map)
{
    flecs_hashmap_iter_t it = flecs_hashmap_iter(*map);
    ecs_string_t *key;
    while (flecs_hashmap_next_w_key(&it, ecs_string_t, &key, ecs_entity_t)) {
        ecs_os_free(key->value);
    }
    
    flecs_hashmap_free(*map);
}

/* Cleanup misc structures */
static
void fini_misc(
    ecs_world_t *world)
{
    on_demand_in_map_fini(world->on_activate_components);
    on_demand_in_map_fini(world->on_enable_components);
    ecs_map_free(world->type_handles);
    ecs_vector_free(world->fini_tasks);
    monitors_fini(&world->monitors);
}

/* The destroyer of worlds */
int ecs_fini(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    world->is_fini = true;

    /* Operations invoked during UnSet/OnRemove/destructors are deferred and
     * will be discarded after world cleanup */
    ecs_defer_begin(world);

    /* Run UnSet/OnRemove actions for components while the store is still
     * unmodified by cleanup. */
    fini_unset_tables(world);
    
    /* Run fini actions (simple callbacks ran when world is deleted) before
     * destroying the storage */
    fini_actions(world);

    /* This will destroy all entities and components. After this point no more
     * user code is executed. */
    fini_store(world);

    /* Purge deferred operations from the queue. This discards operations but
     * makes sure that any resources in the queue are freed */
    flecs_defer_purge(world, &world->stage);

    /* Entity index is kept alive until this point so that user code can do
     * validity checks on entity ids, even though after store cleanup the index
     * will be empty, so all entity ids are invalid. */
    flecs_sparse_free(world->store.entity_index);
    
    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    fini_stages(world);

    fini_component_lifecycle(world);

    fini_queries(world);

    fini_observers(world);

    fini_id_index(world);

    fini_id_triggers(world);

    fini_aliases(&world->aliases);
    
    fini_aliases(&world->symbols);
    
    fini_misc(world);

    /* In case the application tries to use the memory of the freed world, this
     * will trigger an assert */
    world->magic = 0;

    flecs_increase_timer_resolution(0);

    /* End of the world */
    ecs_os_free(world);

    ecs_os_fini(); 

    return 0;
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_eis_set_size(world, entity_count + ECS_HI_COMPONENT_ID);
}

void flecs_eval_component_monitors(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);    
    eval_component_monitor(world);
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (world->stats.target_fps == 0.0f || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);
    world->measure_system_time = enable;
}

/* Increase timer resolution based on target fps */
static 
void set_timer_resolution(
    FLECS_FLOAT fps)
{
    if(fps >= 60.0f) flecs_increase_timer_resolution(1);
    else flecs_increase_timer_resolution(0);
}

void ecs_set_target_fps(
    ecs_world_t *world,
    FLECS_FLOAT fps)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->stats.target_fps = fps;
        set_timer_resolution(fps);
    }
}

void* ecs_get_context(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    world = ecs_get_world(world);
    return world->context;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(!id_end || id_end > id_start, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!id_end || id_end > world->stats.last_id, ECS_INVALID_PARAMETER, NULL);

    if (world->stats.last_id < id_start) {
        world->stats.last_id = id_start - 1;
    }

    world->stats.min_id = id_start;
    world->stats.max_id = id_end;
}

bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    bool old_value = world->range_check_enabled;
    world->range_check_enabled = enable;
    return old_value;
}

int32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->worker_stages);
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);

    if (enable) {
        if (!world->locking_enabled) {
            world->mutex = ecs_os_mutex_new();
            world->thr_sync = ecs_os_mutex_new();
            world->thr_cond = ecs_os_cond_new();
        }
    } else {
        if (world->locking_enabled) {
            ecs_os_mutex_free(world->mutex);
            ecs_os_mutex_free(world->thr_sync);
            ecs_os_cond_free(world->thr_cond);
        }
    }

    bool old = world->locking_enabled;
    world->locking_enabled = enable;
    return old;
}

void ecs_lock(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->mutex);
}

void ecs_unlock(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->mutex);
}

void ecs_begin_wait(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->thr_sync);
    ecs_os_cond_wait(world->thr_cond, world->thr_sync);
}

void ecs_end_wait(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->thr_sync);
}

const ecs_type_info_t * flecs_get_c_info(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    return flecs_sparse_get(world->type_info, ecs_type_info_t, component);
}

ecs_type_info_t * flecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    const ecs_type_info_t *c_info = flecs_get_c_info(world, component);
    ecs_type_info_t *c_info_mut = NULL;
    if (!c_info) {
        c_info_mut = flecs_sparse_ensure(
            world->type_info, ecs_type_info_t, component);
        ecs_assert(c_info_mut != NULL, ECS_INTERNAL_ERROR, NULL);         
    } else {
        c_info_mut = (ecs_type_info_t*)c_info;
    }

    return c_info_mut;
}

static
FLECS_FLOAT insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    ecs_time_t start = *stop;
    FLECS_FLOAT delta_time = (FLECS_FLOAT)ecs_time_measure(stop);

    if (world->stats.target_fps == (FLECS_FLOAT)0.0) {
        return delta_time;
    }

    FLECS_FLOAT target_delta_time = 
        ((FLECS_FLOAT)1.0 / (FLECS_FLOAT)world->stats.target_fps);

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    FLECS_FLOAT sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 20 times lower than the time one frame
     * should take. This means that this function at most iterates 20 times in
     * a busy loop */
    FLECS_FLOAT sleep_time = sleep / (FLECS_FLOAT)4.0;

    do {
        /* Only call sleep when sleep_time is not 0. On some platforms, even
         * a sleep with a timeout of 0 can cause stutter. */
        if (sleep_time != 0) {
            ecs_sleepf((double)sleep_time);
        }

        ecs_time_t now = start;
        delta_time = (FLECS_FLOAT)ecs_time_measure(&now);
    } while ((target_delta_time - delta_time) > 
        (sleep_time / (FLECS_FLOAT)2.0));

    return delta_time;
}

static
FLECS_FLOAT start_measure_frame(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    FLECS_FLOAT delta_time = 0;

    if (world->measure_frame_time || (user_delta_time == 0)) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.sec) {
                delta_time = insert_sleep(world, &t);

                ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->stats.target_fps != 0) {
                    delta_time = (FLECS_FLOAT)1.0 / world->stats.target_fps;
                } else {
                    /* Best guess */
                    delta_time = (FLECS_FLOAT)1.0 / (FLECS_FLOAT)60.0; 
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Keep track of total time passed in world */
        world->stats.world_time_total_raw += (FLECS_FLOAT)delta_time;
    }

    return (FLECS_FLOAT)delta_time;
}

static
void stop_measure_frame(
    ecs_world_t* world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        world->stats.frame_time_total += (FLECS_FLOAT)ecs_time_measure(&t);
    }
}

FLECS_FLOAT ecs_frame_begin(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    ecs_assert(user_delta_time != 0 || ecs_os_has_time(), ECS_MISSING_OS_API, "get_time");

    if (world->locking_enabled) {
        ecs_lock(world);
    }

    /* Start measuring total frame time */
    FLECS_FLOAT delta_time = start_measure_frame(world, user_delta_time);
    if (user_delta_time == 0) {
        user_delta_time = delta_time;
    }  

    world->stats.delta_time_raw = user_delta_time;
    world->stats.delta_time = user_delta_time * world->stats.time_scale;

    /* Keep track of total scaled time passed in world */
    world->stats.world_time_total += world->stats.delta_time;

    flecs_eval_component_monitors(world);

    return world->stats.delta_time;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    world->stats.frame_count_total ++;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        flecs_stage_merge_post_frame(world, stage);
    });        

    if (world->locking_enabled) {
        ecs_unlock(world);

        ecs_os_mutex_lock(world->thr_sync);
        ecs_os_cond_broadcast(world->thr_cond);
        ecs_os_mutex_unlock(world->thr_sync);
    }

    stop_measure_frame(world);
}

const ecs_world_info_t* ecs_get_world_info(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return &world->stats;
}

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        flecs_query_notify(world, 
            flecs_sparse_get_dense(world->queries, ecs_query_t, i), event);
    }    
}

void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    /* Notify queries that table is to be removed */
    flecs_notify_queries(
        world, &(ecs_query_event_t){
            .kind = EcsQueryTableUnmatch,
            .table = table
        });

    uint64_t id = table->id;

    /* Free resources associated with table */
    flecs_table_free(world, table);
    flecs_table_free_type(table);

    /* Remove table from sparse set */
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    flecs_sparse_remove(world->store.tables, id);
}

static
void register_table_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column)
{
    ecs_id_record_t *r = flecs_ensure_id_record(world, id);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!r->table_index) {
        r->table_index = ecs_map_new(ecs_table_record_t, 1);
    }

    ecs_table_record_t *tr = ecs_map_ensure(
        r->table_index, ecs_table_record_t, table->id);

    /* A table can be registered for the same entity multiple times if this is
     * a trait. In that case make sure the column with the first occurrence is
     * registered with the index */
    if (!tr->table || column < tr->column) {
        tr->table = table;
        tr->column = column;
        tr->count = 1;
    } else {
        tr->count ++;
    }

    char buf[255]; ecs_id_str(world, id, buf, 255);

    /* Set flags if triggers are registered for table */
    if (!(table->flags & EcsTableIsDisabled)) {
        if (flecs_triggers_get(world, id, EcsOnAdd)) {
            table->flags |= EcsTableHasOnAdd;
        }
        if (flecs_triggers_get(world, id, EcsOnRemove)) {
            table->flags |= EcsTableHasOnRemove;
        }
        if (flecs_triggers_get(world, id, EcsOnSet)) {
            table->flags |= EcsTableHasOnSet;
        }
        if (flecs_triggers_get(world, id, EcsUnSet)) {
            table->flags |= EcsTableHasUnSet;
        }                
    }
}

static
void unregister_table_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_id_record_t *r = flecs_get_id_record(world, id);
    if (!r || !r->table_index) {
        return;
    }

    ecs_map_remove(r->table_index, table->id);
    if (!ecs_map_count(r->table_index)) {
        flecs_clear_id_record(world, id);
    }
}

static
void do_register_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column,
    bool unregister)
{
    if (unregister) {
        unregister_table_for_id(world, table, id);
    } else {
        register_table_for_id(world, table, id, column);
    }
}

static
void do_register_each_id(
    ecs_world_t *world,
    ecs_table_t *table,
    bool unregister)
{
    int32_t i, count = ecs_vector_count(table->type);
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    bool has_childof = false;
    
    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];

        /* This check ensures that legacy CHILDOF works */
        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            has_childof = true;
        } 

        do_register_id(world, table, id, i, unregister);
        do_register_id(world, table, EcsWildcard, i, unregister);

        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t pred_w_wildcard = ecs_pair(
                ECS_PAIR_RELATION(id), EcsWildcard);       
            do_register_id(world, table, pred_w_wildcard, i, unregister);

            ecs_entity_t obj_w_wildcard = ecs_pair(
                EcsWildcard, ECS_PAIR_OBJECT(id));
            do_register_id(world, table, obj_w_wildcard, i, unregister);

            ecs_entity_t all_wildcard = ecs_pair(EcsWildcard, EcsWildcard);
            do_register_id(world, table, all_wildcard, i, unregister);

            if (!unregister) {
                flecs_set_watch(world, ecs_pair_relation(world, id));
                flecs_set_watch(world, ecs_pair_object(world, id));
            }
        } else {
            if (id & ECS_ROLE_MASK) {
                id &= ECS_COMPONENT_MASK;
                do_register_id(world, table, ecs_pair(id, EcsWildcard), 
                    i, unregister);
            }

            if (!unregister) {
                flecs_set_watch(world, id);
            }            
        }
    }

    if (!has_childof) {
        do_register_id(world, table, ecs_pair(EcsChildOf, 0), 0, unregister);
    }
}

void flecs_register_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    do_register_each_id(world, table, false);
}

void flecs_unregister_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    /* Remove table from id indices */
    do_register_each_id(world, table, true);

    /* Remove table from table map */
    ecs_ids_t key = {
        .array = ecs_vector_first(table->type, ecs_id_t),
        .count = ecs_vector_count(table->type)
    };

    flecs_hashmap_remove(world->store.table_map, &key, ecs_table_t*);
}

ecs_id_record_t* flecs_ensure_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_map_ensure(world->id_index, ecs_id_record_t, id);
}

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_map_get(world->id_index, ecs_id_record_t, id);
}

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_id_record_t* idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    return ecs_map_get(idr->table_index, ecs_table_record_t, table->id);
}

void flecs_clear_id_record(
    const ecs_world_t *world,
    ecs_id_t id)    
{
    ecs_id_record_t *r = flecs_get_id_record(world, id);
    if (!r) {
        return;
    }

    ecs_map_free(r->table_index);
    ecs_map_remove(world->id_index, id);
}
