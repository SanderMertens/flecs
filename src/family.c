#include <string.h>
#include <assert.h>
#include "include/private/reflecs.h"

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

/** Get family id from entity handle */
EcsFamily ecs_family_from_handle(
    EcsWorld *world,
    EcsHandle entity)
{
    if (entity == 0) {
        return 0;
    }

    uint64_t row_64 = ecs_map_get64(world->entity_index, entity);
    assert(row_64 != 0);

    EcsRow row = ecs_to_row(row_64);
    EcsTable *table = ecs_world_get_table(world, row.family_id);
    EcsHandle *components = ecs_array_buffer(table->family);
    EcsHandle component = components[0];
    EcsFamily family = 0;

    if (component == EcsFamily_h) {
        family = *(EcsFamily*)
            ECS_OFFSET(ecs_table_get(table, row.index), sizeof(EcsHandle));
    } else if (component == EcsComponent_h || component == EcsPrefab_h){
        family = ecs_family_register(world, entity, NULL);
    }

    assert(family != 0);

    return family;
}


/** Register a new family, optionally extending from existing family */
EcsFamily ecs_family_register(
    EcsWorld *world,
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

    return register_family_from_buffer(world, new_buffer, count);
}

EcsFamily ecs_family_add(
    EcsWorld *world,
    EcsFamily family,
    EcsHandle component)
{
    EcsArray *array = ecs_map_get(world->family_index, family);
    assert(!family || array != NULL);
    return ecs_family_register(world, component, array);
}

/** O(n) algorithm to merge families */
EcsFamily ecs_family_merge(
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

    if (arr_cur) {
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

        if (add && (!cur || add < cur)) {
            buf_new[new_count] = add;
            new_count ++;
            i_add ++;
            add = i_add < add_count ? buf_add[i_add] : 0;
        } else if (cur && (!add || cur < add)) {
            buf_new[new_count] = cur;
            new_count ++;
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
    } while (prev_count != new_count);

    if (new_count) {
        return register_family_from_buffer(world, buf_new, new_count);
    } else {
        return 0;
    }
}

/* O(n) algorithm to check whether family 1 is equal or superset of family 2 */
EcsHandle ecs_family_contains(
    EcsWorld *world,
    EcsFamily family_id_1,
    EcsFamily family_id_2,
    bool match_all,
    bool match_prefab)
{
    if (family_id_1 == family_id_2) {
        return true;
    }

    EcsArray *f_1 = ecs_map_get(world->family_index, family_id_1);
    EcsArray *f_2 = ecs_map_get(world->family_index, family_id_2);

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
    EcsFamily family_id,
    EcsHandle component)
{
    EcsArray *family = ecs_map_get(world->family_index, family_id);
    EcsHandle *buffer = ecs_array_buffer(family);
    uint32_t i, count = ecs_array_count(family);

    for (i = 0; i < count; i++) {
        if (buffer[i] == component) {
            return true;
        }
    }

    return false;
}
