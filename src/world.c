#include "flecs_private.h"

/* -- Global variables -- */

ecs_type_t TEcsComponent;
ecs_type_t TEcsComponentLifecycle;
ecs_type_t TEcsTrigger;
ecs_type_t TEcsType;
ecs_type_t TEcsParent;
ecs_type_t TEcsPrefab;
ecs_type_t TEcsSystem;
ecs_type_t TEcsColSystem;
ecs_type_t TEcsName;
ecs_type_t TEcsHidden;
ecs_type_t TEcsDisabled;
ecs_type_t TEcsDisabledIntern;
ecs_type_t TEcsInactive;
ecs_type_t TEcsOnDemand;
ecs_type_t TEcsPipeline;
ecs_type_t TEcsPipelineQuery;
ecs_type_t TEcsTimer;
ecs_type_t TEcsRateFilter;
ecs_type_t TEcsTickSource;

static const char *ECS_COMPONENT_NAME =      "EcsComponent";
static const char *ECS_COMPONENT_LIFECYCLE_NAME = "EcsComponentLifecycle";
static const char *ECS_COMPONENT_TRIGGER_NAME = "EcsTrigger";
static const char *ECS_TYPE_NAME =           "EcsType";
static const char *ECS_PARENT_NAME =         "EcsParent";
static const char *ECS_PREFAB_NAME =         "EcsPrefab";
static const char *ECS_SYSTEM_NAME =         "EcsSystem";
static const char *ECS_COL_SYSTEM_NAME =     "EcsColSystem";
static const char *ECS_NAME_NAME =           "EcsName";
static const char *ECS_HIDDEN_NAME =         "EcsHidden";
static const char *ECS_DISABLED_NAME =       "EcsDisabled";
static const char *ECS_DISABLED_INTERN_NAME ="EcsDisabledIntern";
static const char *ECS_INACTIVE_NAME =       "EcsInactive";
static const char *ECS_ON_DEMAND_NAME =      "EcsOnDemand";
static const char *ECS_PIPELINE_NAME =       "EcsPipeline";
static const char *ECS_PIPELINE_QUERY_NAME = "EcsPipelineQuery";
static const char *ECS_TIMER_NAME =          "EcsTimer";
static const char *ECS_RATE_FILTER_NAME =    "EcsRateFilter";
static const char *ECS_TICK_SOURCE_NAME =    "EcsTickSource";

/** Get type for component */
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

/** Bootstrap builtin component types and commonly used types */
static
void bootstrap_types(
    ecs_world_t *world)
{
    TEcsComponent = bootstrap_type(world, EEcsComponent);
    TEcsComponentLifecycle = bootstrap_type(world, EEcsComponentLifecycle);
    TEcsTrigger = bootstrap_type(world, EEcsTrigger);
    TEcsType = bootstrap_type(world, EEcsType);
    TEcsParent = bootstrap_type(world, EEcsParent);
    TEcsPrefab = bootstrap_type(world, EEcsPrefab);
    TEcsColSystem = bootstrap_type(world, EEcsColSystem);
    TEcsName = bootstrap_type(world, EEcsName);
    TEcsHidden = bootstrap_type(world, EEcsHidden);
    TEcsDisabled = bootstrap_type(world, EEcsDisabled);
    TEcsDisabledIntern = bootstrap_type(world, EEcsDisabledIntern);
    TEcsInactive = bootstrap_type(world, EEcsInactive);
    TEcsOnDemand = bootstrap_type(world, EEcsOnDemand);
    TEcsPipeline = bootstrap_type(world, EEcsPipeline);
    TEcsPipelineQuery = bootstrap_type(world, EEcsPipelineQuery);
    TEcsTimer = bootstrap_type(world, EEcsTimer);
    TEcsRateFilter = bootstrap_type(world, EEcsRateFilter);
    TEcsTickSource = bootstrap_type(world, EEcsTickSource);
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {EEcsComponent, EEcsName};
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
    data->columns[1].data = ecs_vector_new(EcsName, EcsLastBuiltin);
    data->columns[1].size = sizeof(EcsName);    
    
    return result;
}

/** Bootstrap the EcsComponent component */
static
void bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *id,
    size_t size)
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
    EcsComponent *component_data = ecs_vector_first(columns[0].data);
    EcsName *id_data = ecs_vector_first(columns[1].data);
    
    component_data[index].size = size;
    id_data[index] = id;
}

static
void rematch_queries(
    ecs_world_t *world)
{
    ecs_sparse_t *queries = world->queries;
    int32_t i, count = ecs_sparse_count(queries);

    for (i = 0; i < count; i ++) {
        ecs_query_t *query = ecs_sparse_get(queries, ecs_query_t, i);
        ecs_rematch_query(world, query);
    }
}

#ifndef NDEBUG
static
void no_threading(
    const char *function)
{
    ecs_os_dbg("threading unavailable: %s not implemented", function);
}

static
void no_time(
    const char *function)
{
    ecs_os_dbg("time management: %s not implemented", function);
}
#endif

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

static
void deinit_tables(
    ecs_world_t *world)
{
    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_table_deinit_components(world, table);
    }
}

/* -- Public functions -- */

ecs_world_t *ecs_init(void) {
    ecs_os_set_api_defaults();

    ecs_trace_1("bootstrap");
    ecs_trace_push();

#ifdef __BAKE__
    ut_init(NULL);
    if (ut_load_init(NULL, NULL, NULL, NULL)) {
        ecs_os_err("warning: failed to initialize package loader");
    }
#endif

    ecs_assert(ecs_os_api.malloc != NULL, ECS_MISSING_OS_API, "malloc");
    ecs_assert(ecs_os_api.realloc != NULL, ECS_MISSING_OS_API, "realloc");
    ecs_assert(ecs_os_api.calloc != NULL, ECS_MISSING_OS_API, "calloc");

    bool time_ok = true;

#ifndef NDEBUG
    bool thr_ok = true;
    if (!ecs_os_api.mutex_new) {thr_ok = false; no_threading("mutex_new");}
    if (!ecs_os_api.mutex_free) {thr_ok = false; no_threading("mutex_free");}
    if (!ecs_os_api.mutex_lock) {thr_ok = false; no_threading("mutex_lock");}
    if (!ecs_os_api.mutex_unlock) {thr_ok = false; no_threading("mutex_unlock");}
    if (!ecs_os_api.cond_new) {thr_ok = false; no_threading("cond_new");}
    if (!ecs_os_api.cond_free) {thr_ok = false; no_threading("cond_free");}
    if (!ecs_os_api.cond_wait) {thr_ok = false; no_threading("cond_wait");}
    if (!ecs_os_api.cond_signal) {thr_ok = false; no_threading("cond_signal");}
    if (!ecs_os_api.cond_broadcast) {thr_ok = false; no_threading("cond_broadcast"); }
    if (!ecs_os_api.thread_new) {thr_ok = false; no_threading("thread_new");}
    if (!ecs_os_api.thread_join) {thr_ok = false; no_threading("thread_join");}
    if (thr_ok) {
        ecs_trace_1("threading available");
    } else {
        ecs_trace_1("threading unavailable");
    }

    if (!ecs_os_api.get_time) {time_ok = false; no_time("get_time");}
    if (!ecs_os_api.sleep) {time_ok = false; no_time("sleep");}
    if (time_ok) {
        ecs_trace_1("time management available");
    } else {
        ecs_trace_1("time management unavailable");
    }
#endif

    ecs_world_t *world = ecs_os_malloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;
    world->component_data = NULL;
    ecs_vector_set_count(
        &world->component_data, ecs_component_data_t, ECS_HI_COMPONENT_ID);
    ecs_component_data_t *cdata_array = ecs_vector_first(world->component_data);
    memset(cdata_array, 0, sizeof(ecs_component_data_t) * ECS_HI_COMPONENT_ID);    

    world->queries = ecs_sparse_new(ecs_query_t, 0);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);

    world->type_handles = ecs_map_new(ecs_entity_t, 0);
    world->on_activate_components = ecs_map_new(ecs_on_demand_in_t, 0);
    world->on_enable_components = ecs_map_new(ecs_on_demand_in_t, 0);

    world->stage_count = 2;
    world->worker_stages = NULL;
    world->worker_threads = NULL;
    world->jobs_finished = 0;
    world->threads_running = 0;
    world->valid_schedule = false;
    world->quit_workers = false;
    world->in_progress = false;
    world->is_merging = false;
    world->auto_merge = true;
    world->measure_frame_time = false;
    world->measure_system_time = false;
    world->last_handle = 0;
    world->should_quit = false;
    world->should_match = false;
    world->locking_enabled = false;

    world->frame_start_time = (ecs_time_t){0, 0};
    if (time_ok) {
        ecs_os_get_time(&world->world_start_time);
    }

    world->target_fps = 0;
    world->fps_sleep = 0;

    world->frame_time_total = 0;
    world->system_time_total = 0;
    world->merge_time_total = 0;
    world->frame_count_total = 0;
    world->world_time_total = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    ecs_stage_init(world, &world->stage);
    ecs_stage_init(world, &world->temp_stage);

    /* Create table that will hold components (EcsComponent, EcsName) */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    ecs_trace_1("initialize builtin components");
    ecs_trace_push();

    /* Create records for internal components */
    bootstrap_component(world, table, EEcsComponent, ECS_COMPONENT_NAME, sizeof(EcsComponent));
    bootstrap_component(world, table, EEcsComponentLifecycle, ECS_COMPONENT_LIFECYCLE_NAME, sizeof(EcsComponentLifecycle));
    bootstrap_component(world, table, EEcsTrigger, ECS_COMPONENT_TRIGGER_NAME, sizeof(EcsTrigger));
    bootstrap_component(world, table, EEcsType, ECS_TYPE_NAME, sizeof(EcsType));
    bootstrap_component(world, table, EEcsParent, ECS_PARENT_NAME, sizeof(EcsParent));
    bootstrap_component(world, table, EEcsPrefab, ECS_PREFAB_NAME, 0);
    bootstrap_component(world, table, EEcsSystem, ECS_SYSTEM_NAME, sizeof(EcsSystem));
    bootstrap_component(world, table, EEcsColSystem, ECS_COL_SYSTEM_NAME, sizeof(EcsColSystem));
    bootstrap_component(world, table, EEcsName, ECS_NAME_NAME, sizeof(EcsName));
    bootstrap_component(world, table, EEcsHidden, ECS_HIDDEN_NAME, 0);
    bootstrap_component(world, table, EEcsDisabled, ECS_DISABLED_NAME, 0);
    bootstrap_component(world, table, EEcsDisabledIntern, ECS_DISABLED_INTERN_NAME, 0);
    bootstrap_component(world, table, EEcsInactive, ECS_INACTIVE_NAME, 0);
    bootstrap_component(world, table, EEcsOnDemand, ECS_ON_DEMAND_NAME, 0);
    bootstrap_component(world, table, EEcsPipeline, ECS_PIPELINE_NAME, 0);
    bootstrap_component(world, table, EEcsPipelineQuery, ECS_PIPELINE_QUERY_NAME, sizeof(EcsPipelineQuery));
    bootstrap_component(world, table, EEcsTimer, ECS_TIMER_NAME, sizeof(EcsTimer));
    bootstrap_component(world, table, EEcsRateFilter, ECS_RATE_FILTER_NAME, sizeof(EcsRateFilter));
    bootstrap_component(world, table, EEcsTickSource, ECS_TICK_SOURCE_NAME, sizeof(EcsTickSource));

    /* Initialize types for builtin types */
    bootstrap_types(world);

    world->last_handle = EcsWorld + 1;
    world->min_handle = 0;
    world->max_handle = 0;

    ecs_trace_pop();

    /* Initialize EcsWorld */
    ecs_set(world, EcsWorld, EcsName, {"EcsWorld"});
    ecs_assert(ecs_get_name(world, EcsWorld) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "EcsWorld") == EcsWorld, ECS_INTERNAL_ERROR, NULL);

    /* Initialize EcsSingleton */
    ecs_set(world, EcsSingleton, EcsName, {"$"});
    ecs_assert(ecs_get_name(world, EcsSingleton) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "$") == EcsSingleton, ECS_INTERNAL_ERROR, NULL);

    ecs_trace_1("initialize builtin triggers");
    ecs_trace_push();
    ecs_init_builtins(world);
    ecs_trace_pop();

    ecs_trace_pop();

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
    /* First parse debug argument so logging is enabled while initializing world */ 
    int i;
    for (i = 1; i < argc; i ++) {
        if (argv[i][0] == '-') {
            bool parsed = false;

            ARG(0, "debug", ecs_os_enable_dbg(true));

            /* Ignore arguments that were not parsed */
            (void)parsed;
        } else {
            /* Ignore arguments that don't start with '-' */
        }
    }

    ecs_world_t *world = ecs_init();
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Parse remaining arguments */
    for (i = 1; i < argc; i ++) {
        if (argv[i][0] == '-') {
            bool parsed = false;
            int threads;
            
            ARG(0, "threads", 
                threads = atoi(argv[i + 1]); 
                ecs_set_threads(world, threads);
                i ++;
            );

            ARG(0, "fps", 
                ecs_set_target_fps(world, atoi(argv[i + 1]));
                world->arg_fps = world->target_fps; 
                i ++);

            ARG(0, "admin", 
				ecs_enable_admin(world, atoi(argv[i + 1]));
                i ++);

            ARG(0, "console", ecs_enable_console(world));

            /* Ignore arguments that were not parsed */
            (void)parsed;
        } else {
            /* Ignore arguments that don't start with '-' */
        }
    }
    
    return world;
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

int ecs_fini(
    ecs_world_t *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(!world->in_progress);
    assert(!world->is_merging);

    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }

    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }    

    deinit_tables(world);

    int32_t i, count = ecs_vector_count(world->component_data);
    ecs_component_data_t *cdata = ecs_vector_first(world->component_data);

    for (i = 0; i < count; i ++) {
        ecs_vector_free(cdata[i].on_add);
        ecs_vector_free(cdata[i].on_remove);
        ecs_vector_free(cdata[i].on_set);
    }
    ecs_vector_free(world->component_data);

    count = ecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *q = ecs_sparse_get(world->queries, ecs_query_t, i);
        ecs_query_free(q);
    }

    ecs_sparse_free(world->queries);
    ecs_map_free(world->type_handles);

    ecs_stage_deinit(world, &world->stage);
    ecs_stage_deinit(world, &world->temp_stage);

    on_demand_in_map_deinit(world->on_activate_components);
    on_demand_in_map_deinit(world->on_enable_components);

    ecs_vector_free(world->fini_tasks);

    world->magic = 0;

    ecs_os_free(world);

#ifdef __BAKE__
    ut_deinit();
#endif    

    return 0;
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_eis_set_size(&world->stage, entity_count);
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

static
ecs_entity_t ecs_lookup_child_in_columns(
    ecs_type_t type,
    ecs_data_t *data,
    ecs_entity_t parent,
    int32_t column_index,
    const char *id)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    if (!columns) {
        return 0;
    }

    ecs_column_t *column = &columns[column_index];
    EcsName *buffer = ecs_vector_first(column->data);
    int32_t i, count = ecs_vector_count(column->data);

    ecs_assert(count == ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
    
    for (i = 0; i < count; i ++) {
        if (!buffer[i]) {
            continue;
        }
        
        if (!strcmp(buffer[i], id)) {
            return *(ecs_entity_t*)ecs_vector_get(
                data->entities, ecs_entity_t, i);
        }
    }

    return 0;
}

ecs_entity_t lookup_child(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_sparse_t *tables,
    ecs_entity_t parent,
    const char *id)
{
    ecs_entity_t result = 0;
    int32_t t, count = ecs_sparse_count(tables);
    int32_t column_index;

    for (t = 0; t < count; t ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, t);
        ecs_type_t type = table->type;

        if ((column_index = ecs_type_index_of(type, EEcsName)) == -1) {
            continue;
        }

        if (parent && ecs_type_index_of(type, ECS_CHILDOF | parent) == -1) {
            continue;
        }

        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        if (data && data->columns) {
            result = ecs_lookup_child_in_columns(
                type, data, parent, column_index, id);
        }

        if (!result && stage != &world->stage) {
            data = ecs_table_get_data(world, table);
            if (!data || !data->columns) {
                continue;
            }
            
            result = ecs_lookup_child_in_columns(
                type, data, parent, column_index, id);
        }

        if (result) {
            break;
        }        
    }

    return result;
}

ecs_entity_t ecs_lookup_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *id)
{
    ecs_entity_t result = 0;
    ecs_stage_t *stage = ecs_get_stage(&world);

    if (stage != &world->stage) {
        result = lookup_child(world, stage, stage->tables, parent, id);
        if (result) {
            return result;
        }
    }

    return lookup_child(world, stage, world->stage.tables, parent, id);
}

ecs_entity_t ecs_lookup(
    ecs_world_t *world,
    const char *id)
{   
    return ecs_lookup_child(world, 0, id);
}

static
float start_measure_frame(
    ecs_world_t *world,
    float user_delta_time)
{
    float delta_time = 0;

    if (world->measure_frame_time || !user_delta_time) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.sec) {
                delta_time = ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->target_fps) {
                    delta_time = 1.0 / world->target_fps;
                } else {
                    delta_time = 1.0 / 60.0; /* Best guess */
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Compute total time passed since start of simulation */
        ecs_time_t diff = ecs_time_sub(t, world->world_start_time);
        world->world_time_total = ecs_time_to_double(diff);
    }

    return delta_time;
}

static
void stop_measure_frame(
    ecs_world_t *world,
    float delta_time)
{
    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        double frame_time = ecs_time_measure(&t);
        world->frame_time_total += frame_time;

        /* Sleep if processing faster than target FPS */
        float target_fps = world->target_fps;
        if (target_fps) {
            float sleep = (1.0 / target_fps) - delta_time + world->fps_sleep;

            if (sleep > 0.01) {
                ecs_sleepf(sleep);
            }

            world->fps_sleep = sleep;
        }        
    }
}

float ecs_frame_begin(
    ecs_world_t *world,
    float user_delta_time)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(user_delta_time || ecs_os_api.get_time, ECS_MISSING_OS_API, "get_time");

    if (world->locking_enabled) {
        ecs_lock(world);
    }

    /* Start measuring total frame time */
    float delta_time = start_measure_frame(world, user_delta_time);

    if (!user_delta_time) {
        user_delta_time = delta_time;
    }

    world->delta_time = user_delta_time;

    if (world->should_match) {
        ecs_trace_2("rematching queries");
        rematch_queries(world);
        world->should_match = false;
    }

    /* Evaluate tick sources */
    bool is_staged = ecs_staging_begin(world);
    ecs_assert(!is_staged, ECS_INTERNAL_ERROR, NULL);
    
    return delta_time;
}

void ecs_frame_end(
    ecs_world_t *world,
    float delta_time)
{
    world->frame_count_total ++;

    ecs_staging_end(world, false);
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    if (world->locking_enabled) {
        ecs_unlock(world);
    }

    stop_measure_frame(world, delta_time);   
}

void ecs_progress_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    float delta_time)
{
    const EcsPipelineQuery *query = ecs_get_ptr(
        world, pipeline, EcsPipelineQuery);

    ecs_time_t start = {0};
    ecs_time_measure(&start);
    
    ecs_query_iter_t it = ecs_query_iter(query->query, 0, 0);
    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;
        ECS_COLUMN(rows, EcsColSystem, sys, 1);

        int32_t i;
        for(i = 0; i < rows->count; i ++) {
            ecs_entity_t e = rows->entities[i];
            ecs_run_intern(world, world, e, &sys[i], delta_time, 0, 0, 
                NULL, NULL);
        }
    }

    world->system_time_total += ecs_time_measure(&start);
}

bool ecs_progress(
    ecs_world_t *world,
    float user_delta_time)
{
    float delta_time = ecs_frame_begin(world, user_delta_time);

    ecs_progress_pipeline(world, world->builtin_pipeline, delta_time);

    ecs_frame_end(world, delta_time);

    return !world->should_quit;
}

float ecs_get_delta_time(
    ecs_world_t *world)
{
    return world->delta_time;
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    world->should_quit = true;
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

    int32_t i, count = ecs_vector_count(world->worker_stages);
    if (count) {
        ecs_stage_t *buffer = ecs_vector_first(world->worker_stages);
        for (i = 0; i < count; i ++) {
            ecs_stage_merge(world, &buffer[i]);
        }
    }

    if (measure_frame_time) {
        world->merge_time_total += ecs_time_measure(&t_start);
    }

    world->is_merging = false;
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
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    if (!world->target_fps || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    world->measure_system_time = enable;
}

void ecs_set_target_fps(
    ecs_world_t *world,
    float fps)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    ecs_assert(ecs_os_api.sleep != NULL, ECS_MISSING_OS_API, "sleep");

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->target_fps = fps;
    }
}

/* Mock types so we don't have to depend on them. 
 * TODO: Need a better workaround */
typedef uint16_t EcsAdmin;
typedef int32_t EcsConsole;

int ecs_enable_admin(
	ecs_world_t* world,
	uint16_t port)
{
    if (!ecs_import_from_library(world, "flecs.systems.civetweb", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.civetweb");
        return 1;
    }
    
    if (!ecs_import_from_library(world, "flecs.systems.admin", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.admin");
        return 1;
    }

    /* Enable monitoring */
    ecs_measure_frame_time(world, true);
    ecs_measure_system_time(world, true);

    /* Create admin instance */
    ecs_entity_t EEcsAdmin = ecs_lookup(world, "EcsAdmin");
    ecs_set(world, 0, EcsAdmin, {port});

    ecs_os_log("admin is running on port %d", port);

    return 0;
}

int ecs_enable_console(
	ecs_world_t* world)
{
    if (!ecs_import_from_library( world, "flecs.systems.console", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.console");
        return 1;
    }

    /* Create console instance */
    ecs_entity_t EEcsConsole = ecs_lookup(world, "EcsConsole");
    ecs_set(world, 0, EcsConsole, {0});

    return 0;
}

void* ecs_get_context(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->context;
}

int32_t ecs_get_tick(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->frame_count_total;
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
    ecs_assert(!id_end || id_end > world->last_handle, ECS_INVALID_PARAMETER, NULL);

    if (world->last_handle < id_start) {
        world->last_handle = id_start - 1;
    }

    world->min_handle = id_start;
    world->max_handle = id_end;
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

ecs_entity_t _ecs_import(
    ecs_world_t *world,
    ecs_module_action_t init_action,
    const char *module_name,
    int flags,
    void *handles_out,
    size_t handles_size)
{
    ecs_entity_t e = ecs_lookup(world, module_name);
    if (!e) {
        /* Load module */
        init_action(world, flags);

        /* Lookup module component (must be registered by module) */
        e = ecs_lookup(world, module_name);
        ecs_assert(e != 0, ECS_MODULE_UNDEFINED, module_name);

        /* Copy value of module component in handles_out parameter */
        if (handles_size && handles_out) {
            void *module_ptr = ecs_get_mut_w_entity(world, EcsSingleton, e, NULL);
            memcpy(handles_out, module_ptr, handles_size);
        }

    /* If module was already loaded, copy module component into handles_out */
    } else if (handles_size) {
        void *handles_ptr = ecs_get_mut_w_entity(world, EcsSingleton, e, NULL);
        memcpy(handles_out, handles_ptr, handles_size);
    }

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name,
    int flags)
{
    ecs_assert(library_name != NULL, ECS_INVALID_PARAMETER, NULL);

    char *import_func = (char*)module_name; /* safe */
    char *module = (char*)module_name;

    if (!ecs_os_api.module_to_dl || 
        !ecs_os_api.dlopen || 
        !ecs_os_api.dlproc || 
        !ecs_os_api.dlclose) 
    {
        ecs_os_err(
            "library loading not supported, set module_to_dl, dlopen, dlclose "
            "and dlproc os API callbacks first");
        return 0;
    }

    /* If no module name is specified, try default naming convention for loading
     * the main module from the library */
    if (!import_func) {
        import_func = ecs_os_malloc(strlen(library_name) + sizeof("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = toupper(ch);
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = tolower(ch);
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_os_dbg("found file '%s' for library '%s'", 
            library_filename, library_name);
    }

    ecs_os_dl_t dl = ecs_os_dlopen(library_filename);
    if (!dl) {
        ecs_os_err("failed to load library '%s' ('%s')", 
            library_name, library_filename);
        
        ecs_os_free(library_filename);

        if (module != module_name) {
            ecs_os_free(module);
        }    

        return 0;
    } else {
        ecs_os_dbg("library '%s' ('%s') loaded", 
            library_name, library_filename);
    }

    ecs_module_action_t action = (ecs_module_action_t)
        ecs_os_dlproc(dl, import_func);
    if (!action) {
        ecs_os_err("failed to load import function %s from library %s",
            import_func, library_name);
        ecs_os_free(library_filename);
        ecs_os_dlclose(dl);            
        return 0;
    } else {
        ecs_os_dbg("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = _ecs_import(world, action, module, flags, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    ecs_os_free(library_filename);

    return result;
}

uint16_t ecs_get_thread_index(
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
    return ecs_vector_count(world->worker_threads);
}

float ecs_get_target_fps(
    ecs_world_t *world)
{
    return world->target_fps;
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
    if (enable) {
        if (!world->locking_enabled) {
            world->mutex = ecs_os_mutex_new();
        }
    } else {
        if (world->locking_enabled) {
            ecs_os_mutex_free(world->mutex);
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

ecs_component_data_t *ecs_get_component_data(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(component < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    ecs_component_data_t* cdata_array = ecs_vector_first(world->component_data);
    return &cdata_array[component];
}

bool ecs_staging_begin(
    ecs_world_t *world)
{
    bool in_progress = world->in_progress;
    world->in_progress = true;
    return in_progress;
}

void ecs_staging_end(
    ecs_world_t *world,
    bool is_staged)
{
    if (!is_staged) {
        world->in_progress = false;
        if (world->auto_merge) {
            ecs_merge(world);
        }
    }
}
