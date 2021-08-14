#include "../private_api.h"

typedef struct ecs_hm_bucket_t {
    ecs_vector_t *keys;
    ecs_vector_t *values;
} ecs_hm_bucket_t;

static
int32_t find_key(
    ecs_hashmap_t map,
    ecs_vector_t *keys,
    ecs_size_t key_size, 
    const void *key)
{
    int32_t i, count = ecs_vector_count(keys);
    void *key_array = ecs_vector_first_t(keys, key_size, 8);
    for (i = 0; i < count; i ++) {
        void *key_ptr = ECS_OFFSET(key_array, key_size * i);
        if (map.compare(key_ptr, key) == 0) {
            return i;
        }
    }
    return -1;
}

ecs_hashmap_t _flecs_hashmap_new(
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare)
{
    return (ecs_hashmap_t){
        .key_size = key_size,
        .value_size = value_size,
        .compare = compare,
        .hash = hash,
        .impl = ecs_map_new(ecs_hm_bucket_t, 0)
    };
}

void flecs_hashmap_free(
    ecs_hashmap_t map)
{
    ecs_map_iter_t it = ecs_map_iter(map.impl);
    ecs_hm_bucket_t *bucket;
    while ((bucket = ecs_map_next(&it, ecs_hm_bucket_t, NULL))) {
        ecs_vector_free(bucket->keys);
        ecs_vector_free(bucket->values);
    }

    ecs_map_free(map.impl);
}

void* _flecs_hashmap_get(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_get(map.impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return NULL;
    }

    int32_t index = find_key(map, bucket->keys, key_size, key);
    if (index == -1) {
        return NULL;
    }

    return ecs_vector_get_t(bucket->values, value_size, 8, index);
}

flecs_hashmap_result_t _flecs_hashmap_ensure(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_ensure(map.impl, ecs_hm_bucket_t, hash);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);

    void *value_ptr, *key_ptr;

    ecs_vector_t *keys = bucket->keys;
    if (!keys) {
        bucket->keys = ecs_vector_new_t(key_size, 8, 1);
        bucket->values = ecs_vector_new_t(value_size, 8, 1);
        key_ptr = ecs_vector_add_t(&bucket->keys, key_size, 8);
        ecs_os_memcpy(key_ptr, key, key_size);
        value_ptr = ecs_vector_add_t(&bucket->values, value_size, 8);
        ecs_os_memset(value_ptr, 0, value_size);
    } else {
        int32_t index = find_key(map, keys, key_size, key);
        if (index == -1) {
            key_ptr = ecs_vector_add_t(&bucket->keys, key_size, 8);
            ecs_os_memcpy(key_ptr, key, key_size);
            value_ptr = ecs_vector_add_t(&bucket->values, value_size, 8);
            ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_os_memset(value_ptr, 0, value_size);
        } else {
            key_ptr = ecs_vector_get_t(bucket->keys, key_size, 8, index);
            value_ptr = ecs_vector_get_t(bucket->values, value_size, 8, index);
        }
    }

    return (flecs_hashmap_result_t){
        .key = key_ptr,
        .value = value_ptr,
        .hash = hash
    };
}

void _flecs_hashmap_set(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value)
{
    void *value_ptr = _flecs_hashmap_ensure(map, key_size, key, value_size).value;
    ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_memcpy(value_ptr, value, value_size);
}

void _flecs_hashmap_remove_w_hash(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash)
{
    ecs_hm_bucket_t *bucket = ecs_map_get(map.impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return;
    }

    int32_t index = find_key(map, bucket->keys, key_size, key);
    if (index == -1) {
        return;
    }

    ecs_vector_remove_t(bucket->keys, key_size, 8, index);
    ecs_vector_remove_t(bucket->values, value_size, 8, index);

    if (!ecs_vector_count(bucket->keys)) {
        ecs_vector_free(bucket->keys);
        ecs_vector_free(bucket->values);
        ecs_map_remove(map.impl, hash);
    }
}

void _flecs_hashmap_remove(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    _flecs_hashmap_remove_w_hash(map, key_size, key, value_size, hash);
}

flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t map)
{
    return (flecs_hashmap_iter_t){
        .it = ecs_map_iter(map.impl)
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
    while (!bucket || it->index >= ecs_vector_count(bucket->keys)) {
        bucket = it->bucket = ecs_map_next(&it->it, ecs_hm_bucket_t, NULL);
        if (!bucket) {
            return NULL;
        }
        index = it->index = 0;
    }

    if (key_out) {
        *(void**)key_out = ecs_vector_get_t(bucket->keys, key_size, 8, index);
    }
    
    return ecs_vector_get_t(bucket->values, value_size, 8, index);
}
