/**
 * @file datastructures/hashmap.c
 * @brief Hashmap data structure.
 * 
 * The hashmap data structure is built on top of the map data structure. Where 
 * the map data structure can only work with 64bit key values, the hashmap can
 * hash keys of any size, and handles collisions between hashes.
 */

#include "../private_api.h"

static ecs_hm_bucket_t* flecs_hashmap_find_key(
    const ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    const void *key)
{
    for (; bucket; bucket = bucket->next) {
        if (!map->compare(flecs_hm_bucket_key(bucket), key)) {
            return bucket;
        }
    }
    return NULL;
}

static ecs_hm_bucket_t* flecs_hm_bucket_new(
    ecs_hashmap_t *map)
{
    return map->impl.allocator
        ? flecs_calloc(map->impl.allocator, map->bucket_size)
        : ecs_os_calloc(map->bucket_size);
}

static void flecs_hm_bucket_free(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket)
{
    if (map->impl.allocator) {
        flecs_free(map->impl.allocator, map->bucket_size, bucket);
    } else {
        ecs_os_free(bucket);
    }
}

void flecs_hashmap_init_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator)
{
    map->key_size = key_size;
    map->value_size = value_size;
    map->value_offset = 16 + ECS_ALIGN(key_size, 16);
    map->bucket_size = map->value_offset + value_size;
    map->hash = hash;
    map->compare = compare;
    ecs_map_init(&map->impl, allocator);
}

void flecs_hashmap_fini(
    ecs_hashmap_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(&map->impl);

    while (ecs_map_next(&it)) {
        ecs_hm_bucket_t *bucket = ecs_map_ptr(&it);
        while (bucket) {
            ecs_hm_bucket_t *next = bucket->next;
            flecs_hm_bucket_free(map, bucket);
            bucket = next;
        }
    }

    ecs_map_fini(&map->impl);
}

void flecs_hashmap_copy(
    ecs_hashmap_t *dst,
    const ecs_hashmap_t *src)
{
    ecs_assert(dst != src, ECS_INVALID_PARAMETER, NULL);

    flecs_hashmap_init_(dst, src->key_size, src->value_size, src->hash, 
        src->compare, src->impl.allocator);
    ecs_map_copy(&dst->impl, &src->impl);

    ecs_map_iter_t it = ecs_map_iter(&dst->impl);
    while (ecs_map_next(&it)) {
        ecs_hm_bucket_t **ptr = ecs_map_ref(&it, ecs_hm_bucket_t);
        for (ecs_hm_bucket_t *src_bucket = *ptr; src_bucket;
            src_bucket = src_bucket->next)
        {
            *ptr = flecs_hm_bucket_new(dst);
            ecs_os_memcpy(*ptr, src_bucket, dst->bucket_size);
            ptr = &(*ptr)->next;
        }
        *ptr = NULL;
    }
}

void* flecs_hashmap_get_(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map->hash(key);
    ecs_hm_bucket_t *bucket = flecs_hashmap_find_key(
        map, flecs_hashmap_get_bucket(map, hash), key);
    return bucket ? flecs_hm_bucket_value(map, bucket) : NULL;
}

flecs_hashmap_result_t flecs_hashmap_ensure_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map->hash(key);
    ecs_hm_bucket_t **r = ecs_map_ensure_ref(&map->impl, ecs_hm_bucket_t, hash);
    while (*r && map->compare(flecs_hm_bucket_key(*r), key)) {
        r = &(*r)->next;
    }
    ecs_hm_bucket_t *bucket = *r;
    if (!bucket) {
        bucket = *r = flecs_hm_bucket_new(map);
        ecs_os_memcpy(flecs_hm_bucket_key(bucket), key, key_size);
    }
    return (flecs_hashmap_result_t){
        .key = flecs_hm_bucket_key(bucket),
        .value = flecs_hm_bucket_value(map, bucket), .hash = hash
    };
}

ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash)
{
    ecs_assert(map != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_map_get_deref(&map->impl, ecs_hm_bucket_t, hash);
}

void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash)
{
    ecs_hm_bucket_t **head = ecs_map_get_ref(&map->impl, ecs_hm_bucket_t, hash);
    ecs_hm_bucket_t **ptr = head;
    while (*ptr != bucket) {
        ptr = &(*ptr)->next;
    }
    *ptr = bucket->next;
    if (!*head) {
        ecs_map_remove(&map->impl, hash);
    }
    flecs_hm_bucket_free(map, bucket);
}

void flecs_hashmap_remove_w_hash_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);
    (void)value_size;

    ecs_hm_bucket_t *bucket = flecs_hashmap_find_key(
        map, flecs_hashmap_get_bucket(map, hash), key);
    if (bucket) {
        flecs_hm_bucket_remove(map, bucket, hash);
    }
}

flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map)
{
    return (flecs_hashmap_iter_t){
        .it = ecs_map_iter(&map->impl), .map = map
    };
}

void* flecs_hashmap_next_(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size)
{
    (void)key_size;
    (void)value_size;
    ecs_hm_bucket_t *bucket = it->bucket;
    if (!bucket) {
        if (!ecs_map_next(&it->it)) {
            return NULL;
        }
        bucket = ecs_map_ptr(&it->it);
    }
    it->bucket = bucket->next;
    if (key_out) {
        *(void**)key_out = flecs_hm_bucket_key(bucket);
    }
    return flecs_hm_bucket_value(it->map, bucket);
}
