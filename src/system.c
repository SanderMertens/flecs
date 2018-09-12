#include <ctype.h>
#include "reflecs.h"

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

static
const char *skip_space(const char *ptr) {
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

static
EcsResult add_component(
    EcsWorld *world,
    EcsSystem *system_data,
    const char *component_id)
{
    EcsEntity *component = ecs_world_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    EcsEntity **e;
    ecs_array_add(system_data->components, &entityptr_arr_params, &e);

    *e = component;

    return EcsOk;
}

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

static
void add_table(
    EcsEntity *entity,
    EcsSystem *system_data,
    EcsTable *table)
{
    EcsSystemTable *systable =
        ecs_vector_add(system_data->tables, &system_data->tables_params);

    uint32_t *offsets = ECS_OFFSET(systable, sizeof(EcsSystemTable));
    systable->table = table;

    uint32_t i = 0;
    EcsIter it = ecs_array_iter(system_data->components, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
        int32_t column = ecs_table_find_column(table, e);
        offsets[i] = (uintptr_t)ecs_table_column(table, NULL, column);
        i ++;
    }

    if (system_data->kind == EcsOnInit) {
        ecs_table_add_on_init(table, entity);
    } else if (system_data->kind == EcsOnDeinit) {
        ecs_table_add_on_deinit(table, entity);
    }
}

static
void match_tables(
    EcsWorld *world,
    EcsEntity *entity,
    EcsSystem *system_data)
{
    EcsIter it = ecs_vector_iter(world->tables, &tables_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        if (ecs_table_has_components(table, system_data->components)) {
            add_table(entity, system_data, table);
        }
    }
}

EcsResult ecs_system_notify_create_table(
    EcsEntity *entity,
    EcsTable *table)
{
    EcsWorld *world = entity->world;
    EcsSystem *system_data = ecs_get(entity, world->system);
    if (!system_data) {
        return EcsError;
    }

    if (ecs_table_has_components(table, system_data->components)) {
        add_table(entity, system_data, table);
    }

    return EcsOk;
}

static
void ecs_system_do_row(
    EcsEntity *sys,
    EcsSystem *system_data,
    EcsSystemTable *systable,
    void *row)
{
    uint32_t *offsets = ECS_OFFSET(systable, sizeof(EcsSystemTable));
    uint32_t component_count = ecs_array_count(system_data->components);
    void *data[component_count];

    EcsEntity *e = *(EcsEntity**)row;

    int i;
    for (i = 0; i < component_count; i ++) {
        data[i] = ECS_OFFSET(row, offsets[i]);
    }

    system_data->action(sys, e, data);
}

void ecs_system_run(
    EcsEntity *sys)
{
    EcsWorld *world = sys->world;
    EcsSystem *system_data = ecs_get(sys, world->system);

    if (system_data->kind != EcsPeriodic) {
        return;
    }

    if (system_data->enabled) {
        EcsIter it =
            ecs_vector_iter(system_data->tables, &system_data->tables_params);

        EcsVectorIter iter_data;
        while (ecs_iter_hasnext(&it)) {
            EcsSystemTable *systable = ecs_iter_next(&it);
            EcsTable *table = systable->table;

            EcsIter row_iter = _ecs_vector_iter(table->rows, &table->rows_params, &iter_data);
            while (ecs_iter_hasnext(&row_iter)) {
                void *row = ecs_iter_next(&row_iter);
                ecs_system_do_row(sys, system_data, systable, row);
            }
        }
    }
}

void ecs_system_notify(
    EcsEntity *sys,
    EcsTable *table,
    EcsEntity *entity)
{
    EcsWorld *world = sys->world;
    EcsSystem *system_data = ecs_get(sys, world->system);
    EcsIter it =
        ecs_vector_iter(system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&it)) {
        EcsSystemTable *systable = ecs_iter_next(&it);
        if (systable->table == table) {
            ecs_system_do_row(sys, system_data, systable, entity->row);
            break;
        }
    }
}

EcsEntity *ecs_system_new(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = components_count(sig);
    if (!count) {
        return NULL;
    }

    EcsEntity *result = ecs_new(world, id);
    EcsSystem *system_data = ecs_add(result, world->system);
    system_data->action = action;
    system_data->enabled = true;
    system_data->tables_params.chunk_count = REFLECS_INITIAL_CHUNK_COUNT;
    system_data->tables_params.element_size =
        sizeof(EcsSystemTable) + sizeof(uint32_t) * count;
    system_data->tables_params.compare_action = NULL;
    system_data->tables_params.move_action = NULL;
    system_data->tables_params.ctx = NULL;
    system_data->tables = ecs_vector_new(&system_data->tables_params);
    system_data->components = ecs_array_new(count, &entityptr_arr_params);
    system_data->kind = kind;

    if (parse_components(world, system_data, sig) != EcsOk) {
        return NULL;
    }

    match_tables(world, result, system_data);

    EcsEntity **elem = ecs_vector_add(world->systems, &entityptr_vec_params);
    *elem = result;

    return result;
}

EcsResult ecs_system_enable(
    EcsEntity *entity,
    bool enabled)
{
    EcsWorld *world = entity->world;
    EcsSystem *system_data = ecs_get(entity, world->system);
    if (!system_data) {
        return EcsError;
    }

    system_data->enabled = enabled;

    return EcsOk;
}

bool ecs_system_is_enabled(
    EcsEntity *entity)
{
    EcsWorld *world = entity->world;
    EcsSystem *system_data = ecs_get(entity, world->system);
    return system_data->enabled;
}
