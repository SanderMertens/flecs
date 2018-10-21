#include <string.h>
#include <assert.h>
#include "include/private/reflecs.h"

/** Parse callback that adds family to family identifier for ecs_new_family */
static
EcsResult add_family(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    void *data)
{
    EcsFamily *family_id = data;
    EcsArray *family = ecs_map_get(world->family_index, *(EcsFamily*)family_id);

    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    *family_id = ecs_family_register(world, component, family);

    return EcsOk;
}

/** Move row from one table to another table */
static
void copy_row(
    EcsWorld *world,
    EcsTable *new_table,
    uint32_t new_index,
    EcsTable *old_table,
    uint32_t old_index)
{
    int i_new = 0, i_old = 0;
    EcsArray *old_family = old_table->family;
    void *old_row = ecs_table_get(old_table, old_index);
    EcsArray *new_family = new_table->family;
    void *new_row = ecs_table_get(new_table, new_index);

    uint32_t new_offset = sizeof(EcsHandle);
    uint32_t old_offset = sizeof(EcsHandle);
    uint32_t bytes_to_copy = 0;
    uint32_t count_new = ecs_array_count(new_family);

    EcsHandle *old_ptr = ecs_array_get(
       old_family, &handle_arr_params, i_old);

    for (; i_new < count_new; ) {
        EcsHandle new = *(EcsHandle*)ecs_array_get(
            new_family, &handle_arr_params, i_new);

        EcsHandle old = 0;

        if (old_ptr) {
            old = *old_ptr;
        }

        if (new == old) {
            bytes_to_copy += new_table->columns[i_new];
            i_new ++;
            i_old ++;
            old_ptr = ecs_array_get(
               old_family, &handle_arr_params, i_old);
        } else {
            if (bytes_to_copy) {
                void *dst = ECS_OFFSET(new_row, new_offset);
                void *src = ECS_OFFSET(old_row, old_offset);
                memcpy(dst, src, bytes_to_copy);
                new_offset += bytes_to_copy;
                old_offset += bytes_to_copy;
                bytes_to_copy = 0;
            }

            if (old) {
                if (new < old) {
                    new_offset += new_table->columns[i_new];
                    i_new ++;
                } else if (old < new) {
                    old_offset += old_table->columns[i_old];
                    i_old ++;
                    old_ptr = ecs_array_get(
                       old_family, &handle_arr_params, i_old);
                }
            }
        }

        if (!old || !old_ptr) {
            break;
        }
    }

    if (bytes_to_copy) {
        void *dst = ECS_OFFSET(new_row, new_offset);
        void *src = ECS_OFFSET(old_row, old_offset);
        memcpy(dst, src, bytes_to_copy);
    }
}

static
void notify(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *systems,
    uint32_t row,
    EcsFamily family)
{
    uint32_t i, count = ecs_array_count(systems);
    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

    for (i = 0; i < count; i ++) {
        EcsHandle h = *(EcsHandle*)
            ecs_array_get(systems, &handle_arr_params, i);
        EcsSystem *system_data = ecs_get(world, h, EcsSystem_h);

        if (ecs_family_contains(
            world,
            family,
            system_data->from_entity[EcsOperAnd],
            true))
        {
            ecs_system_notify(world, h, system_data, table, table_index, row);
        }
    }
}

static
void init_components(
    EcsWorld *world,
    EcsTable *table,
    uint32_t row,
    EcsFamily to_init)
{
    notify(world, table, table->init_systems, row, to_init);
}

static
void deinit_components(
    EcsWorld *world,
    EcsTable *table,
    uint32_t row,
    EcsFamily to_deinit)
{
    notify(world, table, table->deinit_systems, row, to_deinit);
}

/** Stage components for adding or removing from an entity */
static
EcsResult stage(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component,
    bool is_remove)
{
    EcsFamily family_id = 0;
    EcsArray *family = NULL;

    if (is_remove) {
        family_id = ecs_map_get64(world->remove_stage, entity);
    } else {
        family_id = ecs_map_get64(world->add_stage, entity);
    }

    if (family_id) {
        family = ecs_map_get(world->family_index, family_id);
        if (!family) {
            return EcsError;
        }
    }

    EcsFamily new_family_id =
        ecs_family_register(world, component, family);

    assert(new_family_id != 0);

    if (family_id != new_family_id) {
        if (is_remove) {
            ecs_map_set64(world->remove_stage, entity, new_family_id);
        } else {
            ecs_map_set64(world->add_stage, entity, new_family_id);
        }
    }

    return EcsOk;
}

/** Copy default values from base (and base of base) prefabs */
static
void copy_from_prefab(
    EcsWorld *world,
    EcsTable *new_table,
    uint32_t new_index,
    EcsFamily family_id)
{
    EcsHandle prefab;
    while ((prefab = ecs_map_get64(world->prefab_index, family_id))) {
        EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, prefab));
        EcsTable *prefab_table = ecs_world_get_table(world, row.family_id);
        copy_row(world, new_table, new_index, prefab_table, row.index);
        family_id = row.family_id;
    }
}

/** Commit an entity with a specified family to memory */
static
EcsResult commit_w_family(
    EcsWorld *world,
    EcsHandle entity,
    EcsFamily family_id,
    EcsFamily to_add,
    EcsFamily to_remove)
{
    EcsTable *new_table = NULL;
    uint32_t new_index = 0;
    if (family_id) {
        new_table = ecs_world_get_table(world, family_id);
        assert(new_table != NULL);
        new_index = ecs_table_insert(new_table, entity);
    }

    uint64_t row_64 = ecs_map_get64(world->entity_index, entity);

    if (row_64) {
        EcsRow old_row = ecs_to_row(row_64);
        EcsTable *old_table = ecs_world_get_table(world, old_row.family_id);
        if (new_table) {
            copy_row(world, new_table, new_index, old_table, old_row.index);
        }
        ecs_table_delete(old_table, old_row.index);
        if (to_remove) {
            deinit_components(world, old_table, old_row.index, to_remove);
        }
    }

    if (new_table) {
        if (to_add) {
            init_components(world, new_table, new_index, to_add);
            copy_from_prefab(world, new_table, new_index, family_id);
        }

        EcsRow row = {.family_id = family_id, .index = new_index};
        row_64 = ecs_from_row(row);
        ecs_map_set64(world->entity_index, entity, row_64);
    } else {
        ecs_map_set64(world->entity_index, entity, 0);
    }

    world->valid_schedule = false;

    return EcsOk;
}

/* -- Private functions -- */
EcsHandle ecs_new_w_family(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsHandle entity = ++ world->last_handle;
    commit_w_family(world, entity, family_id, family_id, 0);
    return entity;
}

/* -- Public functions -- */

EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsFamily to_add = ecs_map_get64(world->add_stage, entity);
    EcsFamily to_remove = ecs_map_get64(world->remove_stage, entity);
    EcsFamily family_id = 0;

    uint64_t row_64 = ecs_map_get64(world->entity_index, entity);
    if (row_64) {
        EcsRow row = ecs_to_row(row_64);
        family_id = row.family_id;
    }

    EcsFamily new_family_id = ecs_family_merge(
        world, family_id, to_add, to_remove);

    if (new_family_id == family_id) {
        return EcsOk;
    }

    family_id = new_family_id;

    EcsResult result = commit_w_family(
        world, entity, family_id, to_add, to_remove);

    if (to_add) {
        ecs_map_remove(world->add_stage, entity);
    }

    if (to_remove) {
        ecs_map_remove(world->remove_stage, entity);
    }

    return result;
}

EcsHandle ecs_new(
    EcsWorld *world,
    EcsHandle type)
{
    EcsHandle entity = ++ world->last_handle;
    if (type) {
        EcsFamily family_id = ecs_family_from_handle(world, type);
        commit_w_family(world, entity, family_id, family_id, 0);
    }

    return entity;
}

void ecs_delete(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    if (row.family_id) {
        EcsTable *table = ecs_world_get_table(world, row.family_id);
        ecs_table_delete(table, row.index);
        ecs_map_remove(world->entity_index, entity);
        world->valid_schedule = false;
    }
}

EcsResult ecs_add(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    return stage(world, entity, component, false);
}

EcsResult ecs_remove(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    return stage(world, entity, component, true);
}

void* ecs_get(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
    if (!row.family_id) {
        return NULL;
    }

    EcsTable *table = ecs_world_get_table(world, row.family_id);
    uint32_t offset = 0;
    uint16_t column = 0;
    EcsArray *family = table->family;
    EcsHandle *buffer = ecs_array_buffer(family);
    uint32_t count = ecs_array_count(family);

    for (column = 0; column < count; column ++) {
        if (component == buffer[column]) {
            break;
        }
        offset += table->columns[column];
    }

    if (column == count) {
        EcsHandle prefab = ecs_map_get64(world->prefab_index, row.family_id);
        if (prefab) {
            return ecs_get(world, prefab, component);
        } else {
            return NULL;
        }
    }

    void *row_ptr = ecs_table_get(table, row.index);
    assert(row_ptr != NULL);

    return ECS_OFFSET(row_ptr, offset + sizeof(EcsHandle));
}

EcsHandle ecs_new_component(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsHandle result = ecs_new_w_family(world, world->component_family);

    EcsComponent *component_data = ecs_get(world, result, EcsComponent_h);
    if (!component_data) {
        return 0;
    }

    EcsId *id_data = ecs_get(world, result, EcsId_h);
    if (!id_data) {
        return 0;
    }

    id_data->id = id;
    component_data->size = size;

    return result;
}

EcsHandle ecs_new_family(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    EcsFamily family = 0;

    if (ecs_parse_component_expr(world, sig, add_family, &family) != EcsOk) {
        return 0;
    }

    EcsHandle family_entity = ecs_map_get64(world->family_handles, family);
    if (family_entity) {
        EcsId *id_ptr = ecs_get(world, family_entity, EcsId_h);

        assert(id_ptr != NULL);
        assert_func(!strcmp(id_ptr->id, id));

        return family_entity;
    } else {
        EcsHandle result = ecs_new_w_family(world, world->family_family);
        EcsId *id_ptr = ecs_get(world, result, EcsId_h);
        id_ptr->id = id;

        EcsFamily *family_ptr = ecs_get(world, result, EcsFamily_h);
        *family_ptr = family;

        ecs_map_set64(world->family_handles, family, result);

        return result;
    }
}

EcsHandle ecs_new_prefab(
    EcsWorld *world,
    const char *id,
    EcsHandle type)
{
    EcsFamily family = ecs_family_from_handle(world, type);
    family = ecs_family_merge(world, world->prefab_family, family, 0);
    EcsHandle result = ecs_new_w_family(world, family);

    EcsId *id_data = ecs_get(world, result, EcsId_h);
    if (!id_data) {
        return 0;
    }

    id_data->id = id;

    return result;
}

const char* ecs_id(
    EcsWorld *world,
    EcsHandle entity)
{
    EcsId *id = ecs_get(world, entity, EcsId_h);
    if (id) {
        return id->id;
    } else {
        return NULL;
    }
}
