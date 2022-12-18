/**
 * @file datastructures/hashmap.c
 * @brief Hashmap data structure.
 * 
 * The hashmap data structure is built on top of the map data structure. Where 
 * the map data structure can only work with 64bit key values, the hashmap can
 * hash keys of any size, and handles collisions between hashes.
 */

#include "../private_api.h"

static
int32_t flecs_hashmap_find_key(
    const ecs_hashmap_t *map,
    ecs_vec_t *keys,
    ecs_size_t key_size, 
    const void *key)
{
    int32_t i, count = ecs_vec_count(keys);
    void *key_array = ecs_vec_first(keys);
    for (i = 0; i < count; i ++) {
        void *key_ptr = ECS_OFFSET(key_array, key_size * i);
        if (map->compare(key_ptr, key) == 0) {
            return i;
        }
    }
    return -1;
}

void _flecs_hashmap_init(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator)
{
    map->key_size = key_size;
    map->value_size = value_size;
    map->hash = hash;
    map->compare = compare;
    ecs_map_init(&map->impl, ecs_hm_bucket_t, allocator, 0);
}

void flecs_hashmap_fini(
    ecs_hashmap_t *map)
{
    ecs_allocator_t *a = map->impl.allocator;
    ecs_map_iter_t it = ecs_map_iter(&map->impl);
    ecs_hm_bucket_t *bucket;
    uint64_t key;
    while ((bucket = ecs_map_next(&it, ecs_hm_bucket_t, &key))) {
        ecs_vec_fini(a, &bucket->keys, map->key_size);
        ecs_vec_fini(a, &bucket->values, map->value_size);
    }

    ecs_map_fini(&map->impl);
}

void flecs_hashmap_copy(
    const ecs_hashmap_t *src,
    ecs_hashmap_t *dst)
{
    if (dst != src) {
        *dst = *src;
    }
    
    ecs_map_t *impl = ecs_map_copy(&dst->impl);
    ecs_allocator_t *a = impl->allocator;
    dst->impl = *impl;
    ecs_os_free(impl);

    ecs_map_iter_t it = ecs_map_iter(&dst->impl);
    ecs_hm_bucket_t *bucket;
    while ((bucket = ecs_map_next(&it, ecs_hm_bucket_t, NULL))) {
        bucket->keys = ecs_vec_copy(a, &bucket->keys, dst->key_size);
        bucket->values = ecs_vec_copy(a, &bucket->values, dst->value_size);
    }
}

void* _flecs_hashmap_get(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map->hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_get(&map->impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return NULL;
    }

    int32_t index = flecs_hashmap_find_key(map, &bucket->keys, key_size, key);
    if (index == -1) {
        return NULL;
    }

    return ecs_vec_get(&bucket->values, value_size, index);
}

flecs_hashmap_result_t _flecs_hashmap_ensure(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map->hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_ensure(&map->impl, ecs_hm_bucket_t, hash);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_allocator_t *a = map->impl.allocator;
    void *value_ptr, *key_ptr;
    ecs_vec_t *keys = &bucket->keys;
    ecs_vec_t *values = &bucket->values;
    if (!keys->array) {
        keys = ecs_vec_init(a, &bucket->keys, key_size, 1);
        values = ecs_vec_init(a, &bucket->values, value_size, 1);
        key_ptr = ecs_vec_append(a, keys, key_size);        
        value_ptr = ecs_vec_append(a, values, value_size);
        ecs_os_memcpy(key_ptr, key, key_size);
        ecs_os_memset(value_ptr, 0, value_size);
    } else {
        int32_t index = flecs_hashmap_find_key(map, keys, key_size, key);
        if (index == -1) {
            key_ptr = ecs_vec_append(a, keys, key_size);        
            value_ptr = ecs_vec_append(a, values, value_size);
            ecs_os_memcpy(key_ptr, key, key_size);
            ecs_os_memset(value_ptr, 0, value_size);
        } else {
            key_ptr = ecs_vec_get(keys, key_size, index);
            value_ptr = ecs_vec_get(values, value_size, index);
        }
    }

    return (flecs_hashmap_result_t){
        .key = key_ptr, .value = value_ptr, .hash = hash
    };
}

void _flecs_hashmap_set(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value)
{
    void *value_ptr = _flecs_hashmap_ensure(map, key_size, key, value_size).value;
    ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_memcpy(value_ptr, value, value_size);
}

ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash)
{
    ecs_assert(map != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_map_get(&map->impl, ecs_hm_bucket_t, hash);
}

void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash,
    int32_t index)
{
    ecs_vec_remove(&bucket->keys, map->key_size, index);
    ecs_vec_remove(&bucket->values, map->value_size, index);

    if (!ecs_vec_count(&bucket->keys)) {
        ecs_allocator_t *a = map->impl.allocator;
        ecs_vec_fini(a, &bucket->keys, map->key_size);
        ecs_vec_fini(a, &bucket->values, map->value_size);
        ecs_map_remove(&map->impl, hash);
    }
}

void _flecs_hashmap_remove_w_hash(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);
    (void)value_size;

    ecs_hm_bucket_t *bucket = ecs_map_get(&map->impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return;
    }

    int32_t index = flecs_hashmap_find_key(map, &bucket->keys, key_size, key);
    if (index == -1) {
        return;
    }

    flecs_hm_bucket_remove(map, bucket, hash, index);
}

void _flecs_hashmap_remove(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map->key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map->value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map->hash(key);
    _flecs_hashmap_remove_w_hash(map, key_size, key, value_size, hash);
}

flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map)
{
    return (flecs_hashmap_iter_t){
        .it = ecs_map_iter(&map->impl)
    };
}

void* _flecs_hashmap_next(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size)
{
    int32_t index = ++ it->index;
    ecs_hm_bucket_t *bucket = it->bucket;
    while (!bucket || it->index >= ecs_vec_count(&bucket->keys)) {
        bucket = it->bucket = ecs_map_next(&it->it, ecs_hm_bucket_t, NULL);
        if (!bucket) {
            return NULL;
        }
        index = it->index = 0;
    }

    if (key_out) {
        *(void**)key_out = ecs_vec_get(&bucket->keys, key_size, index);
    }
    
    return ecs_vec_get(&bucket->values, value_size, index);
}
