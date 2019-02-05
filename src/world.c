#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "include/util/time.h"
#include "include/util/stats.h"
#include "include/private/reflecs.h"

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

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(EcsEntity*)p1 - *(EcsEntity*)p2;
}

/** Initialize component table. This table is manually constructed to bootstrap
 * reflecs. After this function has been called, the builtin components can be
 * created. */
static
void bootstrap_component_table(
    EcsWorld *world,
    uint64_t family_id)
{
    EcsTable *result = ecs_array_add(&world->table_db, &table_arr_params);
    EcsArray *family = ecs_family_get(world, NULL, family_id);
    result->family_id = family_id;
    ecs_table_init_w_size(world, result, family, sizeof(EcsComponent));
    result->columns = malloc(sizeof(uint16_t));
    result->columns[0] = sizeof(EcsComponent);
    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, result);
    ecs_map_set64(world->table_index, family_id, table_index + 1);
}

/** Bootstrap the EcsComponent component */
static
void bootstrap_component(
    EcsWorld *world)
{
    EcsEntity handle = ecs_new(world, 0);
    assert (handle == EcsComponent_h);

    EcsFamily family_id = ecs_family_register(world, NULL, handle, NULL);
    ecs_map_set(world->stage.add_stage, handle, family_id);

    bootstrap_component_table(world, family_id);

    assert_func (ecs_commit(world, EcsComponent_h) == EcsOk);

    EcsComponent *type_data = ecs_get_ptr(world, handle, handle);

    assert(type_data != NULL);

    type_data->size = sizeof(EcsComponent);
}

/** Generic function for initializing built-in components */
static
EcsEntity new_builtin_component(
    EcsWorld *world,
    size_t size)
{
    EcsEntity handle = ecs_new(world, 0);
    ecs_stage_add(world, handle, EcsComponent_h);
    ecs_commit(world, handle);
    EcsComponent *component_data = ecs_get_ptr(world, handle, EcsComponent_h);

    assert(component_data != NULL);

    component_data->size = size;

    return handle;
}

/** Generic function to add identifier to builtin entities */
static
void add_builtin_id(
    EcsWorld *world,
    EcsEntity handle,
    const char *id)
{
    ecs_stage_add(world, handle, EcsId_h);
    ecs_commit(world, handle);
    EcsId *id_data = ecs_get_ptr(world, handle, EcsId_h);

    assert(id_data != NULL);

    *id_data = (char*)id;
}

/** Obtain family id for specified component + EcsId */
static
EcsFamily get_builtin_family(
    EcsWorld *world,
    EcsEntity component)
{
    EcsFamily family = ecs_family_register(world, NULL, component, NULL);
    EcsArray *family_array = ecs_family_get(world, NULL, family);
    return ecs_family_register(world, NULL, EcsId_h, family_array);
}

static
void notify_create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *systems,
    EcsTable *table)
{
    EcsIter it = ecs_array_iter(systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity system = *(EcsEntity*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, stage, system, table);
    }
}

/** Create a new table and register it with the world and systems */
static
EcsTable* create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsArray **table_db;
    EcsMap *table_index;

    if (world->in_progress) {
        table_db = &stage->table_db_stage;
        table_index = stage->table_stage;
    } else {
        table_db = &world->table_db;
        table_index = world->table_index;
    }

    EcsTable *result = ecs_array_add(table_db, &table_arr_params);
    result->family_id = family_id;

    if (ecs_table_init(world, stage, result) != EcsOk) {
        return NULL;
    }

    uint32_t index = ecs_array_get_index(*table_db, &table_arr_params, result);
    ecs_map_set64(table_index, family_id, index + 1);

    if (!world->in_progress) {
        /* Only notify systems of tables that aren't staged */
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


static
void clean_families(
    EcsWorld *world)
{
    EcsIter it = ecs_map_iter(world->family_index);
    while (ecs_iter_hasnext(&it)) {
        EcsArray *family = ecs_iter_next(&it);
        ecs_array_free(family);
    }

    ecs_map_free(world->family_index);
}

static
void deinit_row_system(
    EcsRowSystem *data)
{
    ecs_array_free(data->components);
}

static
void deinit_table_system(
    EcsTableSystem *data)
{
    ecs_array_free(data->base.columns);
    ecs_array_free(data->components);
    ecs_array_free(data->tables);
    ecs_array_free(data->inactive_tables);
    if (data->jobs) ecs_array_free(data->jobs);
    if (data->refs) ecs_array_free(data->refs);
    data->base.enabled = false;
}

static
void clean_tables(
    EcsWorld *world)
{
    EcsTable *buffer = ecs_array_buffer(world->table_db);
    int32_t i, count = ecs_array_count(world->table_db);

    for (i = count - 1; i >= 0; i --) {
        EcsTable *table = &buffer[i];

        ecs_table_deinit(world, table);
    }

    /* Free builtin systems */
    EcsRowSystem *deinit_row_sys = ecs_get_ptr(
        world, world->deinit_row_system, EcsRowSystem_h);
    EcsRowSystem *deinit_table_sys = ecs_get_ptr(
        world, world->deinit_table_system, EcsRowSystem_h);

    deinit_row_system(deinit_row_sys);
    deinit_row_system(deinit_table_sys);

    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        ecs_table_free(world, table);
    }

    ecs_array_free(world->table_db);
}

/** Cleanup resources allocated by table systems */
static
void remove_systems(
    EcsRows *rows)
{
    void *row;
    EcsEntity component = rows->components[0];

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, 0);
        if (entity != rows->world->deinit_table_system &&
            entity != rows->world->deinit_row_system)
        {
            void *data = ecs_data(rows, row, 0);
            if (component == EcsTableSystem_h) {
                deinit_table_system(data);
            } else {
                deinit_row_system(data);
            }
        }
    }
}

/* -- Private functions -- */

void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    uint32_t line,
    const char *func)
{
    if (!cond) {
        fprintf(stderr, "Assertion failed: %s, file: %s, line: %d, func: %s\n",
            cond_str, file, line, func);
        assert(0);
    }
}

/** Get pointer to table data from family id */
EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    uint32_t table_index = ecs_map_get64(world->table_index, family_id);

    if (!table_index && world->in_progress) {
        assert(stage != NULL);
        table_index = ecs_map_get64(stage->table_stage, family_id);
        if (table_index) {
            return ecs_array_get(
                stage->table_db_stage, &table_arr_params, table_index - 1);
        }
    }

    if (table_index) {
        return ecs_array_get(
            world->table_db, &table_arr_params, table_index - 1);
    } else {
        return create_table(world, stage, family_id);
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
    } else if (kind == EcsOnDemand) {
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

char* ecs_family_tostr(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsArray *family = ecs_family_get(world, stage, family_id);
    EcsArray *chbuf = ecs_array_new(&char_arr_params, 32);
    char *dst;
    uint32_t len;
    char buf[15];

    EcsEntity *handles = ecs_array_buffer(family);
    uint32_t i, count = ecs_array_count(family);

    for (i = 0; i < count; i ++) {
        EcsEntity h = handles[i];
        if (i) {
            *(char*)ecs_array_add(&chbuf, &char_arr_params) = ',';
        }

        const char *str = NULL;
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
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
        return &world->stage;
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

/* -- Public functions -- */

EcsWorld *ecs_init(void) {
    EcsWorld *world = malloc(sizeof(EcsWorld));
    world->magic = ECS_WORLD_MAGIC;

    world->table_db = ecs_array_new(
        &table_arr_params, ECS_WORLD_INITIAL_TABLE_COUNT);
    world->on_frame_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->pre_frame_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->post_frame_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->on_load_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->on_store_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->inactive_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->on_demand_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);

    world->add_systems = ecs_map_new(ECS_WORLD_INITIAL_INIT_SYSTEM_COUNT);
    world->remove_systems = ecs_map_new(ECS_WORLD_INITIAL_DEINIT_SYSTEM_COUNT);
    world->set_systems = ecs_map_new(ECS_WORLD_INITIAL_SET_SYSTEM_COUNT);
    world->tasks = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->fini_tasks = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);

    world->entity_index = ecs_map_new(ECS_WORLD_INITIAL_ENTITY_COUNT);
    world->table_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    world->family_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    world->family_handles = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    world->prefab_index = ecs_map_new(ECS_WORLD_INITIAL_PREFAB_COUNT);

    world->stage_db = NULL;
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

    ecs_stage_init(&world->stage);

    bootstrap_component(world);
    assert_func(new_builtin_component(world, sizeof(EcsFamilyComponent)) == EcsFamilyComponent_h);
    assert_func(new_builtin_component(world, 0) == EcsPrefab_h);
    assert_func(new_builtin_component(world, sizeof(EcsRowSystem)) == EcsRowSystem_h);
    assert_func(new_builtin_component(world, sizeof(EcsTableSystem)) == EcsTableSystem_h);
    assert_func(new_builtin_component(world, sizeof(EcsId)) == EcsId_h);
    assert_func(new_builtin_component(world, 0) == EcsHidden_h);
    assert_func(new_builtin_component(world, 0) == EcsContainer_h);
    assert_func(new_builtin_component(world, 0) == EcsRoot_h);

    add_builtin_id(world, EcsComponent_h, "EcsComponent");
    add_builtin_id(world, EcsFamilyComponent_h, "EcsFamilyComponent");
    add_builtin_id(world, EcsPrefab_h, "EcsPrefab");
    add_builtin_id(world, EcsRowSystem_h, "EcsRowSystem");
    add_builtin_id(world, EcsTableSystem_h, "EcsTableSystem");
    add_builtin_id(world, EcsId_h, "EcsId");
    add_builtin_id(world, EcsHidden_h, "EcsHidden");
    add_builtin_id(world, EcsContainer_h, "EcsContainer");
    add_builtin_id(world, EcsRoot_h, "EcsRoot");

    world->component_family = get_builtin_family(world, EcsComponent_h);
    world->family_family = get_builtin_family(world, EcsFamilyComponent_h);
    world->prefab_family = get_builtin_family(world, EcsPrefab_h);
    world->row_system_family = get_builtin_family(world, EcsRowSystem_h);
    world->table_system_family = get_builtin_family(world, EcsTableSystem_h);

    world->deinit_row_system = ecs_new_system(
        world, "EcsDeinitRowSystem", EcsOnRemove,
               "EcsRowSystem", remove_systems);
    assert(world->deinit_row_system != 0);

    world->deinit_table_system = ecs_new_system(
        world, "EcsDeinitTableSystem", EcsOnRemove,
               "EcsTableSystem", remove_systems);
    assert(world->deinit_table_system != 0);

    ecs_stage_add(world, world->deinit_row_system, EcsHidden_h);
    ecs_commit(world, world->deinit_row_system);

    ecs_stage_add(world, world->deinit_table_system, EcsHidden_h);
    ecs_commit(world, world->deinit_table_system);

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

    clean_tables(world);
    clean_families(world);

    ecs_stage_deinit(&world->stage);

    ecs_array_free(world->on_frame_systems);
    ecs_array_free(world->pre_frame_systems);
    ecs_array_free(world->post_frame_systems);
    ecs_array_free(world->on_load_systems);
    ecs_array_free(world->on_store_systems);

    ecs_array_free(world->inactive_systems);
    ecs_array_free(world->on_demand_systems);
    ecs_array_free(world->tasks);
    ecs_array_free(world->fini_tasks);

    ecs_map_free(world->add_systems);
    ecs_map_free(world->remove_systems);
    ecs_map_free(world->set_systems);
    ecs_map_free(world->entity_index);
    ecs_map_free(world->table_index);
    ecs_map_free(world->family_handles);
    ecs_map_free(world->prefab_index);

    free(world);

    return EcsOk;
}

void ecs_dim(
    EcsWorld *world,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_map_set_size(world->entity_index, entity_count);
}

void ecs_dim_family(
    EcsWorld *world,
    EcsEntity type,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, NULL, type, NULL);
        EcsTable *table = ecs_world_get_table(world, NULL, family_id);
        if (table) {
            ecs_array_set_size(&table->rows, &table->row_params, entity_count);
        }
    }
}

EcsEntity ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    EcsIter it = ecs_array_iter(world->table_db, &table_arr_params);

    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        uint32_t offset;

        if ((offset = ecs_table_column_offset(table, EcsId_h)) == -1) {
            continue;
        }

        void *buffer = ecs_array_buffer(table->rows);
        uint32_t count = ecs_array_count(table->rows);
        uint32_t element_size = table->row_params.element_size;
        void *row, *last = ECS_OFFSET(buffer, count * element_size);

        for (row = buffer; row < last; row = ECS_OFFSET(row, element_size)) {
            EcsId *id_ptr = ECS_OFFSET(row, offset);
            if (!strcmp(*id_ptr, id)) {
                return *(EcsEntity*)row;
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

    ecs_stage_merge(world, &world->stage);

    uint32_t i, count = ecs_array_count(world->stage_db);
    EcsStage *buffer = ecs_array_buffer(world->stage_db);
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

void ecs_import(
    EcsWorld *world,
    EcsModuleInitAction module,
    int flags,
    void *handles)
{
    module(world, flags, handles);
}
