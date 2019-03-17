#include <string.h>
#include <assert.h>
#include "include/private/flecs.h"

/** Parse callback that adds type to type identifier for ecs_new_type */
static
EcsResult add_type(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *entity_id,
    const char *source_id,
    void *data)
{
    EcsTypeComponent *type = data;
    EcsStage *stage = &world->main_stage;

    if (oper_kind != EcsOperAnd) {
        return EcsError;
    }

    if (!strcmp(entity_id, "0")) {
        *type = (EcsTypeComponent){0, 0};
    } else {
        if (elem_kind != EcsFromSelf) {
            return EcsError;
        }

        EcsEntity entity = ecs_lookup(world, entity_id);
        if (!entity) {
            return EcsError;
        }

        EcsType type_id = ecs_type_register(
            world, stage, entity, NULL);
        assert(type_id != 0);

        EcsType resolved_id = ecs_type_from_handle(
            world, stage, entity, NULL);
        assert(resolved_id != 0);

        type->type = ecs_type_merge(
            world, stage, type->type, type_id, 0);

        type->resolved = ecs_type_merge(
            world, stage, type->resolved, resolved_id, 0);
    }

    return EcsOk;
}

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(EcsEntity*)p1 - *(EcsEntity*)p2;
}

/** Hash array of handles */
static
uint32_t hash_handle_array(
    EcsEntity* array,
    uint32_t count)
{
    uint32_t hash = 0;
    int i;
    for (i = 0; i < count; i ++) {
        ecs_hash(&array[i], sizeof(EcsEntity), &hash);
    }
    return hash;
}

static
void notify_create_type(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *systems,
    EcsType type)
{
    EcsEntity *buffer = ecs_array_buffer(systems);
    uint32_t i, count = ecs_array_count(systems);

    for (i = 0; i < count; i ++) {
        ecs_row_system_notify_of_type(world, stage, buffer[i], type);
    }
}

static
EcsType register_type_from_buffer(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity *buf,
    uint32_t count)
{
    EcsType new_id = hash_handle_array(buf, count);
    EcsMap *type_index;

    if (!stage) stage = &world->main_stage;

    type_index = stage->type_index;

    EcsArray *new_array = ecs_map_get(type_index, new_id);

    if (!new_array) {
        new_array = ecs_array_new_from_buffer(&handle_arr_params, count, buf);
        ecs_map_set(type_index, new_id,  new_array);

        if (!world->in_progress) {
            notify_create_type(world, stage, world->add_systems, new_id);
            notify_create_type(world, stage, world->remove_systems, new_id);
            notify_create_type(world, stage, world->set_systems, new_id);
        }
    }

    return new_id;
}

/* -- Private functions -- */

EcsArray* ecs_type_get(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id)
{
    EcsArray *result = ecs_map_get(world->main_stage.type_index, type_id);
    if (!result) {
        if (world->threads_running) {
            assert(stage != NULL);
            result = ecs_map_get(stage->type_index, type_id);
        }
    }

    return result;
}

/** Get type id from entity handle */
EcsType ecs_type_from_handle(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    EcsEntityInfo *info)
{
    if (entity == 0) {
        return 0;
    }

    EcsTable *table = NULL;
    EcsTableColumn *columns = NULL;
    uint32_t index = 0;
    EcsEntity component = 0;
    EcsType type = 0;

    if (!info) {
        uint64_t row_64 = ecs_map_get64(world->main_stage.entity_index, entity);
        if (!row_64 && world->in_progress) {
            row_64 = ecs_map_get64(stage->entity_index, entity);
            if (!row_64) {
                return 0;
            }
        }

        EcsRow row = ecs_to_row(row_64);
        if (row.type_id) {
            table = ecs_world_get_table(world, stage, row.type_id);
            columns = table->columns;
            index = row.index;
        }
    } else {
        table = info->table;
        columns = info->columns;
        index = info->index;
    }

    if (table) {
        EcsEntity *components = ecs_array_buffer(table->type);
        component = components[0];
    }

    if (component == EEcsTypeComponent) {
        EcsArrayParams params = {.element_size = sizeof(EcsTypeComponent)};
        EcsTypeComponent *fe = ecs_array_get(columns[1].data, &params, index);
        type = fe->resolved;
    } else {
        type = ecs_type_register(world, stage, entity, NULL);
    }

    assert(type != 0);

    return type;
}


/** Register a new type, optionally extending from existing type */
EcsType ecs_type_register(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity to_add,
    EcsArray *set)
{
    uint32_t count = ecs_array_count(set);
    EcsEntity new_set[count + 1];
    void *new_buffer = new_set;

    if (to_add) {
        void *buffer = ecs_array_buffer(set);
        if (count) {
            memcpy(new_set, buffer, sizeof(EcsEntity) * count);
        }
        new_set[count] = to_add;
        qsort(new_set, count + 1, sizeof(EcsEntity), compare_handle);
        count ++;
    } else if (set) {
        void *buffer = ecs_array_buffer(set);
        new_buffer = buffer;
    } else {
        return 0;
    }

    return register_type_from_buffer(world, stage, new_buffer, count);
}

EcsType ecs_type_add(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type,
    EcsEntity component)
{
    EcsArray *array = ecs_type_get(world, stage, type);
    assert(!type || array != NULL);
    return ecs_type_register(world, stage, component, array);
}

EcsType ecs_type_merge_arr(
    EcsWorld *world,
    EcsStage *stage,
    EcsArray *arr_cur,
    EcsArray *to_add,
    EcsArray *to_del)
{
    EcsEntity *buf_add = NULL, *buf_del = NULL, *buf_cur = NULL;
    EcsEntity cur = 0, add = 0, del = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    if (to_del) {
        del_count = ecs_array_count(to_del);
        buf_del = ecs_array_buffer(to_del);
        del = buf_del[0];
    }

    if (arr_cur) {
        cur_count = ecs_array_count(arr_cur);
        buf_cur = ecs_array_buffer(arr_cur);
        cur = buf_cur[0];
    }

    if (to_add) {
        add_count = ecs_array_count(to_add);
        buf_add = ecs_array_buffer(to_add);
        add = buf_add[0];
    }

        EcsEntity buf_new[cur_count + add_count];
    uint32_t new_count = 0;

    do {
        if (add && (!cur || add < cur)) {
            buf_new[new_count] = add;
            new_count ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
        } else if (cur && (!add || cur < add)) {
            while (del && del < cur) {
                i_del ++;
                del = i_del < del_count ? buf_del[i_del] : 0;
            }

            if (del != cur) {
                buf_new[new_count] = cur;
                new_count ++;
            } else if (del == cur) {
                i_del ++;
                del = i_del < del_count ? buf_del[i_del] : 0;
            }

            i_cur ++;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        } else if (add && add == cur) {
            buf_new[new_count] = add;
            new_count ++;
            i_cur ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
            cur = i_cur < cur_count ? buf_cur[i_cur] : 0;
        }
    } while (cur || add);

    if (new_count) {
        return register_type_from_buffer(world, stage, buf_new, new_count);
    } else {
        return 0;
    }
}

/** O(n) algorithm to merge families */
EcsType ecs_type_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsType cur_id,
    EcsType to_add_id,
    EcsType to_remove_id)
{
    if (!to_remove_id) {
        if (cur_id && !to_add_id) {
            return cur_id;
        } else if (to_add_id && !cur_id) {
            return to_add_id;
        } else if (to_add_id == cur_id) {
            return cur_id;
        }
    } else if (to_remove_id == cur_id) {
        return to_add_id;
    }

    EcsArray *arr_cur = ecs_type_get(world, stage, cur_id);
    EcsArray *to_add = NULL, *to_del = NULL;

    if (to_remove_id) {
        to_del = ecs_type_get(world, stage, to_remove_id);
    }

    if (to_add_id) {
        to_add = ecs_type_get(world, stage, to_add_id);
    }

    return ecs_type_merge_arr(world, stage, arr_cur, to_add, to_del);
}

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
EcsEntity ecs_type_contains(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id_1,
    EcsType type_id_2,
    bool match_all,
    bool match_prefab)
{
    if (!type_id_1) {
        return 0;
    }

    assert(type_id_2 != 0);

    EcsArray *f_1 = ecs_type_get(world, stage, type_id_1);
    EcsArray *f_2 = ecs_type_get(world, stage, type_id_2);

    assert(f_1 && f_2);

    if (type_id_1 == type_id_2) {
        return *(EcsEntity*)ecs_array_get(f_1, &handle_arr_params, 0);
    }

    uint32_t i_2, i_1 = 0;
    EcsEntity *h2p, *h1p = ecs_array_get(f_1, &handle_arr_params, i_1);
    EcsEntity h1 = 0, prefab = 0;
    bool prefab_searched = false;

    for (i_2 = 0; (h2p = ecs_array_get(f_2, &handle_arr_params, i_2)); i_2 ++) {
        EcsEntity h2 = *h2p;

        if (!h1p) {
            return 0;
        }

        h1 = *h1p;

        if (h2 > h1) {
            do {
                i_1 ++;
                h1p = ecs_array_get(f_1, &handle_arr_params, i_1);
            } while (h1p && (h1 = *h1p) && h2 > h1);
        }

        if (h1 != h2) {
            if (match_prefab && !prefab_searched) {
                prefab = ecs_map_get64(world->prefab_index, type_id_1);
                prefab_searched = true;
            }

            if (prefab) {
                if (h2 != EEcsId && h2 != EEcsPrefab) {
                    EcsType type = ecs_type_from_entity(world, h2);
                    if (_ecs_has(world, prefab, type)) {
                        h1 = h2;
                    }
                }
            }

            if (h1 != h2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return h1;
            }
        } else {
            if (!match_all) return h1;
            i_1 ++;
            h1p = ecs_array_get(f_1, &handle_arr_params, i_1);
        }
    }

    if (match_all) {
        return h1;
    } else {
        return 0;
    }
}

bool ecs_type_contains_component(
    EcsWorld *world,
    EcsStage *stage,
    EcsType type_id,
    EcsEntity component,
    bool match_prefab)
{
    EcsArray *type = ecs_type_get(world, stage, type_id);
    EcsEntity *buffer = ecs_array_buffer(type);
    uint32_t i, count = ecs_array_count(type);

    for (i = 0; i < count; i++) {
        if (buffer[i] == component) {
            return true;
        }
    }

    if (match_prefab) {
        EcsEntity prefab = ecs_map_get64(world->prefab_index, type_id);
        if (prefab) {
            if (_ecs_has(world, prefab, component)) {
                return true;
            }
        }
    }

    return false;
}

/* -- Public API -- */

EcsType ecs_new_type(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsTypeComponent type = {0};

    EcsEntity result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    if (ecs_parse_component_expr(world, sig, add_type, &type) != EcsOk) {
        return 0;
    }

    EcsEntity type_entity = ecs_map_get64(
        world->type_handles, type.type);

    if (type_entity) {
        EcsId *id_ptr = ecs_get_ptr(world, type_entity, EcsId);

        assert(id_ptr != NULL);
        if(!strcmp(*id_ptr, id)) {
            ecs_abort(ECS_ENTITY_ALREADY_DEFINED, id);
        }

        return type_entity;
    } else {
        result = _ecs_new(world, world->t_type);
        ecs_set(world, result, EcsId, {id});
        ecs_set(world, result, EcsTypeComponent, {
            .type = type.type, .resolved = type.resolved
        });

        ecs_map_set64(world->type_handles, type.type, result);

        return result;
    }
}

EcsEntity ecs_new_prefab(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsTypeComponent type = {0};

    EcsEntity result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    if (ecs_parse_component_expr(world, sig, add_type, &type) != EcsOk) {
        return 0;
    }

    type.resolved = ecs_type_merge(
        world, NULL, world->t_prefab, type.resolved, 0);

    result = _ecs_new(world, type.resolved);
    ecs_set(world, result, EcsId, {id});

    return result;
}

EcsEntity ecs_new_entity(
    EcsWorld *world,
    const char *id,
    const char *components)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsTypeComponent type = {0};

    EcsEntity result = ecs_lookup(world, id);
    ecs_assert(!result, ECS_ENTITY_ALREADY_DEFINED, id);

    if (ecs_parse_component_expr(world, components, add_type, &type) != EcsOk) {
        return 0;
    }

    result = _ecs_new(world, type.resolved);
    ecs_set(world, result, EcsId, {id});

    return result;
}

int16_t ecs_type_index_of(
    EcsArray *type,
    EcsEntity component)
{
    EcsEntity *buf = ecs_array_buffer(type);
    int i, count = ecs_array_count(type);
    
    for (i = 0; i < count; i ++) {
        if (buf[i] == component) {
            return i; 
        }
    }

    return -1;
}

EcsType _ecs_merge_type(
    EcsWorld *world,
    EcsType type,
    EcsType type_add,
    EcsType type_remove)
{
    EcsStage *stage = ecs_get_stage(&world);
    return ecs_type_merge(world, stage, type, type_add, type_remove);
}