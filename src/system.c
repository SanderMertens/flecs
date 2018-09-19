#include <ctype.h>
#include "include/private/reflecs.h"

/** Count components in a signature */
static
uint32_t components_count(
    const char *sig)
{
    const char *ptr = sig;
    uint32_t count = 1;

    while ((ptr = strchr(ptr + 1, ','))) {
        count ++;
    }

    return count;
}

/** Skip spaces when parsing signature */
static
const char *skip_space(const char *ptr) {
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

/** Add component to the components array for a system */
static
EcsResult add_component(
    EcsWorld *world,
    EcsSystem *system_data,
    const char *component_id)
{
    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    EcsHandle *e;
    ecs_array_add(system_data->components, &handle_arr_params, &e);

    *e = component;

    return EcsOk;
}

/** Parse system signature */
static
EcsResult parse_components(
    EcsWorld *world,
    EcsSystem *system_data,
    const char *sig)
{
    size_t len = strlen(sig);
    const char *ptr;
    char ch, *bptr, *buffer = malloc(len + 1);

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (ch == ',' || ch == '\0') {
            if (bptr == buffer) {
                goto error;
            }

            *bptr = '\0';
            if (add_component(world, system_data, buffer) != EcsOk) {
                goto error;
            }

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;
        }
    }

    free(buffer);
    return EcsOk;
error:
    free(buffer);
    return EcsError;
}

/** Add table to system, compute offsets for system components in table rows */
static
void add_table(
    EcsHandle system,
    EcsSystem *system_data,
    EcsTable *table)
{
    EcsSystemTable *systable =
        ecs_vector_add(system_data->tables, &system_data->tables_params);

    uint32_t *offsets = ECS_OFFSET(systable, sizeof(EcsSystemTable));
    systable->table = table;

    uint32_t i = 0;
    EcsIter it = ecs_array_iter(system_data->components, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        int32_t column = ecs_table_find_column(table, h);
        offsets[i] = (uintptr_t)ecs_table_column(table, NULL, column);
        i ++;
    }

    if (system_data->kind == EcsOnInit) {
        ecs_table_add_on_init(table, system);
    } else if (system_data->kind == EcsOnDeinit) {
        ecs_table_add_on_deinit(table, system);
    }
}


/** Match existing tables against system (table is created before system) */
static
void match_tables(
    EcsWorld *world,
    EcsHandle system,
    EcsSystem *system_data)
{
    EcsIter it = ecs_vector_iter(world->tables, &tables_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        if (ecs_table_has_components(table, system_data->components)) {
            add_table(system, system_data, table);
        }
    }
}

/** Match new table against system (table is created after system) */
EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    if (!system_data) {
        return EcsError;
    }

    if (ecs_table_has_components(table, system_data->components)) {
        add_table(system, system_data, table);
    }

    return EcsOk;
}

/** Invoke system action for a table row */
static
void ecs_system_visit_row(
    EcsWorld *world,
    EcsHandle system,
    EcsSystem *system_data,
    EcsSystemTable *systable,
    void *row,
    void *param)
{
    uint32_t *offsets = ECS_OFFSET(systable, sizeof(EcsSystemTable));
    uint32_t component_count = ecs_array_count(system_data->components);
    void *data[component_count];
    EcsInfo info;

    EcsHandle entity = *(EcsHandle*)row;

    int i;
    for (i = 0; i < component_count; i ++) {
        data[i] = ECS_OFFSET(row, offsets[i]);
    }

    info.world = world;
    info.system = system;
    info.entity = entity;
    info.param = param;
    system_data->action(data, &info);
}

/** Run system against all rows in all matching tables */
void ecs_run_system(
    EcsWorld *world,
    EcsHandle system,
    void *param)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);

    if (system_data->enabled) {
        EcsIter it =
            ecs_vector_iter(system_data->tables, &system_data->tables_params);

        while (ecs_iter_hasnext(&it)) {
            EcsSystemTable *systable = ecs_iter_next(&it);
            EcsTable *table = systable->table;

            EcsVectorChunk *chunk = NULL;
            size_t element_size = table->rows_params.element_size;
            uint32_t chunk_count = table->rows_params.chunk_count;
            uint32_t row_count = ecs_vector_count(table->rows);
            uint32_t count = 0, i = 0;
            while ((chunk = ecs_vector_get_next_chunk(table->rows, chunk))) {
                void *row, *buffer = ecs_vector_chunk_get_buffer(chunk);
                for (row = buffer, i = 0;
                     i < chunk_count && count < row_count;
                     i ++, count ++)
                {
                    ecs_system_visit_row(
                        world, system, system_data, systable, row, param);
                    row = (void*)((uintptr_t)row + element_size);
                }
            }
        }
    }
}

void ecs_run_job(
    EcsWorld *world,
    EcsJob *job)
{
    EcsHandle system = job->system;
    EcsSystem *system_data = job->system_data;
    uint32_t table_index = job->table_index;
    uint32_t chunk_index = job->chunk_index;
    uint32_t job_count = job->total_rows;
    EcsVectorChunk *chunk = job->chunk;
    bool first = true;

    EcsIter it =
        ecs_vector_iter(system_data->tables, &system_data->tables_params);

    int i;
    for (i = 0; i < table_index; i ++) {
        if (!ecs_iter_hasnext(&it)) {
            break;
        }

        ecs_iter_next(&it);
    }

    while (ecs_iter_hasnext(&it)) {
        EcsSystemTable *systable = ecs_iter_next(&it);
        EcsTable *table = systable->table;

        size_t element_size = table->rows_params.element_size;
        uint32_t chunk_count = table->rows_params.chunk_count;
        uint32_t table_count = ecs_vector_count(table->rows);
        uint32_t table_i = 0, chunk_i, job_i = 0;
        while ((chunk = ecs_vector_get_next_chunk(table->rows, chunk))) {
            void *row, *buffer = ecs_vector_chunk_get_buffer(chunk);

            if (first) {
                row = ECS_OFFSET(buffer, element_size * chunk_index);
                chunk_i = chunk_index;
                first = false;
            } else {
                row = buffer;
                chunk_i = 0;
            }

            for (;
              chunk_i < chunk_count && table_i < table_count && job_i < job_count;
              chunk_i ++, table_i ++, job_i ++)
            {
                ecs_system_visit_row(
                    world, system, system_data, systable, row, NULL);

                row = (void*)((uintptr_t)row + element_size);
            }
        }
    }
}

/** Run system on a single row */
void ecs_system_notify(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table,
    EcsEntity *entity)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    EcsIter it =
        ecs_vector_iter(system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&it)) {
        EcsSystemTable *systable = ecs_iter_next(&it);
        if (systable->table == table) {
            ecs_system_visit_row(
                world, system, system_data, systable, entity->row, NULL);
            break;
        }
    }
}


/* -- Public API -- */

EcsHandle ecs_system_new(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = components_count(sig);
    if (!count) {
        return 0;
    }

    EcsHandle result = ecs_new(world);
    ecs_add(world, result, world->system);
    ecs_add(world, result, world->id);

    if (ecs_commit(world, result) != EcsOk) {
        ecs_delete(world, result);
        return 0;
    }

    EcsSystem *system_data = ecs_get(world, result, world->system);
    system_data->action = action;
    system_data->enabled = true;
    system_data->tables_params.chunk_count = REFLECS_TABLES_CHUNK_COUNT;
    system_data->tables_params.element_size =
        sizeof(EcsSystemTable) + sizeof(uint32_t) * count;
    system_data->tables_params.compare_action = NULL;
    system_data->tables_params.move_action = NULL;
    system_data->tables_params.ctx = NULL;
    system_data->tables = ecs_vector_new(&system_data->tables_params);
    system_data->jobs = NULL;
    system_data->components = ecs_array_new(count, &handle_arr_params);
    system_data->kind = kind;

    EcsId *id_data = ecs_get(world, result, world->id);
    id_data->id = id;

    if (parse_components(world, system_data, sig) != EcsOk) {
        ecs_delete(world, result);
        return 0;
    }

    match_tables(world, result, system_data);

    if (kind == EcsPeriodic) {
        EcsHandle *elem = ecs_vector_add(world->periodic_systems, &handle_vec_params);
        *elem = result;
    } else {
        EcsHandle *elem = ecs_vector_add(world->other_systems, &handle_vec_params);
        *elem = result;
    }

    return result;
}

EcsResult ecs_enable(
    EcsWorld *world,
    EcsHandle system,
    bool enabled)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    if (!system_data) {
        return EcsError;
    }

    system_data->enabled = enabled;

    return EcsOk;
}

bool ecs_is_enabled(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    if (system_data) {
        return system_data->enabled;
    } else {
        return true;
    }
}
