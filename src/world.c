#include "private_api.h"

/* -- Private functions -- */

ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_THREAD_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        if (world->in_progress) {
            return &world->temp_stage;
        } else {
            return &world->stage;
        }
    } else if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thread = (ecs_thread_t*)world;
        *world_ptr = thread->world;
        return thread->stage;
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

void ecs_component_monitor_free(
    ecs_component_monitor_t *mon)
{
    int i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_vector_free(mon->monitors[i]);
    }
}

/* -- Public functions -- */


FLECS_EXPORT
bool ecs_os_has_heap(void);

/** Are threading functions available? */
FLECS_EXPORT
bool ecs_os_has_threading(void);

/** Are time functions available? */
FLECS_EXPORT
bool ecs_os_has_time(void);

/** Are logging functions available? */
FLECS_EXPORT
bool ecs_os_has_logging(void);

/** Are dynamic library functions available? */
FLECS_EXPORT
bool ecs_os_has_dl(void);

/** Are module path functions available? */
FLECS_EXPORT
bool ecs_os_has_modules(void);


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

    ecs_world_t *world = ecs_os_malloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;
    memset(&world->c_info, 0, sizeof(ecs_c_info_t) * ECS_HI_COMPONENT_ID); 

    world->t_info = ecs_map_new(ecs_c_info_t, 0);  
    world->fini_actions = NULL; 

    world->queries = ecs_vector_new(ecs_query_t*, 0);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    world->child_tables = NULL;
    world->name_prefix = NULL;

    memset(&world->component_monitors, 0, sizeof(world->component_monitors));
    memset(&world->parent_monitors, 0, sizeof(world->parent_monitors));

    world->type_handles = ecs_map_new(ecs_entity_t, 0);
    world->on_activate_components = ecs_map_new(ecs_on_demand_in_t, 0);
    world->on_enable_components = ecs_map_new(ecs_on_demand_in_t, 0);

    world->stage_count = 2;
    world->worker_stages = NULL;
    world->workers = NULL;
    world->workers_waiting = 0;
    world->workers_running = 0;
    world->valid_schedule = false;
    world->quit_workers = false;
    world->in_progress = false;
    world->is_merging = false;
    world->auto_merge = true;
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
    world->stats.sleep_err = 0;
    world->stats.system_time_total = 0;
    world->stats.merge_time_total = 0;
    world->stats.world_time_total = 0;
    world->stats.frame_count_total = 0;
    world->stats.merge_count_total = 0;
    world->stats.systems_ran_frame = 0;
    world->stats.pipeline_build_count_total = 0;

    world->fps_sleep = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    ecs_stage_init(world, &world->stage);
    ecs_stage_init(world, &world->temp_stage);

    world->stage.world = world;
    world->temp_stage.world = world;

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
    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_table_notify(world, table, event);
    }
}

void ecs_set_component_actions_w_entity(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *lifecycle)
{
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
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_c_info_t *c_info = ecs_get_c_info(world, component);
    return (c_info != NULL) && c_info->lifecycle_set;
}

void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
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
    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

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
    ecs_sparse_each(world->stage.tables, ecs_table_t, table, {
        ecs_table_unset(world, table);
    });
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
    int32_t i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_vector_free(world->c_info[i].on_add);
        ecs_vector_free(world->c_info[i].on_remove);
    }

    ecs_map_iter_t it = ecs_map_iter(world->t_info);
    ecs_c_info_t *c_info;
    while ((c_info = ecs_map_next(&it, ecs_c_info_t, NULL))) {
        ecs_vector_free(c_info->on_add);
        ecs_vector_free(c_info->on_remove);
    }    

    ecs_map_free(world->t_info);
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
    ecs_stage_deinit(world, &world->temp_stage);
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
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(!world->in_progress);
    assert(!world->is_merging);

    fini_unset_tables(world);

    fini_actions(world);    

    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    fini_stages(world);

    fini_component_lifecycle(world);

    fini_queries(world);

    fini_child_tables(world);

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
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_eis_set_size(&world->stage, entity_count + ECS_HI_COMPONENT_ID);
}

void ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        ecs_table_t *table = ecs_table_from_type(
            world, &world->stage, type);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, table);
        ecs_table_set_size(world, table, data, entity_count);
    }
}

void ecs_eval_component_monitors(
    ecs_world_t *world)
{
    eval_component_monitor(world, &world->component_monitors);
    eval_component_monitor(world, &world->parent_monitors);
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    assert(world->is_merging == false);

    bool measure_frame_time = world->measure_frame_time;

    world->is_merging = true;

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    ecs_stage_merge(world, &world->temp_stage);

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_stage_merge(world, stage);
    });

    world->is_merging = false;

    ecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->stats.merge_time_total += (float)ecs_time_measure(&t_start);
    }

    world->stats.merge_count_total ++;

    /* Execute post frame actions */
    ecs_vector_each(world->stage.post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });

    ecs_vector_free(world->stage.post_frame_actions);
    world->stage.post_frame_actions = NULL;

    ecs_vector_each(world->temp_stage.post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });
    ecs_vector_free(world->temp_stage.post_frame_actions);
    world->temp_stage.post_frame_actions = NULL;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_vector_each(stage->post_frame_actions, ecs_action_elem_t, action, {
            action->action(world, action->ctx);
        });
        ecs_vector_free(stage->post_frame_actions);
        stage->post_frame_actions = NULL;
    });    
}

void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->auto_merge = auto_merge;
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (world->stats.target_fps == 0.0 || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);
    world->measure_system_time = enable;
}

/* Increase timer resolution based on target fps */
static void set_timer_resolution(float fps)
{
    if(fps >= 60.0f) ecs_increase_timer_resolution(1);
    else ecs_increase_timer_resolution(0);
}

void ecs_set_target_fps(
    ecs_world_t *world,
    float fps)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->stats.target_fps = fps;
        set_timer_resolution(fps);
    }
}

void* ecs_get_context(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->context;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
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
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool old_value = stage->range_check_enabled;
    stage->range_check_enabled = enable;
    return old_value;
}

int32_t ecs_get_thread_index(
    ecs_world_t *world)
{
    if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thr = (ecs_thread_t*)world;
        return thr->index;
    } else if (world->magic == ECS_WORLD_MAGIC) {
        return 0;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

int32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->workers);
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
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
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->mutex);
}

void ecs_unlock(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->mutex);
}

void ecs_begin_wait(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->thr_sync);
    ecs_os_cond_wait(world->thr_cond, world->thr_sync);
}

void ecs_end_wait(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->thr_sync);
}

ecs_c_info_t * ecs_get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    if (component < ECS_HI_COMPONENT_ID) {
        ecs_c_info_t *c_info = &world->c_info[component];
        if (c_info->component) {
            ecs_assert(c_info->component == component, ECS_INTERNAL_ERROR, NULL);
            return c_info;
        } else {
            return NULL;
        }
    } else {
        return ecs_map_get(world->t_info, ecs_c_info_t, component);
    }
}

ecs_c_info_t * ecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{    
    ecs_c_info_t *c_info = ecs_get_c_info(world, component);
    if (!c_info) {
        if (component < ECS_HI_COMPONENT_ID) {
            c_info = &world->c_info[component];
            ecs_assert(c_info->component == 0, ECS_INTERNAL_ERROR, NULL);
            c_info->component = component;
        } else {
            ecs_c_info_t t_info = { 0 };
            ecs_map_set(world->t_info, component, &t_info);
            c_info = ecs_map_get(world->t_info, ecs_c_info_t, component);
            ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL); 
        }
    }

    return c_info;
}

bool ecs_staging_begin(
    ecs_world_t *world)
{
    bool in_progress = world->in_progress;
    world->in_progress = true;
    return in_progress;
}

bool ecs_staging_end(
    ecs_world_t *world,
    bool is_staged)
{
    bool result = world->in_progress;

    if (!is_staged) {
        world->in_progress = false;
        if (world->auto_merge) {
            ecs_merge(world);
        }
    }

    return result;
}

const ecs_world_info_t* ecs_get_world_info(
    ecs_world_t *world)
{
    return &world->stats;
}

void ecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event)
{
    int32_t i, count = ecs_vector_count(world->queries);
    ecs_query_t **queries = ecs_vector_first(world->queries, ecs_query_t*);

    for (i = 0; i < count; i ++) {
        ecs_query_notify(world, queries[i], event);
    }    
}
