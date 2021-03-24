#include "private_api.h"

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

const ecs_stage_t* ecs_stage_from_readonly_world(
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

ecs_stage_t *ecs_stage_from_world(
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
    ecs_world_t *world,
    ecs_component_monitor_t *mon)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    if (!mon->rematch) {
        return;
    }

    ecs_vector_t *eval[ECS_HI_COMPONENT_ID];
    int32_t eval_count = 0;

    int32_t i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        if (mon->dirty_flags[i]) {
            eval[eval_count ++] = mon->monitors[i];
            mon->dirty_flags[i] = 0;
        }
    }

    for (i = 0; i < eval_count; i ++) {
        ecs_vector_each(eval[i], ecs_query_t*, q_ptr, {
            ecs_query_notify(world, *q_ptr, &(ecs_query_event_t) {
                .kind = EcsQueryTableRematch
            });
        });
    }
    
    mon->rematch = false;
}

void ecs_component_monitor_mark(
    ecs_component_monitor_t *mon,
    ecs_entity_t component)
{
    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    if (mon->monitors[component]) {
        mon->dirty_flags[component] = true;
        mon->rematch = true;
    }
}

void ecs_component_monitor_register(
    ecs_component_monitor_t *mon,
    ecs_entity_t component,
    ecs_query_t *query)
{
    ecs_assert(mon != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Ignore component ids > ECS_HI_COMPONENT_ID */
    if(component >= ECS_HI_COMPONENT_ID) {
        return;
    }    

    ecs_query_t **q = ecs_vector_add(&mon->monitors[component], ecs_query_t*);
    *q = query;
}

static
void ecs_component_monitor_free(
    ecs_component_monitor_t *mon)
{
    int i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_vector_free(mon->monitors[i]);
    }
}

static
void init_store(
    ecs_world_t *world) 
{
    ecs_os_memset(&world->store, 0, ECS_SIZEOF(ecs_store_t));
    
    /* Initialize entity index */
    world->store.entity_index = ecs_sparse_new(ecs_record_t);
    ecs_sparse_set_id_source(world->store.entity_index, &world->stats.last_id);

    /* Initialize root table */
    world->store.tables = ecs_sparse_new(ecs_table_t);

    /* Initialize table map */
    world->store.table_map = ecs_map_new(ecs_vector_t*, 8);

    /* Initialize one root table per stage */
    ecs_init_root_table(world);
}

static
void clean_tables(
    ecs_world_t *world)
{
    int32_t i, count = ecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *t = ecs_sparse_get(world->store.tables, ecs_table_t, i);
        ecs_table_free(world, t);
    }

    /* Clear the root table */
    if (count) {
        ecs_table_reset(world, &world->store.root);
    }
}

static
void fini_store(ecs_world_t *world) {
    clean_tables(world);
    ecs_sparse_free(world->store.tables);
    ecs_table_free(world, &world->store.root);
    ecs_sparse_free(world->store.entity_index);

    ecs_map_iter_t it = ecs_map_iter(world->store.table_map);
    ecs_vector_t *tables;
    while ((tables = ecs_map_next_ptr(&it, ecs_vector_t*, NULL))) {
        ecs_vector_free(tables);
    }
    
    ecs_map_free(world->store.table_map);
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
    world->type_info = ecs_sparse_new(ecs_c_info_t);
    world->fini_actions = NULL; 

    world->aliases = NULL;

    world->queries = ecs_vector_new(ecs_query_t*, 0);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    world->child_tables = NULL;
    world->name_prefix = NULL;

    memset(&world->component_monitors, 0, sizeof(world->component_monitors));
    memset(&world->parent_monitors, 0, sizeof(world->parent_monitors));

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
    
    world->range_check_enabled = true;

    world->fps_sleep = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    ecs_stage_init(world, &world->stage);
    ecs_set_stages(world, 1);

    init_store(world);

    ecs_bootstrap(world);

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
    ecs_stage_from_world(&world);
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
void on_demand_in_map_deinit(
    ecs_map_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_on_demand_in_t *elem;

    while ((elem = ecs_map_next(&it, ecs_on_demand_in_t, NULL))) {
        ecs_vector_free(elem->systems);
    }

    ecs_map_free(map);
}

static
void ctor_init_zero(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)ctx;
    ecs_os_memset(ptr, 0, ecs_from_size_t(size) * count);
}

void ecs_notify_tables(
    ecs_world_t *world,
    ecs_table_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_sparse_t *tables = world->store.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_table_notify(world, table, event);
    }
}

void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *lifecycle)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_from_world(&world);

#ifndef NDEBUG
    const EcsComponent *component_ptr = ecs_get(world, component, EcsComponent);

    /* Cannot register lifecycle actions for things that aren't a component */
    ecs_assert(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Cannot register lifecycle actions for components with size 0 */
    ecs_assert(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);
#endif

    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, component);
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

        /* If no constructor is set, invoking any of the other lifecycle actions 
         * is not safe as they will potentially access uninitialized memory. For 
         * ease of use, if no constructor is specified, set a default one that 
         * initializes the component to 0. */
        if (!lifecycle->ctor && (lifecycle->dtor || lifecycle->copy || lifecycle->move)) {
            c_info->lifecycle.ctor = ctor_init_zero;   
        }

        ecs_notify_tables(world, &(ecs_table_event_t) {
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

    const ecs_c_info_t *c_info = ecs_get_c_info(world, component);
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
    ecs_stage_t *stage = ecs_stage_from_world(&world);

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
    int32_t i, count = ecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(world->store.tables, ecs_table_t, i);
        ecs_table_unset(world, table);
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
    int32_t i, count = ecs_sparse_count(world->type_info);
    for (i = 0; i < count; i ++) {
        ecs_c_info_t *type_info = ecs_sparse_get(
            world->type_info, ecs_c_info_t, i);
        ecs_assert(type_info != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_vector_free(type_info->on_add);
        ecs_vector_free(type_info->on_remove);
    }

    ecs_sparse_free(world->type_info);
}

/* Cleanup queries */
static
void fini_queries(
    ecs_world_t *world)
{
    /* Set world->queries to NULL, so ecs_query_free won't attempt to remove
     * itself from the vector */
    ecs_vector_t *query_vec = world->queries;
    world->queries = NULL;

    int32_t i, count = ecs_vector_count(query_vec);
    ecs_query_t **queries = ecs_vector_first(query_vec, ecs_query_t*);
    for (i = 0; i < count; i ++) {
        ecs_query_free(queries[i]);
    }

    ecs_vector_free(query_vec);
}

/* Cleanup stages */
static
void fini_stages(
    ecs_world_t *world)
{
    ecs_stage_deinit(world, &world->stage);
    ecs_set_stages(world, 0);
}

/* Cleanup child table admin */
static
void fini_child_tables(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(world->child_tables);
    ecs_vector_t *tables;
    while ((tables = ecs_map_next_ptr(&it, ecs_vector_t*, NULL))) {
        ecs_vector_free(tables);
    }

    ecs_map_free(world->child_tables);
}

/* Cleanup aliases */
static
void fini_aliases(
    ecs_world_t *world)
{
    int32_t i, count = ecs_vector_count(world->aliases);
    ecs_alias_t *aliases = ecs_vector_first(world->aliases, ecs_alias_t);

    for (i = 0; i < count; i ++) {
        ecs_os_free(aliases[i].name);
    }

    ecs_vector_free(world->aliases);
}

/* Cleanup misc structures */
static
void fini_misc(
    ecs_world_t *world)
{
    on_demand_in_map_deinit(world->on_activate_components);
    on_demand_in_map_deinit(world->on_enable_components);
    ecs_map_free(world->type_handles);
    ecs_vector_free(world->fini_tasks);
    ecs_component_monitor_free(&world->component_monitors);
    ecs_component_monitor_free(&world->parent_monitors);
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

    fini_unset_tables(world);

    fini_actions(world);    

    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    fini_stages(world);

    fini_store(world);

    fini_component_lifecycle(world);

    fini_queries(world);

    fini_child_tables(world);

    fini_aliases(world);

    fini_misc(world);

    /* In case the application tries to use the memory of the freed world, this
     * will trigger an assert */
    world->magic = 0;

    ecs_increase_timer_resolution(0);

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

void ecs_eval_component_monitors(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);    
    eval_component_monitor(world, &world->component_monitors);
    eval_component_monitor(world, &world->parent_monitors);
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
    if(fps >= 60.0f) ecs_increase_timer_resolution(1);
    else ecs_increase_timer_resolution(0);
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

const ecs_c_info_t * ecs_get_c_info(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    return ecs_sparse_get_sparse(world->type_info, ecs_c_info_t, component);
}

ecs_c_info_t * ecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    const ecs_c_info_t *c_info = ecs_get_c_info(world, component);
    ecs_c_info_t *c_info_mut = NULL;
    if (!c_info) {
        c_info_mut = ecs_sparse_ensure(
            world->type_info, ecs_c_info_t, component);
        ecs_assert(c_info_mut != NULL, ECS_INTERNAL_ERROR, NULL);         
    } else {
        c_info_mut = (ecs_c_info_t*)c_info;
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

    ecs_eval_component_monitors(world);

    return user_delta_time;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    world->stats.frame_count_total ++;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_stage_merge_post_frame(world, stage);
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

void ecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    int32_t i, count = ecs_vector_count(world->queries);
    ecs_query_t **queries = ecs_vector_first(world->queries, ecs_query_t*);

    for (i = 0; i < count; i ++) {
        ecs_query_notify(world, queries[i], event);
    }    
}

void ecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    /* Notify queries that table is to be removed */
    ecs_notify_queries(
        world, &(ecs_query_event_t){
            .kind = EcsQueryTableUnmatch,
            .table = table
        });

    uint32_t id = table->id;

    /* Free resources associated with table */
    ecs_table_free(world, table);

    /* Remove table from sparse set */
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_sparse_remove(world->store.tables, id);

    /* Don't do generations as we want table ids to remain 32 bit */
    ecs_sparse_set_generation(world->store.tables, id);
}
