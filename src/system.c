#include <ctype.h>
#include <string.h>
#include "include/private/reflecs.h"

typedef EcsResult (*parse_action)(
    EcsWorld *world,
    const char *component,
    void *ctx);

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
    const char *component_id,
    void *data)
{
    EcsSystem *system_data = data;
    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    EcsHandle *e = ecs_array_add(&system_data->components, &handle_arr_params);

    *e = component;

    return EcsOk;
}

static
EcsResult add_family(
    EcsWorld *world,
    const char *component_id,
    void *data)
{
    EcsFamily *family_id = data;
    EcsArray *family = ecs_map_get(world->family_index, *(EcsFamily*)family_id);

    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    *family_id = ecs_world_register_family(world, component, family);

    return EcsOk;
}


/** Parse system signature */
static
EcsResult parse_components(
    EcsWorld *world,
    const char *sig,
    parse_action action,
    void *ctx)
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
            if (action(world, buffer, ctx) != EcsOk) {
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
    EcsWorld *world,
    EcsHandle system,
    EcsSystem *system_data,
    EcsTable *table)
{
    uint32_t *table_data =
        ecs_array_add(&system_data->tables, &system_data->table_params);

    table_data[0] = ecs_array_get_index(world->table_db, &table_arr_params, table);

    uint32_t i = 1; /* Offsets start after table index */
    EcsIter it = ecs_array_iter(system_data->components, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        table_data[i] = ecs_table_column_offset(table, h);
        i ++;
    }

    if (system_data->kind == EcsOnInit) {
        EcsHandle *h = ecs_array_add(&table->init_systems, &handle_arr_params);
        *h = system;
    } else if (system_data->kind == EcsOnDeinit) {
        EcsHandle *h = ecs_array_add(&table->deinit_systems, &handle_arr_params);
        *h = system;
    }
}


/** Match existing tables against system (table is created before system) */
static
void match_tables(
    EcsWorld *world,
    EcsHandle system,
    EcsSystem *system_data)
{
    EcsIter it = ecs_array_iter(world->table_db, &table_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        if (ecs_table_has_components(table, system_data->components)) {
            add_table(world, system, system_data, table);
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
        add_table(world, system, system_data, table);
    }

    return EcsOk;
}

/** Run system against all rows in all matching tables */
void ecs_run_system(
    EcsWorld *world,
    EcsHandle system,
    void *param)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    EcsSystemAction action = system_data->action;
    uint32_t component_count = ecs_array_count(system_data->components);
    void *data[component_count];

    EcsInfo info = {
        .world = world,
        .system = system,
        .param = param
    };

    if (system_data->enabled) {
        EcsIter it =
            ecs_array_iter(system_data->tables, &system_data->table_params);

        while (ecs_iter_hasnext(&it)) {
            uint32_t *table_data = ecs_iter_next(&it);
            EcsTable *table = ecs_array_get(
                world->table_db, &table_arr_params, table_data[0]);

            uint32_t i, count = ecs_array_count(table->rows);
            info.element_size = table->row_params.element_size;
            info.buffer = ecs_array_buffer(table->rows);

            for (i = 0; i < count; i ++) {
                action(&info);
                info.index ++;
            }
        }
    }
}

/** Run system on a single row */
void ecs_system_notify(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table,
    EcsRow *row_data)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    EcsSystemAction action = system_data->action;

    EcsInfo info = {
        .world = world,
        .system = system,
        .param = NULL
    };

    EcsIter it =
        ecs_array_iter(system_data->tables, &system_data->table_params);

    while (ecs_iter_hasnext(&it)) {
        uint32_t *table_data = ecs_iter_next(&it);
        EcsTable *table_el = ecs_array_get(world->table_db, &table_arr_params, table_data[0]);
        if (table_el == table) {
            info.buffer = ecs_array_buffer(table->rows);
            info.index = row_data->index;
            info.element_size = table->row_params.element_size;
            action(&info);
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

    EcsHandle result = ecs_new(world, 0);
    ecs_stage(world, result, world->system);
    ecs_stage(world, result, world->id);

    if (ecs_commit(world, result) != EcsOk) {
        ecs_delete(world, result);
        return 0;
    }

    EcsSystem *system_data = ecs_get(world, result, world->system);
    system_data->action = action;
    system_data->enabled = true;
    system_data->table_params.element_size = sizeof(uint32_t) * (count + 1);
    system_data->table_params.move_action = NULL;
    system_data->tables = ecs_array_new(&system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    /*system_data->jobs = NULL;*/
    system_data->components = ecs_array_new(&handle_arr_params, count);
    system_data->kind = kind;

    EcsId *id_data = ecs_get(world, result, world->id);
    id_data->id = id;

    if (parse_components(world, sig, add_component, system_data) != EcsOk) {
        ecs_delete(world, result);
        return 0;
    }

    match_tables(world, result, system_data);

    if (kind == EcsPeriodic) {
        EcsHandle *elem = ecs_array_add(&world->periodic_systems, &handle_arr_params);
        *elem = result;
    } else {
        EcsHandle *elem = ecs_array_add(&world->other_systems, &handle_arr_params);
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

EcsFamily ecs_family_get(
    EcsWorld *world,
    const char *sig)
{
    EcsFamily family = 0;

    if (parse_components(world, sig, add_family, &family) != EcsOk) {
        return 0;
    }

    return family;
}
