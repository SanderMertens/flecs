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
    EcsEntity *component = ecs_lookup(world, component_id);
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
}

static
void match_tables(
    EcsWorld *world,
    EcsSystem *system_data)
{
    EcsIter it = ecs_vector_iter(world->tables, &tables_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        if (ecs_table_has_components(table, system_data->components)) {
            add_table(system_data, table);
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
        add_table(system_data, table);
    }

    return EcsOk;
}

void ecs_system_run(
    EcsEntity *sys)
{
    EcsWorld *world = sys->world;
    EcsSystem *system_data = ecs_get(sys, world->system);
    uint32_t component_count = ecs_array_count(system_data->components);
    void *data[component_count];

    if (system_data->enabled) {
        EcsIter it =
            ecs_vector_iter(system_data->tables, &system_data->tables_params);

        EcsVectorIter iter_data;
        while (ecs_iter_hasnext(&it)) {
            EcsSystemTable *systable = ecs_iter_next(&it);
            EcsTable *table = systable->table;
            uint32_t *offsets = ECS_OFFSET(systable, sizeof(EcsSystemTable));

            EcsIter row_iter =
                _ecs_vector_iter(table->rows, &table->rows_params, &iter_data);

            while (ecs_iter_hasnext(&row_iter)) {
                void *row = ecs_iter_next(&row_iter);
                EcsEntity *e = *(EcsEntity**)row;

                int i;
                for (i = 0; i < component_count; i ++) {
                    data[i] = ECS_OFFSET(row, offsets[i]);
                }

                system_data->action(sys, e, data);
            }
        }
    }
}

EcsEntity *ecs_system_new(
    EcsWorld *world,
    const char *id,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = components_count(sig);
    if (!count) {
        return NULL;
    }

    EcsEntity *result = ecs_new(world, id);
    EcsSystem *system_data = _ecs_add(result, world->system);
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

    if (parse_components(world, system_data, sig) != EcsOk) {
        return NULL;
    }

    match_tables(world, system_data);

    EcsEntity **elem = ecs_vector_add(world->systems, &entityptr_vec_params);
    *elem = result;

    return result;
}

EcsResult _ecs_system_enable(
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
