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

/** Parse callback that adds component to the components array for a system */
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

/** Parse callback that adds family to family identifier for ecs_family_get */
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
    uint32_t *table_data;
    uint32_t count = ecs_array_count(table->rows);
    if (count) {
        table_data = ecs_array_add(
            &system_data->tables, &system_data->table_params);
    } else {
        table_data = ecs_array_add(
            &system_data->inactive_tables, &system_data->table_params);
    }

    table_data[0] = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

    uint32_t i = 1; /* Offsets start after table index */
    EcsIter it = ecs_array_iter(system_data->components, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        table_data[i] = ecs_table_column_offset(table, h);
        i ++;
    }

    EcsHandle *h = NULL;
    if (system_data->kind == EcsPeriodic || system_data->kind == EcsOnDemand) {
        h = ecs_array_add(&table->periodic_systems, &handle_arr_params);
    } else if (system_data->kind == EcsOnInit) {
        h = ecs_array_add(&table->init_systems, &handle_arr_params);
    } else if (system_data->kind == EcsOnDeinit) {
        h = ecs_array_add(&table->deinit_systems, &handle_arr_params);
    }

    if (h) *h = system;
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

        if (ecs_family_contains(
            world, table->family_id, system_data->family_id))
        {
            add_table(world, system, system_data, table);
        }
    }
}

static
EcsFamily family_from_components(
    EcsWorld *world,
    EcsArray *components)
{
    EcsIter it = ecs_array_iter(components, &handle_arr_params);
    EcsArray *family_array = NULL;
    EcsFamily result = 0;

    while (ecs_iter_hasnext(&it)) {
        EcsHandle *h = ecs_iter_next(&it);
        result = ecs_world_register_family(world, *h, family_array);
        family_array = ecs_map_get(world->family_index, result);
    }

    return result;
}

/* -- Private functions -- */

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

    if (ecs_family_contains(world, table->family_id, system_data->family_id)) {
        add_table(world, system, system_data, table);
    }

    return EcsOk;
}

/** Run subset of the matching entities for a system (used in worker threads) */
void ecs_run_job(
    EcsWorld *world,
    EcsJob *job)
{
    EcsHandle system = job->system;
    EcsSystem *system_data = job->system_data;
    EcsSystemAction action = system_data->action;
    uint32_t table_element_size = system_data->table_params.element_size;
    uint32_t table_index = job->table_index;
    uint32_t start_index = job->start_index;
    uint32_t remaining = job->row_count;
    char *table_buffer = ecs_array_get(
        system_data->tables, &system_data->table_params, table_index);

    EcsRows info = {
        .world = world,
        .system = system
    };

    do {
        EcsTable *table = ecs_array_get(
            world->table_db, &table_arr_params, *(uint32_t*)table_buffer);
        EcsArray *rows = table->rows;
        void *start = ecs_array_get(rows, &table->row_params, start_index);
        uint32_t count = ecs_array_count(rows);
        uint32_t element_size = table->row_params.element_size;

        info.count = count;
        info.element_size = element_size;
        info.columns = ECS_OFFSET(table_buffer, sizeof(uint32_t));
        info.first = ECS_OFFSET(start, sizeof(EcsHandle));

        if (remaining >= count) {
            info.last = ECS_OFFSET(info.first, element_size * count);
            table_buffer += table_element_size;
            start_index = 0;
            remaining -= count;
        } else {
            info.last = ECS_OFFSET(info.first, element_size * remaining);
            remaining = 0;
        }

        action(&info);
    } while (remaining);
}


/** Run system on a single row */
void ecs_system_notify(
    EcsWorld *world,
    EcsHandle system,
    EcsSystem *system_data,
    EcsTable *table,
    uint32_t table_index,
    uint32_t row_index)
{
    EcsSystemAction action = system_data->action;

    EcsRows info = {
        .world = world,
        .system = system,
        .param = NULL
    };

    uint32_t t, table_count = ecs_array_count(system_data->tables);

    for (t = 0; t < table_count; t++) {
        uint32_t *table_data = ecs_array_get(
            system_data->tables, &table_arr_params, t);

        if (*table_data == table_index) {
            EcsArray *rows = table->rows;
            void *row = ecs_array_get(rows, &table->row_params, row_index);
            info.element_size = table->row_params.element_size;
            info.columns = ECS_OFFSET(table_data, sizeof(uint32_t));
            info.first = ECS_OFFSET(row, sizeof(EcsHandle));
            info.last = ECS_OFFSET(info.first, info.element_size);
            action(&info);
            break;
        }
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_system_activate_table(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table,
    bool active)
{
    EcsArray *src_array, *dst_array;
    EcsSystem *system_data = ecs_get(world, system, world->system);
    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

    if (active) {
        src_array = system_data->inactive_tables;
        dst_array = system_data->tables;
    } else {
        src_array = system_data->tables;
        dst_array = system_data->inactive_tables;
    }

    uint32_t count = ecs_array_count(src_array);
    int i;
    for (i = 0; i < count; i ++) {
        uint32_t *index = ecs_array_get(
            src_array, &system_data->table_params, i);
        if (*index == table_index) {
            break;
        }
    }

    if (i == count) {
        abort();
    }

    uint32_t src_count = ecs_array_move_index(
        &dst_array, src_array, &system_data->table_params, i);

    if (active) {
        uint32_t dst_count = ecs_array_count(dst_array);
        if (dst_count == 1 && system_data->enabled) {
            ecs_world_activate_system(world, system, true);
        }
        system_data->tables = dst_array;
    } else {
        if (src_count == 0) {
            ecs_world_activate_system(world, system, false);
        }
        system_data->inactive_tables = dst_array;
    }
}


/* -- Public API -- */

EcsHandle ecs_new_system(
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

    EcsHandle result = ecs_new_w_family(world, world->system_family);

    EcsSystem *system_data = ecs_get(world, result, world->system);
    system_data->action = action;
    system_data->enabled = true;
    system_data->table_params.element_size =
        sizeof(uint32_t) + sizeof(uint32_t) * (count + 1);
    system_data->table_params.move_action = NULL;
    system_data->tables = ecs_array_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->inactive_tables = ecs_array_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->components = ecs_array_new(&handle_arr_params, count);
    system_data->kind = kind;
    system_data->jobs = NULL;

    EcsId *id_data = ecs_get(world, result, world->id);
    id_data->id = id;

    if (parse_components(world, sig, add_component, system_data) != EcsOk) {
        ecs_delete(world, result);
        return 0;
    }

    system_data->family_id = family_from_components(
        world, system_data->components);

    match_tables(world, result, system_data);

    if (kind == EcsPeriodic) {
        EcsHandle *elem;
        if (ecs_array_count(system_data->tables)) {
            elem = ecs_array_add(&world->periodic_systems, &handle_arr_params);
        } else {
            elem = ecs_array_add(&world->inactive_systems, &handle_arr_params);
        }
        *elem = result;
    } else {
        EcsHandle *elem = ecs_array_add(
            &world->other_systems, &handle_arr_params);
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

    if (enabled) {
        if (!system_data->enabled) {
            if (ecs_array_count(system_data->tables)) {
                ecs_world_activate_system(world, system, true);
            }
        }
    } else {
        if (system_data->enabled) {
            if (ecs_array_count(system_data->tables)) {
                ecs_world_activate_system(world, system, false);
            }
        }
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

void ecs_run_system(
    EcsWorld *world,
    EcsHandle system,
    void *param)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    if (!system_data->enabled) {
        return;
    }

    EcsSystemAction action = system_data->action;
    EcsArray *tables = system_data->tables;
    uint32_t table_count = ecs_array_count(tables);
    uint32_t element_size = system_data->table_params.element_size;
    char *table_buffer = ecs_array_buffer(tables);
    char *last = ECS_OFFSET(table_buffer, element_size * table_count);

    EcsRows info = {
        .world = world,
        .system = system,
        .param = param
    };

    for (; table_buffer < last; table_buffer += element_size) {
        EcsTable *table = ecs_array_get(
            world->table_db, &table_arr_params, *(uint32_t*)table_buffer);
        EcsArray *rows = table->rows;
        void *buffer = ecs_array_buffer(rows);

        uint32_t count = ecs_array_count(rows);
        info.count = count;
        info.element_size = table->row_params.element_size;
        info.first = ECS_OFFSET(buffer, sizeof(EcsHandle));
        info.last = ECS_OFFSET(info.first, info.element_size * count);
        info.columns = ECS_OFFSET(table_buffer, sizeof(uint32_t));
        action(&info);
    }
}

EcsHandle ecs_new_family(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    EcsFamily family = 0;

    if (parse_components(world, sig, add_family, &family) != EcsOk) {
        return 0;
    }

    EcsTable *table = ecs_world_get_table(world, family);
    if (table->family_entity) {
        EcsId *id_ptr = ecs_get(world, table->family_entity, world->id);
        if (!id_ptr) {
            abort();
        }

        if (strcmp(id_ptr->id, id)) {
            abort();
        }

        return table->family_entity;
    } else {
        EcsHandle result = ecs_new_w_family(world, world->family_family);
        EcsId *id_ptr = ecs_get(world, result, world->id);
        id_ptr->id = id;
        table->family_entity = result;

        EcsFamily *family_ptr = ecs_get(world, result, world->family);
        *family_ptr = family;

        return result;
    }
}
