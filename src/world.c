#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "include/private/reflecs.h"

const EcsArrayParams table_arr_params = {
    .element_size = sizeof(EcsTable)
};

const EcsArrayParams handle_arr_params = {
    .element_size = sizeof(EcsHandle)
};

const EcsArrayParams stage_arr_params = {
    .element_size = sizeof(EcsStage)
};

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
    EcsHandle handle = ecs_new(world, 0);
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
EcsHandle new_builtin_component(
    EcsWorld *world,
    size_t size)
{
    EcsHandle handle = ecs_new(world, 0);
    ecs_add(world, handle, EcsComponent_h);
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
    EcsHandle handle,
    const char *id)
{
    ecs_add(world, handle, EcsId_h);
    ecs_commit(world, handle);
    EcsId *id_data = ecs_get_ptr(world, handle, EcsId_h);

    assert(id_data != NULL);

    id_data->id = (char*)id;
}

/** Obtain family id for specified component + EcsId */
static
EcsFamily get_builtin_family(
    EcsWorld *world,
    EcsHandle component)
{
    EcsFamily family = ecs_family_register(world, NULL, component, NULL);
    EcsArray *family_array = ecs_family_get(world, NULL, family);
    return ecs_family_register(world, NULL, EcsId_h, family_array);
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

    if (world->in_progress && world->threads_running) {
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

    EcsIter it = ecs_array_iter(world->periodic_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, stage, system, result);
    }

    it = ecs_array_iter(world->inactive_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, stage, system, result);
    }

    assert(result != NULL);

    return result;
}


/** Initialize size of new components to 0 */
static
void init_component(
    EcsRows *rows)
{
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsComponent *data = ecs_column(rows, row, 0);
        data->size = 0;
    }
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

    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        ecs_table_free(world, table);
    }

    ecs_array_free(world->table_db);
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
    ecs_array_free(data->columns);
    ecs_array_free(data->components);
    ecs_array_free(data->tables);
    ecs_array_free(data->inactive_tables);
    if (data->jobs) ecs_array_free(data->jobs);
    if (data->refs) ecs_array_free(data->refs);
    data->enabled = false;
}

/** Cleanup resources allocated by table systems */
static
void remove_systems(
    EcsRows *rows)
{
    void *row;
    void *deinit_system_ptr = NULL;
    EcsHandle component = rows->components[0];

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        if (entity != rows->world->deinit_table_system) {
            void *data = ecs_column(rows, row, 0);
            if (component == EcsTableSystem_h) {
                deinit_table_system(data);
            } else {
                deinit_row_system(data);
            }
        } else {
            deinit_system_ptr = ecs_column(rows, row, 0);
        }
    }

    if (deinit_system_ptr) {
        if (component == EcsTableSystem_h) {
            deinit_table_system(deinit_system_ptr);
        } else {
            deinit_row_system(deinit_system_ptr);
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
        abort();
    }
}

/** Get pointer to table data from family id */
EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    uint32_t table_index = ecs_map_get64(world->table_index, family_id);
    if (!table_index && world->in_progress && world->threads_running) {
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
    EcsHandle system,
    bool active)
{
    EcsArray *src_array, *dst_array;

    if (active) {
        src_array = world->inactive_systems;
        dst_array = world->periodic_systems;
    } else {
        src_array = world->periodic_systems;
        dst_array = world->inactive_systems;
    }

    uint32_t count = ecs_array_count(src_array);
    int i;
    for (i = 0; i < count; i ++) {
        EcsHandle *h = ecs_array_get(
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
        world->periodic_systems = dst_array;
    } else {
        world->inactive_systems = dst_array;
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

/** Print components in a family. Used by ecs_dump */
void ecs_family_dump(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsArray *family = ecs_family_get(world, stage, family_id);

    EcsIter it = ecs_array_iter(family, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        if (id) {
            printf("%s ", id->id);
        } else {
            printf("%" PRIu64 " ", h);
        }
    }
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
        abort();
        return NULL;
    }
}

/* -- Public functions -- */

void ecs_dump(
    EcsWorld *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    printf("Tables (%d)\n", ecs_map_count(world->table_index));
    EcsIter it = ecs_map_iter(world->table_index);
    while (ecs_iter_hasnext(&it)) {
        uint32_t table_index = (uintptr_t)ecs_iter_next(&it);
        EcsTable *table =
            ecs_array_get(world->table_db, &table_arr_params, table_index - 1);

        uint32_t count = ecs_array_count(table->rows);
        uint32_t size = ecs_array_size(table->rows);
        printf("[%10u] %u rows, %.2fKb, columns: ",
          table->family_id,
          count,
          (size * table->row_params.element_size) / 1000.0);
        ecs_family_dump(world, NULL, table->family_id);
        printf("\n");
    }

    printf("\nPeriodic systems\n");
    it = ecs_array_iter(world->periodic_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        printf(" - %s\n", id->id);
    }

    it = ecs_array_iter(world->inactive_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        printf(" - %s [inactive]\n", id->id);
    }

    printf("\nInit systems\n");
    it = ecs_map_iter(world->add_systems);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = ecs_map_next(&it, NULL);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        printf(" - %s\n", id->id);
    }

    printf("\nDeinit systems\n");
    it = ecs_map_iter(world->remove_systems);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = ecs_map_next(&it, NULL);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        printf(" - %s\n", id->id);
    }

    printf("\nEntities (%d)\n", ecs_map_count(world->entity_index));
    it = ecs_map_iter(world->entity_index);
    while (ecs_iter_hasnext(&it)) {
        EcsRow row = ecs_to_row((uintptr_t)ecs_iter_next(&it));
        EcsTable *table = ecs_world_get_table(world, NULL, row.family_id);
        void *row_ptr = ecs_table_get(table, table->rows, row.index);
        EcsHandle h = *(EcsHandle*)row_ptr;

        printf("[%u, %u] %" PRIu64 " - ", row.family_id, row.index, h);
        EcsId *id = ecs_get_ptr(world, h, EcsId_h);
        if (id) {
            printf("%s", id->id);
        } else {
            printf("<no id>");
        }
        printf("\n");
    }
}

EcsWorld *ecs_init(void) {
    EcsWorld *world = malloc(sizeof(EcsWorld));
    world->magic = ECS_WORLD_MAGIC;

    world->table_db = ecs_array_new(
        &table_arr_params, ECS_WORLD_INITIAL_TABLE_COUNT);
    world->periodic_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    world->inactive_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);

    world->add_systems = ecs_map_new(ECS_WORLD_INITIAL_INIT_SYSTEM_COUNT);
    world->remove_systems = ecs_map_new(ECS_WORLD_INITIAL_DEINIT_SYSTEM_COUNT);
    world->set_systems = ecs_map_new(ECS_WORLD_INITIAL_SET_SYSTEM_COUNT);
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
    world->last_handle = 0;

    ecs_stage_init(&world->stage);

    bootstrap_component(world);
    assert_func(new_builtin_component(world, sizeof(EcsFamilyComponent)) == EcsFamilyComponent_h);
    assert_func(new_builtin_component(world, 0) == EcsPrefab_h);
    assert_func(new_builtin_component(world, sizeof(EcsRowSystem)) == EcsRowSystem_h);
    assert_func(new_builtin_component(world, sizeof(EcsTableSystem)) == EcsTableSystem_h);
    assert_func(new_builtin_component(world, sizeof(EcsId)) == EcsId_h);

    add_builtin_id(world, EcsComponent_h, "EcsComponent");
    add_builtin_id(world, EcsFamilyComponent_h, "EcsFamilyComponent");
    add_builtin_id(world, EcsPrefab_h, "EcsPrefab");
    add_builtin_id(world, EcsRowSystem_h, "EcsRowSystem");
    add_builtin_id(world, EcsTableSystem_h, "EcsTableSystem");
    add_builtin_id(world, EcsId_h, "EcsId");

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

    assert_func(ecs_new_system(
        world, "EcsInitComponent", EcsOnAdd, "EcsComponent", init_component)
        != 0);

    return world;
}

EcsResult ecs_fini(
    EcsWorld *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }

    clean_tables(world);
    clean_families(world);

    ecs_stage_deinit(&world->stage);

    ecs_array_free(world->periodic_systems);
    ecs_array_free(world->inactive_systems);

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
    EcsHandle type,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, NULL, type);
        EcsTable *table = ecs_world_get_table(world, NULL, family_id);
        if (table) {
            ecs_array_set_size(&table->rows, &table->row_params, entity_count);
        }
    }
}

EcsHandle ecs_lookup(
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
            if (!strcmp(id_ptr->id, id)) {
                return *(EcsHandle*)row;
            }
        }
    }

    return 0;
}

void ecs_progress(
    EcsWorld *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    uint32_t count = ecs_array_count(world->periodic_systems);
    if (count) {
        int i;
        EcsHandle *buffer = ecs_array_buffer(world->periodic_systems);
        bool has_threads = ecs_array_count(world->worker_threads) != 0;

        world->in_progress = true;

        if (has_threads) {
            bool valid_schedule = world->valid_schedule;
            for (i = 0; i < count; i ++) {
                if (!valid_schedule) {
                    ecs_schedule_jobs(world, buffer[i]);
                }
                ecs_prepare_jobs(world, buffer[i]);
            }
            ecs_run_jobs(world);
            world->valid_schedule = true;
        } else {
            for (i = 0; i < count; i ++) {
                ecs_run_system(world, buffer[i], NULL);
            }
        }

        world->in_progress = false;

        if (world->auto_merge) {
            ecs_merge(world);
        }
    }
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
