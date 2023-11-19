
#include "../private_api.h"

/* Find location where to insert id into type */
int flecs_type_find_insert(
    const ecs_type_t *type,
    int32_t offset,
    ecs_id_t to_add)
{
    ecs_id_t *array = type->array;
    int32_t i, count = type->count;

    for (i = offset; i < count; i ++) {
        ecs_id_t id = array[i];
        if (id == to_add) {
            return -1;
        }
        if (id > to_add) {
            return i;
        }
    }
    return i;
}

/* Find location of id in type */
int flecs_type_find(
    const ecs_type_t *type,
    ecs_id_t id)
{
    ecs_id_t *array = type->array;
    int32_t i, count = type->count;

    for (i = 0; i < count; i ++) {
        ecs_id_t cur = array[i];
        if (ecs_id_match(cur, id)) {
            return i;
        }
        if (cur > id) {
            return -1;
        }
    }

    return -1;
}

/* Count number of matching ids */
int flecs_type_count_matches(
    const ecs_type_t *type,
    ecs_id_t wildcard,
    int32_t offset)
{
    ecs_id_t *array = type->array;
    int32_t i = offset, count = type->count;

    for (; i < count; i ++) {
        ecs_id_t cur = array[i];
        if (!ecs_id_match(cur, wildcard)) {
            break;
        }
    }

    return i - offset;
}

/* Create type from source type with id */
int flecs_type_new_with(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t with)
{
    ecs_id_t *src_array = src->array;
    int32_t at = flecs_type_find_insert(src, 0, with);
    if (at == -1) {
        return -1;
    }

    int32_t dst_count = src->count + 1;
    ecs_id_t *dst_array = flecs_walloc_n(world, ecs_id_t, dst_count);
    dst->count = dst_count;
    dst->array = dst_array;

    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t remain = src->count - at;
    if (remain) {
        ecs_os_memcpy_n(&dst_array[at + 1], &src_array[at], ecs_id_t, remain);
    }

    dst_array[at] = with;

    return 0;
}

/* Copy type */
ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src)
{
    int32_t src_count = src->count;
    if (!src_count) {
        return (ecs_type_t){ 0 };
    }

    ecs_id_t *ids = flecs_walloc_n(world, ecs_id_t, src_count);
    ecs_os_memcpy_n(ids, src->array, ecs_id_t, src_count);
    return (ecs_type_t) {
        .array = ids,
        .count = src_count
    };
}

/* Create type from source type without ids matching wildcard */
int flecs_type_new_filtered(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t wildcard,
    int32_t at)
{
    *dst = flecs_type_copy(world, src);
    ecs_id_t *dst_array = dst->array;
    ecs_id_t *src_array = src->array;
    if (at) {
        ecs_assert(dst_array != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t i = at + 1, w = at, count = src->count;
    for (; i < count; i ++) {
        ecs_id_t id = src_array[i];
        if (!ecs_id_match(id, wildcard)) {
            dst_array[w] = id;
            w ++;
        }
    }

    dst->count = w;
    if (w != count) {
        dst->array = flecs_wrealloc_n(world, ecs_id_t, w, count, dst->array);
    }

    return 0;
}

/* Create type from source type without id */
int flecs_type_new_without(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t without)
{
    ecs_id_t *src_array = src->array;
    int32_t count = 1, at = flecs_type_find(src, without);
    if (at == -1) {
        return -1;
    }

    int32_t src_count = src->count;
    if (src_count == 1) {
        dst->array = NULL;
        dst->count = 0;
        return 0;
    }

    if (ecs_id_is_wildcard(without)) {
        if (ECS_IS_PAIR(without)) {
            ecs_entity_t r = ECS_PAIR_FIRST(without);
            ecs_entity_t o = ECS_PAIR_SECOND(without);
            if (r == EcsWildcard && o != EcsWildcard) {
                return flecs_type_new_filtered(world, dst, src, without, at);
            }
        }
        count += flecs_type_count_matches(src, without, at + 1);
    }

    int32_t dst_count = src_count - count;
    dst->count = dst_count;
    if (!dst_count) {
        dst->array = NULL;
        return 0;
    }

    ecs_id_t *dst_array = flecs_walloc_n(world, ecs_id_t, dst_count);
    dst->array = dst_array;

    if (at) {
        ecs_os_memcpy_n(dst_array, src_array, ecs_id_t, at);
    }

    int32_t remain = dst_count - at;
    if (remain) {
        ecs_os_memcpy_n(
            &dst_array[at], &src_array[at + count], ecs_id_t, remain);
    }

    return 0;
}

/* Free type */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type)
{
    int32_t count = type->count;
    if (count) {
        flecs_wfree_n(world, ecs_id_t, type->count, type->array);
    }
}

/* Add to type */
void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add)
{
    ecs_type_t new_type;
    int res = flecs_type_new_with(world, &new_type, type, add);
    if (res != -1) {
        flecs_type_free(world, type);
        type->array = new_type.array;
        type->count = new_type.count;
    }
}
