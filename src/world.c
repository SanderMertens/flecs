#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "include/util/time.h"
#include "include/util/stats.h"
#include "include/private/flecs.h"


/* -- Global array parameters -- */

const EcsArrayParams table_arr_params = {
    .element_size = sizeof(EcsTable)
};

const EcsArrayParams handle_arr_params = {
    .element_size = sizeof(EcsEntity)
};

const EcsArrayParams stage_arr_params = {
    .element_size = sizeof(EcsStage)
};

const EcsArrayParams char_arr_params = {
    .element_size = sizeof(char)
};


/* -- Global variables -- */

EcsType TEcsComponent;
EcsType TEcsTypeComponent;
EcsType TEcsPrefab;
EcsType TEcsRowSystem;
EcsType TEcsColSystem;
EcsType TEcsId;
EcsType TEcsHidden;
EcsType TEcsContainer;

const char *ECS_COMPONENT_ID =      "EcsComponent";
const char *ECS_TYPE_COMPONENT_ID = "EcsTypeComponent";
const char *ECS_PREFAB_ID =         "EcsPrefab";
const char *ECS_ROW_SYSTEM_ID =     "EcsRowSystem";
const char *ECS_COL_SYSTEM_ID =     "EcsColSystem";
const char *ECS_ID_ID =             "EcsId";
const char *ECS_HIDDEN_ID =         "EcsHidden";
const char *ECS_CONTAINER_ID =      "EcsContainer";


/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(EcsEntity*)p1 - *(EcsEntity*)p2;
}

/** Bootstrap builtin component types and commonly used types */
static
void bootstrap_types(
    EcsWorld *world)
{
    EcsStage *stage = &world->main_stage;
    TEcsComponent = ecs_type_register(world, stage, EEcsComponent, NULL);
    TEcsTypeComponent = ecs_type_register(world, stage, EEcsTypeComponent, NULL);
    TEcsPrefab = ecs_type_register(world, stage, EEcsPrefab, NULL);
    TEcsRowSystem = ecs_type_register(world, stage, EEcsRowSystem, NULL);
    TEcsColSystem = ecs_type_register(world, stage, EEcsColSystem, NULL);
    TEcsId = ecs_type_register(world, stage, EEcsId, NULL);
    TEcsHidden = ecs_type_register(world, stage, EEcsHidden, NULL);
    TEcsContainer = ecs_type_register(world, stage, EEcsContainer, NULL);    

    world->t_component = ecs_type_merge(world, stage, TEcsComponent, TEcsId, 0);
    world->t_type = ecs_type_merge(world, stage, TEcsTypeComponent, TEcsId, 0);
    world->t_prefab = ecs_type_merge(world, stage, TEcsPrefab, TEcsId, 0);
    world->t_row_system = ecs_type_merge(world, stage, TEcsRowSystem, TEcsId, 0);
    world->t_col_system = ecs_type_merge(world, stage, TEcsColSystem, TEcsId, 0);
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. */
static
EcsTable* bootstrap_component_table(
    EcsWorld *world)
{
    EcsStage *stage = &world->main_stage;
    EcsTable *result = ecs_array_add(&stage->tables, &table_arr_params);
    EcsArray *type = ecs_map_get(stage->type_index, world->t_component);
    result->type_id = world->t_component;
    result->type = type;
    result->frame_systems = NULL;
    result->columns = malloc(sizeof(EcsTableColumn) * 3);
    result->columns[0].data = ecs_array_new(&handle_arr_params, 8);
    result->columns[0].size = sizeof(EcsEntity);
    result->columns[1].data = ecs_array_new(&handle_arr_params, 8);
    result->columns[1].size = sizeof(EcsComponent);
    result->columns[2].data = ecs_array_new(&handle_arr_params, 8);
    result->columns[2].size = sizeof(EcsId);

    uint32_t index = ecs_array_get_index(
        stage->tables, &table_arr_params, result);

    ecs_assert(index == 0, ECS_INTERNAL_ERROR, "first table index must be 0");

    ecs_map_set64(stage->table_index, world->t_component, 1);

    return result;
}

/** Bootstrap the EcsComponent component */
static
void bootstrap_component(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity entity,
    const char *id,
    size_t size)
{
    EcsStage *stage = &world->main_stage;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_insert(world, table, table->columns, entity);

    /* Create record in entity index */
    EcsRow row = {.type_id = world->t_component, .index = index};
    ecs_map_set(stage->entity_index, entity, ecs_from_row(row));

    /* Set size and id */
    EcsComponent *component_data = ecs_array_buffer(table->columns[1].data);
    EcsId *id_data = ecs_array_buffer(table->columns[2].data);
    
    component_data[index].size = size;
    id_data[index] = id;
}

static
void notify_create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *systems,
    EcsTable *table)
{
    EcsEntity *buffer = ecs_array_buffer(systems);
    uint32_t i, count = ecs_array_count(systems);

    for (i = 0; i < count; i ++) {
        ecs_col_system_notify_of_table(world, buffer[i], table);
    }
}

/** Create a new table and register it with the world and systems. A table in
 * flecs is equivalent to an archetype */
static
EcsTable* create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id)
{
    /* Add and initialize table */
    bool is_main_table_array = stage->tables == world->main_stage.tables;
    EcsTable *result = ecs_array_add(&stage->tables, &table_arr_params);


    /* The tables array is shared between the main stage and temp stage. Make
     * sure to update the array pointer in both stages. */
    if (is_main_table_array) {
        world->main_stage.tables = stage->tables;
        world->temp_stage.tables = stage->tables;
    }
    
    result->type_id = type_id;

    if (ecs_table_init(world, stage, result) != EcsOk) {
        return NULL;
    }

    uint32_t index = ecs_array_get_index(stage->tables, &table_arr_params, result);
    ecs_map_set64(stage->table_index, type_id, index + 1);

    /* Notify all column systems of a new table. Only notify for tables that are
     * added/merged to the main stage, as this could otherwise introduce race
     * conditions between threads. 
     * 
     * Notifying systems is necessary, because column systems are prematched
     * with tables, so this doesn't need to happen in the main loop. */
    if (!world->in_progress) {
        notify_create_table(world, stage, world->pre_frame_systems, result);
        notify_create_table(world, stage, world->post_frame_systems, result);
        notify_create_table(world, stage, world->on_load_systems, result);
        notify_create_table(world, stage, world->on_store_systems, result);
        notify_create_table(world, stage, world->on_frame_systems, result);
        notify_create_table(world, stage, world->inactive_systems, result);
        notify_create_table(world, stage, world->on_demand_systems, result);
    }

    assert(result != NULL);

    return result;
}

/* -- Private functions -- */

/** Get pointer to table data from type id */
EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id)
{
    EcsStage *main_stage = &world->main_stage;
    uint32_t table_index = ecs_map_get64(main_stage->table_index, type_id);

    if (!table_index && world->in_progress) {
        assert(stage != NULL);
        table_index = ecs_map_get64(stage->table_index, type_id);
        if (table_index) {
            return ecs_array_get(
                stage->tables, &table_arr_params, table_index - 1);
        }
    }

    if (table_index) {
        return ecs_array_get(
            main_stage->tables, &table_arr_params, table_index - 1);
    } else {
        return create_table(world, stage, type_id);
    }

    return NULL;
}

static
EcsArray** frame_system_array(
    EcsWorld *world,
    EcsSystemKind kind)
{
    if (kind == EcsOnFrame) {
        return &world->on_frame_systems;
    } else if (kind == EcsPreFrame) {
        return &world->pre_frame_systems;
    } else if (kind == EcsPostFrame) {
        return &world->post_frame_systems;
    } else if (kind == EcsOnLoad) {
        return &world->on_load_systems;
    } else if (kind == EcsOnStore) {
        return &world->on_store_systems;
    } else if (kind == EcsManual) {
        return &world->on_demand_systems;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, 0);
    }

    return NULL;
}

/** Inactive systems are systems that either:
 * - are not enabled
 * - matched with no tables
 * - matched with only empty tables.
 *
 * These systems are not considered in the main loop, which can speed up things
 * when applications contain large numbers of disabled systems.
 */
void ecs_world_activate_system(
    EcsWorld *world,
    EcsEntity system,
    EcsSystemKind kind,
    bool active)
{
    EcsArray *src_array, *dst_array;

    if (active) {
        src_array = world->inactive_systems;
        dst_array = *frame_system_array(world, kind);
     } else {
        src_array = *frame_system_array(world, kind);
        dst_array = world->inactive_systems;
    }

    uint32_t count = ecs_array_count(src_array);
    int i;
    for (i = 0; i < count; i ++) {
        EcsEntity *h = ecs_array_get(
            src_array, &handle_arr_params, i);
        if (*h == system) {
            break;
        }
    }

    if (i == count) {
        return; /* System is disabled */
    }

    ecs_array_move_index(
        &dst_array, src_array, &handle_arr_params, i);

    if (active) {
         *frame_system_array(world, kind) = dst_array;
         qsort(dst_array, ecs_array_count(dst_array) + 1,
          sizeof(EcsEntity), compare_handle);
    } else {
        world->inactive_systems = dst_array;
        qsort(src_array, ecs_array_count(src_array) + 1,
          sizeof(EcsEntity), compare_handle);
    }
}

union RowUnion {
    EcsRow row;
    uint64_t value;
};

/** Utility to translate from uint64 to EcsRow */
EcsRow ecs_to_row(
    uint64_t value)
{
    union RowUnion u = {.value = value};
    return u.row;
}

/** Utility to translate from EcsRow to uint64 */
uint64_t ecs_from_row(
    EcsRow row)
{
    union RowUnion u = {.row = row};
    return u.value;
}

char* ecs_type_tostr(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id)
{
    EcsArray *type = ecs_type_get(world, stage, type_id);
    EcsArray *chbuf = ecs_array_new(&char_arr_params, 32);
    char *dst;
    uint32_t len;
    char buf[15];

    EcsEntity *handles = ecs_array_buffer(type);
    uint32_t i, count = ecs_array_count(type);

    for (i = 0; i < count; i ++) {
        EcsEntity h = handles[i];
        if (i) {
            *(char*)ecs_array_add(&chbuf, &char_arr_params) = ',';
        }

        const char *str = NULL;
        EcsId *id = ecs_get_ptr(world, h, EcsId);
        if (id) {
            str = *id;
        } else {
            sprintf(buf, "%" PRIu64, h);
            str = buf;
        }
        len = strlen(str);
        dst = ecs_array_addn(&chbuf, &char_arr_params, len);
        memcpy(dst, str, len);
    }

    *(char*)ecs_array_add(&chbuf, &char_arr_params) = '\0';

    char *result = strdup(ecs_array_buffer(chbuf));
    ecs_array_free(chbuf);
    return result;
}

EcsStage *ecs_get_stage(
    EcsWorld **world_ptr)
{
    EcsWorld *world = *world_ptr;
    if (world->magic == ECS_WORLD_MAGIC) {
        if (world->in_progress) {
            return &world->temp_stage;
        } else {
            return &world->main_stage;
        }
    } else if (world->magic == ECS_THREAD_MAGIC) {
        EcsThread *thread = (EcsThread*)world;
        *world_ptr = thread->world;
        return thread->stage;
    } else {
        fprintf(stderr, "Invalid world object\n");
        assert(0);
        return NULL;
    }
}

static
void col_systems_deinit(
    EcsWorld *world,
    EcsArray *systems)
{
    uint32_t i, count = ecs_array_count(systems);
    EcsEntity *buffer = ecs_array_buffer(systems);

    for (i = 0; i < count; i ++) {
        EcsColSystem *ptr = ecs_get_ptr(world, buffer[i], EcsColSystem);
        ecs_array_free(ptr->base.columns);
        ecs_array_free(ptr->components);
        ecs_array_free(ptr->inactive_tables);
        ecs_array_free(ptr->jobs);
        ecs_array_free(ptr->tables);
        ecs_array_free(ptr->refs);
    }
}

/* -- Public functions -- */

EcsWorld *ecs_init(void) {
    EcsWorld *world = malloc(sizeof(EcsWorld));
    world->magic = ECS_WORLD_MAGIC;

    world->on_frame_systems = ecs_array_new(&handle_arr_params, 0);
    world->pre_frame_systems = ecs_array_new(&handle_arr_params, 0);
    world->post_frame_systems = ecs_array_new(&handle_arr_params, 0);
    world->on_load_systems = ecs_array_new(&handle_arr_params, 0);
    world->on_store_systems = ecs_array_new( &handle_arr_params, 0);
    world->inactive_systems = ecs_array_new(&handle_arr_params, 0);
    world->on_demand_systems = ecs_array_new(&handle_arr_params, 0);

    world->add_systems = ecs_array_new(&handle_arr_params, 0);
    world->remove_systems = ecs_array_new(&handle_arr_params, 0);
    world->set_systems = ecs_array_new(&handle_arr_params, 0);
    world->tasks = ecs_array_new(&handle_arr_params, 0);
    world->fini_tasks = ecs_array_new(&handle_arr_params, 0);

    world->type_sys_add_index = ecs_map_new(0);
    world->type_sys_remove_index = ecs_map_new(0);
    world->type_sys_set_index = ecs_map_new(0);
    world->type_handles = ecs_map_new(0);
    world->prefab_index = ecs_map_new(0);

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

    world->frame_start = (struct timespec){0, 0};
    world->frame_time = 0;
    world->system_time = 0;
    world->target_fps = 0;
    world->fps_sleep = 0;
    world->tick = 0;

    ecs_stage_init(world, &world->main_stage);
    ecs_stage_init(world, &world->temp_stage);

    /* Initialize families for builtin types */
    bootstrap_types(world);

    /* Create table that will hold components (EcsComponent, EcsId) */
    EcsTable *table = bootstrap_component_table(world);
    assert(table != NULL);

    /* Create records for internal components */
    bootstrap_component(world, table, EEcsComponent, ECS_COMPONENT_ID, sizeof(EcsComponent));
    bootstrap_component(world, table, EEcsTypeComponent, ECS_TYPE_COMPONENT_ID, sizeof(EcsTypeComponent));
    bootstrap_component(world, table, EEcsPrefab, ECS_PREFAB_ID, 0);
    bootstrap_component(world, table, EEcsRowSystem, ECS_ROW_SYSTEM_ID, sizeof(EcsRowSystem));
    bootstrap_component(world, table, EEcsColSystem, ECS_COL_SYSTEM_ID, sizeof(EcsColSystem));
    bootstrap_component(world, table, EEcsId, ECS_ID_ID, sizeof(EcsId));
    bootstrap_component(world, table, EEcsHidden, ECS_HIDDEN_ID, 0);
    bootstrap_component(world, table, EEcsContainer, ECS_CONTAINER_ID, 0);

    world->last_handle = EEcsContainer + 1;

    return world;
}

EcsResult ecs_fini(
    EcsWorld *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(!world->in_progress);
    assert(!world->is_merging);

    uint32_t i, system_count = ecs_array_count(world->fini_tasks);
    if (system_count) {
        EcsEntity *buffer = ecs_array_buffer(world->fini_tasks);
        for (i = 0; i < system_count; i ++) {
            ecs_run_task(world, buffer[i], 0);
        }
    }

    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }

    col_systems_deinit(world, world->on_frame_systems);
    col_systems_deinit(world, world->pre_frame_systems);
    col_systems_deinit(world, world->post_frame_systems);
    col_systems_deinit(world, world->on_load_systems);
    col_systems_deinit(world, world->on_store_systems);
    col_systems_deinit(world, world->on_demand_systems);
    col_systems_deinit(world, world->inactive_systems);

    ecs_stage_deinit(world, &world->main_stage);
    ecs_stage_deinit(world, &world->temp_stage);

    ecs_array_free(world->on_frame_systems);
    ecs_array_free(world->pre_frame_systems);
    ecs_array_free(world->post_frame_systems);
    ecs_array_free(world->on_load_systems);
    ecs_array_free(world->on_store_systems);

    ecs_array_free(world->inactive_systems);
    ecs_array_free(world->on_demand_systems);
    ecs_array_free(world->tasks);
    ecs_array_free(world->fini_tasks);

    ecs_array_free(world->add_systems);
    ecs_array_free(world->remove_systems);
    ecs_array_free(world->set_systems);

    ecs_map_free(world->prefab_index);
    ecs_map_free(world->type_sys_add_index);
    ecs_map_free(world->type_sys_remove_index);
    ecs_map_free(world->type_sys_set_index);
    ecs_map_free(world->type_handles);

    world->magic = 0;

    free(world);

    return EcsOk;
}

void ecs_dim(
    EcsWorld *world,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_map_set_size(world->main_stage.entity_index, entity_count);
}

void _ecs_dim_type(
    EcsWorld *world,
    EcsType type,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        EcsTable *table = ecs_world_get_table(world, &world->main_stage, type);
        if (table) {
            ecs_table_dim(table, entity_count);
        }
    }
}

EcsEntity ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    EcsIter it = ecs_array_iter(world->main_stage.tables, &table_arr_params);

    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        uint32_t column_index;

        if ((column_index = ecs_type_index_of(table->type, EEcsId)) == -1) {
            continue;
        }


        EcsTableColumn *column = &table->columns[column_index + 1];
        EcsId *buffer = ecs_array_buffer(column->data);
        uint32_t i, count = ecs_array_count(column->data);
        
        for (i = 0; i < count; i ++) {
            if (!strcmp(buffer[i], id)) {
                return *(EcsEntity*)ecs_array_get(
                    table->columns[0].data, &handle_arr_params, i);
            }
        }
    }

    return 0;
}

static
void run_single_thread_stage(
    EcsWorld *world,
    EcsArray *systems,
    float delta_time)
{
    uint32_t i, system_count = ecs_array_count(systems);
    if (system_count) {
        EcsEntity *buffer = ecs_array_buffer(systems);

        world->in_progress = true;

        for (i = 0; i < system_count; i ++) {
            ecs_run(world, buffer[i], delta_time, NULL);
        }

        if (world->auto_merge) {
            world->in_progress = false;
            ecs_merge(world);
            world->in_progress = true;
        }
    }
}

static
void run_multi_thread_stage(
    EcsWorld *world,
    EcsArray *systems,
    float delta_time)
{
    /* Run periodic table systems */
    uint32_t i, system_count = ecs_array_count(systems);
    if (system_count) {
        bool valid_schedule = world->valid_schedule;
        EcsEntity *buffer = ecs_array_buffer(systems);

        world->in_progress = true;

        for (i = 0; i < system_count; i ++) {
            if (!valid_schedule) {
                ecs_schedule_jobs(world, buffer[i]);
            }
            ecs_prepare_jobs(world, buffer[i]);
        }
        ecs_run_jobs(world);

        if (world->auto_merge) {
            world->in_progress = false;
            ecs_merge(world);
            world->in_progress = true;
        }
    }
}

static
void run_tasks(
    EcsWorld *world,
    float delta_time)
{
    /* Run periodic row systems (not matched to any entity) */
    uint32_t i, system_count = ecs_array_count(world->tasks);
    if (system_count) {
        world->in_progress = true;

        EcsEntity *buffer = ecs_array_buffer(world->tasks);
        for (i = 0; i < system_count; i ++) {
            ecs_run_task(world, buffer[i], delta_time);
        }
    }
}

static
float start_measure_frame(
    EcsWorld *world,
    float delta_time)
{
    if (world->measure_frame_time || !delta_time) {
        if (world->frame_start.tv_sec) {
            float delta = ut_time_measure(&world->frame_start);
            if (!delta_time) {
                delta_time = delta;
            }
        } else {
            ut_time_measure(&world->frame_start);
            if (world->target_fps) {
                delta_time = 1.0 / world->target_fps;
            } else {
                delta_time = 1.0 / 60.0; /* Best guess */
            }
        }
    }

    return delta_time;
}

static
void stop_measure_frame(
    EcsWorld *world,
    float delta_time)
{
    if (world->measure_frame_time) {
        struct timespec t = world->frame_start;
        world->frame_time += ut_time_measure(&t);
        world->tick ++;

        /* Sleep if processing faster than target FPS */
        float target_fps = world->target_fps;
        if (target_fps) {
            float sleep = (1.0 / target_fps) - delta_time + world->fps_sleep;
            if (sleep < 0) {
                sleep = 0;
            }
            world->fps_sleep = sleep;
            ut_sleepf(sleep);
        }
    }
}

bool ecs_progress(
    EcsWorld *world,
    float delta_time)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    bool measure_frame_time = world->measure_frame_time;
    bool has_threads = ecs_array_count(world->worker_threads) != 0;

    /* Start measuring total frame time */
    delta_time = start_measure_frame(world, delta_time);
    world->delta_time = delta_time;

    run_single_thread_stage(world, world->on_load_systems, delta_time);

    if (has_threads) {
        run_multi_thread_stage(world, world->pre_frame_systems, delta_time);
        run_multi_thread_stage(world, world->on_frame_systems, delta_time);
        run_multi_thread_stage(world, world->post_frame_systems, delta_time);
        world->valid_schedule = true;
    } else {
        run_single_thread_stage(world, world->pre_frame_systems, delta_time);
        run_single_thread_stage(world, world->on_frame_systems, delta_time);
        run_single_thread_stage(world, world->post_frame_systems, delta_time);
    }

    run_tasks(world, delta_time);

    run_single_thread_stage(world, world->on_store_systems, delta_time);

    /* Profile system time & merge if systems were processed */
    if (world->in_progress) {
        if (measure_frame_time) {
            struct timespec temp = world->frame_start;
            world->system_time += ut_time_measure(&temp);
        }

        world->in_progress = false;
    }

    stop_measure_frame(world, delta_time);

    return !world->should_quit;
}

float ecs_get_delta_time(
    EcsWorld *world)
{
    return world->delta_time;
}

void ecs_quit(
    EcsWorld *world)
{
    world->should_quit = true;
}

void ecs_merge(
    EcsWorld *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(world->is_merging == false);

    world->is_merging = true;

    ecs_stage_merge(world, &world->temp_stage);

    uint32_t i, count = ecs_array_count(world->worker_stages);
    EcsStage *buffer = ecs_array_buffer(world->worker_stages);
    for (i = 0; i < count; i ++) {
        ecs_stage_merge(world, &buffer[i]);
    }

    world->is_merging = false;
}

void ecs_set_automerge(
    EcsWorld *world,
    bool auto_merge)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    world->auto_merge = auto_merge;
}

void ecs_measure_frame_time(
    EcsWorld *world,
    bool enable)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (!world->target_fps || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    EcsWorld *world,
    bool enable)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    world->measure_system_time = enable;
}

void ecs_set_target_fps(
    EcsWorld *world,
    float fps)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_measure_frame_time(world, true);
    world->target_fps = fps;
}

void* ecs_get_context(
    EcsWorld *world)
{
    ecs_get_stage(&world);
    return world->context;
}

void ecs_set_context(
    EcsWorld *world,
    void *context)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    world->context = context;
}

EcsEntity ecs_import(
    EcsWorld *world,
    EcsModuleInitAction module,
    const char *module_name,
    int flags,
    void *handles_out,
    size_t handles_size)
{
    EcsEntity e = ecs_lookup(world, module_name);
    if (!e) {
        module(world, flags, handles_out);

        /* Register component for module that contains handles */
        e = ecs_new_component(world, module_name, handles_size);
        EcsType t = ecs_type_from_entity(world, e);

        /* Set module handles component on singleton */
        _ecs_set_singleton_ptr(world, t, handles_size, handles_out);
    }

    return e;
}

