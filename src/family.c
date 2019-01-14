#include <string.h>
#include <assert.h>
#include "include/private/reflecs.h"

/** Parse callback that adds family to family identifier for ecs_new_family */
static
EcsResult add_family(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *entity_id,
    void *data)
{
    EcsFamilyComponent *family = data;

    if (oper_kind != EcsOperAnd) {
        return EcsError;
    }

    if (!strcmp(entity_id, "0")) {
        *family = (EcsFamilyComponent){0, 0};
    } else {
        if (elem_kind != EcsFromEntity) {
            return EcsError;
        }

        EcsHandle entity = ecs_lookup(world, entity_id);
        if (!entity) {
            return EcsError;
        }

        EcsFamily family_id = ecs_family_register(
            world, NULL, entity, NULL);
        assert(family_id != 0);

        EcsFamily resolved_id = ecs_family_from_handle(
            world, NULL, entity, NULL);
        assert(resolved_id != 0);

        family->family = ecs_family_merge(
            world, NULL, family->family, family_id, 0);

        family->resolved = ecs_family_merge(
            world, NULL, family->resolved, resolved_id, 0);
    }

    return EcsOk;
}

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

static
EcsFamily register_family_from_buffer(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle *buf,
    uint32_t count)
{
    EcsFamily new_id = hash_handle_array(buf, count);
    EcsMap *family_index;

    if (world->in_progress && world->threads_running) {
        assert(stage != NULL);
        family_index = stage->family_stage;
    } else {
        family_index = world->family_index;
    }

    EcsArray *new_array = ecs_map_get(family_index, new_id);

    if (!new_array) {
        new_array = ecs_array_new_from_buffer(&handle_arr_params, count, buf);
        ecs_map_set(family_index, new_id,  new_array);
    }

    return new_id;
}

/* -- Private functions -- */

EcsArray* ecs_family_get(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id)
{
    EcsArray *result = ecs_map_get(world->family_index, family_id);
    if (!result) {
        if (world->in_progress && world->threads_running) {
            assert(stage != NULL);
            result = ecs_map_get(stage->family_stage, family_id);
        }
    }

    return result;
}

/** Get family id from entity handle */
EcsFamily ecs_family_from_handle(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity,
    EcsEntityInfo *info)
{
    if (entity == 0) {
        return 0;
    }

    EcsTable *table;
    EcsArray *rows;
    uint32_t index;

    if (!info) {
        uint64_t row_64 = ecs_map_get64(world->entity_index, entity);
        assert(row_64 != 0);
        EcsRow row = ecs_to_row(row_64);
        table = ecs_world_get_table(world, stage, row.family_id);
        rows = table->rows;
        index = row.index;
    } else {
        table = info->table;
        rows = info->rows;
        index = info->index;
    }

    EcsHandle *components = ecs_array_buffer(table->family);
    EcsHandle component = components[0];
    EcsFamily family = 0;

    if (component == EcsFamilyComponent_h) {
        EcsFamilyComponent *fe = ECS_OFFSET(ecs_table_get(
            table, rows, index), sizeof(EcsHandle));
        family = fe->resolved;
    } else {
        family = ecs_family_register(world, stage, entity, NULL);
    }

    assert(family != 0);

    return family;
}


/** Register a new family, optionally extending from existing family */
EcsFamily ecs_family_register(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle to_add,
    EcsArray *set)
{
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
        count ++;
    } else if (set) {
        void *buffer = ecs_array_buffer(set);
        new_buffer = buffer;
    } else {
        return 0;
    }

    return register_family_from_buffer(world, stage, new_buffer, count);
}

EcsFamily ecs_family_add(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family,
    EcsHandle component)
{
    EcsArray *array = ecs_family_get(world, stage, family);
    assert(!family || array != NULL);
    return ecs_family_register(world, stage, component, array);
}

/** O(n) algorithm to merge families */
EcsFamily ecs_family_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily cur_id,
    EcsFamily to_add_id,
    EcsFamily to_remove_id)
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

    EcsArray *arr_cur = ecs_family_get(world, stage, cur_id);
    EcsArray *to_add = NULL, *to_del = NULL;
    EcsHandle *buf_add = NULL, *buf_del = NULL, *buf_cur = NULL;
    EcsHandle cur = 0, add = 0, del = 0;
    uint32_t i_cur = 0, i_add = 0, i_del = 0;
    uint32_t cur_count = 0, add_count = 0, del_count = 0;

    if (to_remove_id) {
        to_del = ecs_family_get(world, stage, to_remove_id);
        del_count = ecs_array_count(to_del);
        buf_del = ecs_array_buffer(to_del);
        del = buf_del[0];
    }

    if (arr_cur) {
        cur_count = ecs_array_count(arr_cur);
        buf_cur = ecs_array_buffer(arr_cur);
        cur = buf_cur[0];
    }

    if (to_add_id) {
        to_add = ecs_family_get(world, stage, to_add_id);
        add_count = ecs_array_count(to_add);
        buf_add = ecs_array_buffer(to_add);
        add = buf_add[0];
    }

    EcsHandle buf_new[cur_count + add_count];
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
        return register_family_from_buffer(world, stage, buf_new, new_count);
    } else {
        return 0;
    }
}

/* O(n) algorithm to check whether family 1 is equal or superset of family 2 */
EcsHandle ecs_family_contains(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id_1,
    EcsFamily family_id_2,
    bool match_all,
    bool match_prefab)
{
    if (family_id_1 == family_id_2) {
        return true;
    }

    EcsArray *f_1 = ecs_family_get(world, stage, family_id_1);
    EcsArray *f_2 = ecs_family_get(world, stage, family_id_2);

    assert(f_1 && f_2);

    uint32_t i_2, i_1 = 0;
    EcsHandle *h2p, *h1p = ecs_array_get(f_1, &handle_arr_params, i_1);
    EcsHandle h1 = 0, prefab = 0;
    bool prefab_searched = false;

    for (i_2 = 0; (h2p = ecs_array_get(f_2, &handle_arr_params, i_2)); i_2 ++) {
        EcsHandle h2 = *h2p;

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
                prefab = ecs_map_get64(world->prefab_index, family_id_1);
                prefab_searched = true;
            }

            if (prefab) {
                if (ecs_get_ptr(world, prefab, h2) != NULL) {
                    h1 = h2;
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

bool ecs_family_contains_component(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id,
    EcsHandle component)
{
    EcsArray *family = ecs_family_get(world, stage, family_id);
    EcsHandle *buffer = ecs_array_buffer(family);
    uint32_t i, count = ecs_array_count(family);

    for (i = 0; i < count; i++) {
        if (buffer[i] == component) {
            return true;
        }
    }

    return false;
}

/* -- Public API -- */

EcsHandle ecs_new_family(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsFamilyComponent family = {0};

    EcsHandle result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    if (ecs_parse_component_expr(world, sig, add_family, &family) != EcsOk) {
        return 0;
    }

    EcsHandle family_entity = ecs_map_get64(
        world->family_handles, family.family);

    if (family_entity) {
        EcsId *id_ptr = ecs_get_ptr(world, family_entity, EcsId_h);

        assert(id_ptr != NULL);
        assert_func(!strcmp(*id_ptr, id));

        return family_entity;
    } else {
        result = ecs_new_w_family(world, NULL, world->family_family);
        EcsId *id_ptr = ecs_get_ptr(world, result, EcsId_h);
        *id_ptr = id;

        EcsFamilyComponent *family_ptr = ecs_get_ptr(
            world, result, EcsFamilyComponent_h);
        *family_ptr = family;

        ecs_map_set64(world->family_handles, family.family, result);

        return result;
    }
}

EcsHandle ecs_new_prefab(
    EcsWorld *world,
    const char *id,
    const char *sig)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsFamilyComponent family = {0};

    EcsHandle result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    if (ecs_parse_component_expr(world, sig, add_family, &family) != EcsOk) {
        return 0;
    }

    family.resolved = ecs_family_merge(
        world, NULL, world->prefab_family, family.resolved, 0);
    result = ecs_new_w_family(world, NULL, family.resolved);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId_h);
    if (!id_data) {
        return 0;
    }

    *id_data = id;

    return result;
}
