/**
 * @file datastructures/map.c
 * @brief Map data structure.
 * 
 * Map data structure for 64bit keys and dynamic payload size.
 */

#include "../private_api.h"
#include <math.h>

/* The ratio used to determine whether the map should flecs_map_rehash. If
 * (element_count * LOAD_FACTOR) > bucket_count, bucket count is increased. */
#define LOAD_FACTOR (12)
#define KEY_SIZE (ECS_SIZEOF(ecs_map_key_t))
#define GET_ELEM(array, elem_size, index) \
    ECS_OFFSET(array, (elem_size) * (index))

static
uint8_t ecs_log2(uint32_t v) {
    static const uint8_t log2table[32] = 
        {0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
         8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31};

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return log2table[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

/* Get bucket count for number of elements */
static
int32_t flecs_map_get_bucket_count(
    int32_t count)
{
    return flecs_next_pow_of_2((int32_t)(count * LOAD_FACTOR * 0.1));
}

/* Get bucket shift amount for a given bucket count */
static
uint8_t flecs_map_get_bucket_shift (
    int32_t bucket_count)
{
    return (uint8_t)(64u - ecs_log2((uint32_t)bucket_count));
}

/* Get bucket index for provided map key */
static
int32_t flecs_map_get_bucket_index(
    uint16_t bucket_shift,
    ecs_map_key_t key) 
{
    ecs_assert(bucket_shift != 0, ECS_INTERNAL_ERROR, NULL);
    return (int32_t)((11400714819323198485ull * key) >> bucket_shift);
}

/* Get bucket for key */
static
ecs_bucket_t* flecs_map_get_bucket(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map->bucket_shift == flecs_map_get_bucket_shift(map->bucket_count),
        ECS_INTERNAL_ERROR, NULL);
    int32_t bucket_id = flecs_map_get_bucket_index(map->bucket_shift, key);
    ecs_assert(bucket_id < map->bucket_count, ECS_INTERNAL_ERROR, NULL);
    return &map->buckets[bucket_id];
}

/* Free contents of bucket */
static
void flecs_map_clear_bucket(
    ecs_block_allocator_t *allocator,
    ecs_bucket_entry_t *bucket)
{
    while(bucket) {
      ecs_bucket_entry_t *next = bucket->next;
      flecs_bfree(allocator, bucket);
      bucket = next;
    }
}

/* Add element to bucket */
static
void* flecs_map_add_to_bucket(
    ecs_block_allocator_t *allocator,
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *new_entry = flecs_balloc(allocator);
    new_entry->key = key;
    new_entry->next = bucket->first;
    bucket->first = new_entry;
    return ECS_OFFSET(&new_entry->key, ECS_SIZEOF(ecs_map_key_t));;
}

/*  Remove element from bucket */
static
bool flecs_map_remove_from_bucket(
    ecs_block_allocator_t *allocator,
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *entry;
    for (entry = bucket->first; entry; entry = entry->next) {
        if (entry->key == key) {
            ecs_bucket_entry_t **next_holder = &bucket->first;
            while(*next_holder != entry) {
                next_holder = &(*next_holder)->next;
            }
            *next_holder = entry->next;
            flecs_bfree(allocator, entry);
            return true;
        }
    }
    
    return false;
}

/* Get payload pointer for key from bucket */
static
void* flecs_map_get_from_bucket(
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *entry;
    for (entry = bucket->first; entry; entry = entry->next) {
        if (entry->key == key) {
            return ECS_OFFSET(&entry->key, ECS_SIZEOF(ecs_map_key_t));
        }
    }
    
    return NULL;
}

/* Grow number of buckets */
static
void flecs_map_rehash(
    ecs_map_t *map,
    int32_t count)
{
    count = flecs_next_pow_of_2(count);
    if (count < 2) {
        count = 2;
    }
    ecs_assert(count > map->bucket_count, ECS_INTERNAL_ERROR, NULL);
    
    ecs_bucket_t *buckets = map->buckets, *buckets_end = map->buckets_end, *b;
    int32_t old_count = map->bucket_count;
    
    if (map->allocator) {
        map->buckets = flecs_calloc_n(map->allocator, ecs_bucket_t, count);
    } else {
        map->buckets = ecs_os_calloc_n(ecs_bucket_t, count);
    }
    map->bucket_count = count;
    map->bucket_shift = flecs_map_get_bucket_shift(count);
    map->buckets_end = ECS_ELEM_T(map->buckets, ecs_bucket_t, count);

    /* Remap old bucket entries to new buckets */
    for (b = buckets; b < buckets_end; b++) {
        ecs_bucket_entry_t* entry;
        for (entry = b->first; entry;) {
            ecs_bucket_entry_t* next = entry->next;
            int32_t bucket_index = flecs_map_get_bucket_index(
                map->bucket_shift, entry->key);
            ecs_bucket_t *bucket = &map->buckets[bucket_index];
            entry->next = bucket->first;
            bucket->first = entry;
            entry = next;
        }
    }

    if (map->allocator) {
        flecs_free_n(map->allocator, ecs_bucket_t, old_count, buckets);
    } else {
        ecs_os_free(buckets);
    }
}

static
ecs_size_t flecs_map_chunk_size(
    ecs_size_t size)
{
    return size + ECS_SIZEOF(ecs_bucket_entry_t);
}

void _ecs_map_params_init(
    ecs_map_params_t *params,
    ecs_allocator_t *allocator,
    ecs_size_t size)
{
    params->size = size;
    params->allocator = allocator;
    flecs_ballocator_init(&params->entry_allocator,
        flecs_map_chunk_size(size));
}

void ecs_map_params_fini(
    ecs_map_params_t *params)
{
    flecs_ballocator_fini(&params->entry_allocator);
}

void _ecs_map_init_w_params(
    ecs_map_t *result,
    ecs_map_params_t *params)
{
    ecs_assert(params->size < INT16_MAX, ECS_INVALID_PARAMETER, NULL);
    ecs_os_zeromem(result);

    result->elem_size = flecs_ito(int16_t, params->size);
    result->allocator = params->allocator;

    if (params->entry_allocator.chunk_size) {
        result->entry_allocator = &params->entry_allocator;
        result->shared_allocator = true;
    } else {
        result->entry_allocator = flecs_ballocator_new(
            flecs_map_chunk_size(params->size));
    }

    flecs_map_rehash(result, params->initial_count);
}

void _ecs_map_init_w_params_if(
    ecs_map_t *result,
    ecs_map_params_t *params)
{
    if (ecs_map_is_init(result)) {
        ecs_assert(params->size == result->elem_size, 
            ECS_INVALID_PARAMETER, NULL);
        return;
    }
    _ecs_map_init_w_params(result, params);
}

void _ecs_map_init(
    ecs_map_t *result,
    ecs_size_t elem_size,
    ecs_allocator_t *allocator,
    int32_t element_count)
{
    _ecs_map_init_w_params(result, &(ecs_map_params_t) {
        .size = elem_size,
        .initial_count = element_count,
        .allocator = allocator
    });
}

void _ecs_map_init_if(
    ecs_map_t *result,
    ecs_size_t elem_size,
    ecs_allocator_t *allocator,
    int32_t element_count)
{
    if (ecs_map_is_init(result)) {
        ecs_assert(elem_size == result->elem_size, ECS_INVALID_PARAMETER, NULL);
        return;
    }
    _ecs_map_init(result, elem_size, allocator, element_count);
}

ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    ecs_allocator_t *allocator,
    int32_t element_count)
{
    ecs_map_t *result = ecs_os_calloc_t(ecs_map_t);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    _ecs_map_init(result, elem_size, allocator, element_count);
    return result;
}

void ecs_map_fini(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INTERNAL_ERROR, NULL);
#ifdef FLECS_SANITIZE
    bool sanitize = true;
#else
    bool sanitize = false;
#endif

    /* Free buckets in sanirized mode, so we can replace the allocator with
     * regular malloc/free and use asan/valgrind to find memory errors. */
    if (map->shared_allocator || sanitize) {
        ecs_bucket_t *bucket = map->buckets;
        while ((bucket != map->buckets_end)) {
            ecs_bucket_entry_t *entry = bucket->first;
            while (entry) {
                ecs_bucket_entry_t *next = entry->next;
                flecs_bfree(map->entry_allocator, entry);
                entry = next;
            }
            bucket ++;
        }
    }

    if (map->entry_allocator && !map->shared_allocator) {
        flecs_ballocator_free(map->entry_allocator);
        map->entry_allocator = NULL;
    }
    if (map->allocator) {
        flecs_free_n(map->allocator, ecs_bucket_t, map->bucket_count, 
            map->buckets);
    } else {
        ecs_os_free(map->buckets);
    }

    map->bucket_shift = 0;
    ecs_assert(!ecs_map_is_init(map), ECS_INTERNAL_ERROR, NULL);
}

void ecs_map_free(
    ecs_map_t *map)
{
    if (map) {
        ecs_map_fini(map);
        ecs_os_free(map);
    }
}

void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    ecs_assert(elem_size == map->elem_size, ECS_INVALID_PARAMETER, NULL);
    (void)elem_size;
    return flecs_map_get_from_bucket(flecs_map_get_bucket(map, key), key);
}

void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    void* ptr_ptr = _ecs_map_get(map, ECS_SIZEOF(void*), key);

    if (ptr_ptr) {
        return *(void**)ptr_ptr;
    } else {
        return NULL;
    }
}

void* _ecs_map_insert(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    ecs_assert(_ecs_map_get(map, elem_size, key) == NULL, 
        ECS_INVALID_PARAMETER, NULL);

    int32_t map_count = ++map->count;
    int32_t target_bucket_count = flecs_map_get_bucket_count(map_count);
    int32_t map_bucket_count = map->bucket_count;
    if (target_bucket_count > map_bucket_count) {
        flecs_map_rehash(map, target_bucket_count);
    }

    ecs_bucket_t *bucket = flecs_map_get_bucket(map, key);
    return flecs_map_add_to_bucket(map->entry_allocator, bucket, key);
}

void* _ecs_map_ensure(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    ecs_bucket_t *bucket = flecs_map_get_bucket(map, key);

    void *elem = flecs_map_get_from_bucket(bucket, key);
    if (!elem) {
        int32_t map_count = ++map->count;
        int32_t target_bucket_count = flecs_map_get_bucket_count(map_count);
        int32_t map_bucket_count = map->bucket_count;

        if (target_bucket_count > map_bucket_count) {
            flecs_map_rehash(map, target_bucket_count);
            bucket = flecs_map_get_bucket(map, key);
            elem = flecs_map_add_to_bucket(map->entry_allocator, bucket, key);
            ecs_assert(elem != NULL, ECS_INVALID_PARAMETER, NULL);
        } else {
            elem = flecs_map_add_to_bucket(map->entry_allocator, bucket, key);
        }

        ecs_os_memset(elem, 0, elem_size);
    }

    return elem;
}

int32_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t *bucket = flecs_map_get_bucket(map, key);
    if (flecs_map_remove_from_bucket(map->entry_allocator, bucket, key)) {
        return --map->count;
    }
    
    return map->count;
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t i, count = map->bucket_count;
    for (i = 0; i < count; i ++) {
        flecs_map_clear_bucket(map->entry_allocator, map->buckets[i].first);
    }
    if (map->allocator) {
        flecs_free_n(map->allocator, ecs_bucket_t, count, map->buckets);
    } else {
        ecs_os_free(map->buckets);
    }
    map->buckets = NULL;
    map->buckets_end = NULL;
    map->bucket_count = 0;
    map->count = 0;
    flecs_map_rehash(map, 2);
}

ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    return (ecs_map_iter_t){
        .map = map,
        .bucket = NULL,
        .entry = NULL
    };
}

void* _ecs_map_next(
    ecs_map_iter_t *iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key_out)
{
    (void)elem_size;
    const ecs_map_t *map = iter->map;
    if (!ecs_map_is_init(map)) {
        return NULL;
    }
    if (iter->bucket == map->buckets_end) {
        return NULL;
    }

    ecs_assert(!elem_size || elem_size == map->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_entry_t *entry = NULL;
    
    if (!iter->bucket) {
        for (iter->bucket = map->buckets; 
            iter->bucket != map->buckets_end; 
            ++iter->bucket) 
        {
            if (iter->bucket->first) {
                entry = iter->bucket->first;
                break;
            }
        }
        if (iter->bucket == map->buckets_end) {
            return NULL;
        }
    } else if ((entry = iter->entry) == NULL) {
        do {
            ++iter->bucket;
            if (iter->bucket == map->buckets_end) {
                return NULL;
            }
        } while(!iter->bucket->first);
        entry = iter->bucket->first;
    }
    
    if (key_out) {
        ecs_assert(entry != NULL, ECS_INTERNAL_ERROR, NULL);
        *key_out = entry->key;
    }

    iter->entry = entry->next;

    return ECS_OFFSET(&entry->key, ECS_SIZEOF(ecs_map_key_t));
}

void* _ecs_map_next_ptr(
    ecs_map_iter_t *iter,
    ecs_map_key_t *key_out)
{
    void *result = _ecs_map_next(iter, ECS_SIZEOF(void*), key_out);
    if (result) {
        return *(void**)result;
    } else {
        return NULL;
    }
}

ecs_map_t* ecs_map_copy(
    ecs_map_t *map)
{
    if (!ecs_map_is_init(map)) {
        return NULL;
    }

    ecs_size_t elem_size = map->elem_size;
    ecs_map_t *result = _ecs_map_new(map->elem_size,
        map->allocator,
        ecs_map_count(map));

    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_map_key_t key;
    void *ptr;
    while ((ptr = _ecs_map_next(&it, elem_size, &key))) {
        void *elem = _ecs_map_ensure(result, elem_size, key);
        ecs_os_memcpy(elem, ptr, elem_size);
    }

    return result;
}
