#include <string.h>
#include <inttypes.h>
#include "include/private/reflecs.h"

const EcsArrayParams table_arr_params = {
    .element_size = sizeof(EcsTable)
};

const EcsArrayParams handle_arr_params = {
    .element_size = sizeof(EcsHandle)
};

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(EcsHandle*)p1 - *(EcsHandle*)p2;
}

/** Hash array of handles */
static
uint32_t hash_handle_array(
    EcsHandle* array,
    uint32_t count)
{
    uint32_t hash = 0;
    int i;
    for (i = 0; i < count; i ++) {
        ecs_hash(&array[i], sizeof(EcsHandle), &hash);
    }
    return hash;
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
    EcsArray *family = ecs_map_get(world->family_index, family_id);
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
EcsResult bootstrap_component(
    EcsWorld *world)
{
    EcsHandle handle = ecs_new(world, 0);
    ecs_add(world, handle, handle);
    uint64_t family_id = ecs_map_get64(world->add_stage, handle);

    world->component = handle;

    ecs_world_register_family(world, handle, NULL);
    bootstrap_component_table(world, family_id);

    if (ecs_commit(world, world->component) != EcsOk) {
        return EcsError;
    }

    EcsComponent *type_data = ecs_get(world, handle, handle);
    if (!type_data) {
        return EcsError;
    }

    type_data->size = sizeof(EcsComponent);

    return EcsOk;
}

/** Generic function for initializing built-in components */
static
EcsHandle init_component(
    EcsWorld *world,
    size_t size)
{
    EcsHandle handle = ecs_new(world, 0);
    ecs_add(world, handle, world->component);
    EcsComponent *component_data = ecs_get(world, handle, world->component);
    if (!component_data) {
        abort();
    }

    component_data->size = size;

    return handle;
}

/** Generic function to add identifier to builtin entities */
static
void init_id(
    EcsWorld *world,
    EcsHandle handle,
    const char *id)
{
    ecs_add(world, handle, world->id);
    EcsId *id_data = ecs_get(world, handle, world->id);
    if (!id_data) {
        abort();
    }

    id_data->id = (char*)id;
}

/** Create a new table and register it with the world and systems */
static
EcsTable* ecs_world_create_table(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsTable *result = ecs_array_add(&world->table_db, &table_arr_params);
    result->family_id = family_id;

    if (ecs_table_init(world, result) != EcsOk) {
        return NULL;
    }

    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, result);
    ecs_map_set64(world->table_index, family_id, table_index + 1);

    EcsIter it = ecs_array_iter(world->periodic_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, result);
    }

    it = ecs_array_iter(world->inactive_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, result);
    }

    it = ecs_array_iter(world->other_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, result);
    }

    return result;
}

/** Print components in a family. Used by ecs_dump */
static
void family_print(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsArray *family = ecs_map_get(world->family_index, family_id);

    EcsIter it = ecs_array_iter(family, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsId *id = ecs_get(world, h, world->id);
        if (id) {
            printf("%s ", id->id);
        }
    }
}

static
EcsFamily register_family_from_buffer(
    EcsWorld *world,
    EcsHandle *buf,
    uint32_t count)
{
    EcsFamily new_id = hash_handle_array(buf, count);
    EcsArray *new_array = ecs_map_get(world->family_index, new_id);

    if (!new_array) {
        new_array = ecs_array_new_from_buffer(&handle_arr_params, count, buf);
        ecs_map_set(world->family_index, new_id,  new_array);
    }

    return new_id;
}


/* -- Private functions -- */

/** Register a new family, optionally extending from existing family */
EcsFamily ecs_world_register_family(
    EcsWorld *world,
    EcsHandle to_add,
    EcsArray *set)
{
    uint32_t stage_hash = 0;
    uint32_t count = ecs_array_count(set);
    EcsHandle new_set[count + 1];
    void *new_buffer = new_set;

    if (to_add) {
        void *buffer = ecs_array_buffer(set);
        if (count) {
            memcpy(new_set, buffer, sizeof(EcsHandle) * count);
        }
        new_set[count] = to_add;
        qsort(new_set, count + 1, sizeof(EcsHandle), compare_handle);
        stage_hash = hash_handle_array(new_set, count + 1);
        count ++;
    } else if (set) {
        void *buffer = ecs_array_buffer(set);
        stage_hash = hash_handle_array(buffer, count);
        new_buffer = buffer;
    } else {
        return 0;
    }

    return register_family_from_buffer(world, new_buffer, count);
}

/** Merge families (ordered sets of components) */
EcsFamily ecs_world_merge_families(
    EcsWorld *world,
    EcsFamily cur_id,
    EcsFamily to_add_id,
    EcsFamily to_remove_id)
{
    EcsArray *arr_cur = ecs_map_get(world->family_index, cur_id);
    EcsArray *to_add = NULL, *to_del = NULL;
    EcsHandle *buf_add = NULL, *buf_del = NULL, *buf_cur = NULL;
    EcsHandle cur = 0, add = 0, del = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    if (to_remove_id) {
        to_del = ecs_map_get(world->family_index, to_remove_id);
        del_count = ecs_array_count(to_del);
        buf_del = ecs_array_buffer(to_del);
        del = buf_del[0];
    } else {
        if (cur_id && !to_add_id) {
            return cur_id;
        } else if (to_add_id && !cur_id) {
            return to_add_id;
        }
    }

    if (cur) {
        cur_count = ecs_array_count(arr_cur);
        buf_cur = ecs_array_buffer(arr_cur);
        cur = buf_cur[0];
    }

    if (to_add_id) {
        to_add = ecs_map_get(world->family_index, to_add_id);
        add_count = ecs_array_count(to_add);
        buf_add = ecs_array_buffer(to_add);
        add = buf_add[0];
    }

    EcsHandle buf_new[cur_count + add_count];
    uint32_t new_count = 0, prev_count = 0;

    do {
        prev_count = new_count;

        if (del && new_count) {
            EcsFamily last = buf_new[new_count - 1];
            if (last == del) {
                prev_count = -- new_count;
                i_del ++;
                del = i_del < del_count ? buf_del[i_del] : 0;
            } else if (last > del) {
                do {
                    i_del ++;
                    del = i_del < del_count ? buf_del[i_del] : 0;
                } while (del && last > del);
            }
        }

        if (add && add < cur) {
            buf_new[new_count] = add;
            new_count ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
        } else if (cur && cur < add) {
            buf_new[new_count] = cur;
            new_count ++;
            i_cur ++;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        } else {
            buf_new[new_count] = add;
            new_count ++;
            i_cur ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        }
    } while (prev_count != new_count);

    if (new_count) {
        return register_family_from_buffer(world, buf_new, new_count);
    } else {
        return 0;
    }
}

/** Get pointer to table data from family id */
EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsFamily family_id)
{
    uint32_t table_index = ecs_map_get64(world->table_index, family_id);
    if (table_index) {
        return ecs_array_get(
            world->table_db, &table_arr_params, table_index - 1);
    } else {
        return ecs_world_create_table(world, family_id);
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


/* -- Public functions -- */

void ecs_dump(
    EcsWorld *world)
{
    printf("Tables (%d)\n", ecs_map_count(world->table_index));
    EcsIter it = ecs_map_iter(world->table_index);
    while (ecs_iter_hasnext(&it)) {
        uint32_t table_index = (uintptr_t)ecs_iter_next(&it);
        EcsTable *table =
            ecs_array_get(world->table_db, &table_arr_params, table_index - 1);

        uint32_t count = ecs_array_count(table->rows);
        uint32_t size = ecs_array_size(table->rows);
        printf("[%u] Rows: %u, Components: ", table->family_id, count);
        family_print(world, table->family_id);
        printf(" - %.2fKb", (size * table->row_params.element_size) / 1000.0);
        printf("\n");
    }

    printf("\nEntities (%d)\n", ecs_map_count(world->entity_index));
    it = ecs_map_iter(world->entity_index);
    while (ecs_iter_hasnext(&it)) {
        EcsRow row = ecs_to_row((uintptr_t)ecs_iter_next(&it));
        EcsTable *table = ecs_world_get_table(world, row.family_id);
        void *row_ptr = ecs_table_get(table, row.index);
        EcsHandle h = *(EcsHandle*)row_ptr;

        printf("[%u, %u] %" PRIu64 " - ", row.family_id, row.index, h);
        EcsId *id = ecs_get(world, h, world->id);
        if (id) {
            printf("%s", id->id);
        } else {
            printf("...");
        }
        printf("\n");
    }
}

EcsWorld *ecs_init(void) {
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->table_db = ecs_array_new(
        &table_arr_params, ECS_WORLD_INITIAL_TABLE_COUNT);
    result->periodic_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    result->inactive_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    result->other_systems = ecs_array_new(
        &handle_arr_params, ECS_WORLD_INITIAL_OTHER_SYSTEM_COUNT);

    result->worker_threads = NULL;
    result->jobs_finished = 0;
    result->threads_running = 0;
    result->valid_schedule = false;
    result->quit_workers = false;

    result->entity_index = ecs_map_new(ECS_WORLD_INITIAL_ENTITY_COUNT);
    result->table_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->family_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->add_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    result->remove_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    result->component = 0;
    result->system = 0;
    result->id = 0;
    result->last_handle = 0;

    bootstrap_component(result);

    result->system = init_component(result, sizeof(EcsSystem));
    result->id = init_component(result, sizeof(EcsId));

    init_id(result, result->component, "EcsComponent");
    init_id(result, result->system, "EcsSystem");
    init_id(result, result->id, "EcsId");

    return result;
}

EcsResult ecs_fini(
    EcsWorld *world)
{
    ecs_array_free(world->table_db);
    ecs_map_free(world->entity_index);
    ecs_map_free(world->table_index);
    ecs_map_free(world->family_index);
    ecs_map_free(world->add_stage);
    ecs_map_free(world->remove_stage);
    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }
    free(world);
    return EcsOk;
}

void ecs_dim(
    EcsWorld *world,
    uint32_t entity_count)
{
    ecs_map_set_size(world->entity_index, entity_count);
}

void ecs_dim_family(
    EcsWorld *world,
    EcsFamily family_id,
    uint32_t entity_count)
{
    EcsTable *table = ecs_world_get_table(world, family_id);
    if (table) {
        ecs_array_set_size(&table->rows, &table->row_params, entity_count);
    }
}

EcsHandle ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    EcsIter it = ecs_array_iter(world->table_db, &table_arr_params);
    EcsHandle id_component = world->id;

    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);

        if (ecs_table_column_offset(table, id_component) == -1) {
            continue;
        }

        EcsArrayIter iter_data;
        EcsIter row_it = _ecs_array_iter(
            table->rows, &table->row_params, &iter_data);

        while (ecs_iter_hasnext(&row_it)) {
            void *row = ecs_iter_next(&row_it);
            EcsHandle h = *(EcsHandle*)row;
            EcsId *id_ptr = ecs_get(world, h, id_component);
            if (!strcmp(id_ptr->id, id)) {
                return h;
            }
        }
    }

    return 0;
}

void ecs_progress(
    EcsWorld *world)
{
    uint32_t count = ecs_array_count(world->periodic_systems);
    if (count) {
        int i;
        EcsHandle *buffer = ecs_array_buffer(world->periodic_systems);
        bool has_threads = ecs_array_count(world->worker_threads) != 0;

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
    }
}

void* ecs_get_context(
    EcsWorld *world)
{
    return world->context;
}

void ecs_set_context(
    EcsWorld *world,
    void *context)
{
    world->context = context;
}
