/**
 * @file datastructures/map.c
 * @brief Map data structure.
 * 
 * Map data structure for 64bit keys and dynamic payload size.
 */

#include "../private_api.h"

/* The ratio used to determine whether the map should flecs_map_rehash. If
 * (element_count * ECS_LOAD_FACTOR) > bucket_count, bucket count is increased. */
#define ECS_LOAD_FACTOR (12)
#define ECS_BUCKET_END(b, c) ECS_ELEM_T(b, ecs_bucket_t, c)

static
uint8_t flecs_log2(uint32_t v) {
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
    return flecs_next_pow_of_2((int32_t)(count * ECS_LOAD_FACTOR * 0.1));
}

/* Get bucket shift amount for a given bucket count */
static
uint8_t flecs_map_get_bucket_shift (
    int32_t bucket_count)
{
    return (uint8_t)(64u - flecs_log2((uint32_t)bucket_count));
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
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t bucket_id = flecs_map_get_bucket_index(map->bucket_shift, key);
    ecs_assert(bucket_id < map->bucket_count, ECS_INTERNAL_ERROR, NULL);
    return &map->buckets[bucket_id];
}

/* Add element to bucket */
static
ecs_map_val_t* flecs_map_bucket_add(
    ecs_block_allocator_t *allocator,
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *new_entry = flecs_balloc(allocator);
    new_entry->key = key;
    new_entry->next = bucket->first;
    bucket->first = new_entry;
    return &new_entry->value;
}

/* Remove element from bucket */
static
ecs_map_val_t flecs_map_bucket_remove(
    ecs_map_t *map,
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *entry;
    for (entry = bucket->first; entry; entry = entry->next) {
        if (entry->key == key) {
            ecs_map_val_t value = entry->value;
            ecs_bucket_entry_t **next_holder = &bucket->first;
            while(*next_holder != entry) {
                next_holder = &(*next_holder)->next;
            }
            *next_holder = entry->next;
            flecs_bfree(map->entry_allocator, entry);
            map->count --;
            return value;
        }
    }
    
    return 0;
}

/* Free contents of bucket */
static
void flecs_map_bucket_clear(
    ecs_block_allocator_t *allocator,
    ecs_bucket_t *bucket)
{
    ecs_bucket_entry_t *entry = bucket->first;
    while(entry) {
        ecs_bucket_entry_t *next = entry->next;
        flecs_bfree(allocator, entry);
        entry = next;
    }
}

/* Get payload pointer for key from bucket */
static
ecs_map_val_t* flecs_map_bucket_get(
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *entry;
    for (entry = bucket->first; entry; entry = entry->next) {
        if (entry->key == key) {
            return &entry->value;
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
    
    int32_t old_count = map->bucket_count;
    ecs_bucket_t *buckets = map->buckets, *b, *end = ECS_BUCKET_END(buckets, old_count);

    if (map->allocator) {
        map->buckets = flecs_calloc_n(map->allocator, ecs_bucket_t, count);
    } else {
        map->buckets = ecs_os_calloc_n(ecs_bucket_t, count);
    }
    map->bucket_count = count;
    map->bucket_shift = flecs_map_get_bucket_shift(count);

    /* Remap old bucket entries to new buckets */
    for (b = buckets; b < end; b++) {
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

void ecs_map_params_init(
    ecs_map_params_t *params,
    ecs_allocator_t *allocator)
{
    params->allocator = allocator;
    flecs_ballocator_init_t(&params->entry_allocator, ecs_bucket_entry_t);
}

void ecs_map_params_fini(
    ecs_map_params_t *params)
{
    flecs_ballocator_fini(&params->entry_allocator);
}

void ecs_map_init_w_params(
    ecs_map_t *result,
    ecs_map_params_t *params)
{
    ecs_os_zeromem(result);

    result->allocator = params->allocator;

    if (params->entry_allocator.chunk_size) {
        result->entry_allocator = &params->entry_allocator;
        result->shared_allocator = true;
    } else {
        result->entry_allocator = flecs_ballocator_new_t(ecs_bucket_entry_t);
    }

    flecs_map_rehash(result, 0);
}

void ecs_map_init_w_params_if(
    ecs_map_t *result,
    ecs_map_params_t *params)
{
    if (!ecs_map_is_init(result)) {
        ecs_map_init_w_params(result, params);
    }
}

void ecs_map_init(
    ecs_map_t *result,
    ecs_allocator_t *allocator)
{
    ecs_map_init_w_params(result, &(ecs_map_params_t) {
        .allocator = allocator
    });
}

void ecs_map_init_if(
    ecs_map_t *result,
    ecs_allocator_t *allocator)
{
    if (!ecs_map_is_init(result)) {
        ecs_map_init(result, allocator);
    }   
}

void ecs_map_fini(
    ecs_map_t *map)
{
    if (!ecs_map_is_init(map)) {
        return;
    }

    bool sanitize = false;
#ifdef FLECS_SANITIZE
    sanitize = true;
#endif

    /* Free buckets in sanitized mode, so we can replace the allocator with
     * regular malloc/free and use asan/valgrind to find memory errors. */
    ecs_allocator_t *a = map->allocator;
    ecs_block_allocator_t *ea = map->entry_allocator;
    if (map->shared_allocator || sanitize) {
        ecs_bucket_t *bucket = map->buckets, *end = &bucket[map->bucket_count];
        while (bucket != end) {
            flecs_map_bucket_clear(ea, bucket);
            bucket ++;
        }
    }

    if (ea && !map->shared_allocator) {
        flecs_ballocator_free(ea);
        map->entry_allocator = NULL;
    }
    if (a) {
        flecs_free_n(a, ecs_bucket_t, map->bucket_count, map->buckets);
    } else {
        ecs_os_free(map->buckets);
    }

    map->bucket_shift = 0;
}

ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    return flecs_map_bucket_get(flecs_map_get_bucket(map, key), key);
}

void* _ecs_map_get_deref(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_map_val_t* ptr = flecs_map_bucket_get(
        flecs_map_get_bucket(map, key), key);
    if (ptr) {
        return (void*)ptr[0];
    }
    return NULL;
}

void ecs_map_insert(
    ecs_map_t *map,
    ecs_map_key_t key,
    ecs_map_val_t value)
{
    ecs_assert(ecs_map_get(map, key) == NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t map_count = ++map->count;
    int32_t tgt_bucket_count = flecs_map_get_bucket_count(map_count);
    int32_t bucket_count = map->bucket_count;
    if (tgt_bucket_count > bucket_count) {
        flecs_map_rehash(map, tgt_bucket_count);
    }

    ecs_bucket_t *bucket = flecs_map_get_bucket(map, key);
    flecs_map_bucket_add(map->entry_allocator, bucket, key)[0] = value;
}

void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    void *elem = ecs_os_calloc(elem_size);
    ecs_map_insert_ptr(map, key, elem);
    return elem;
}

ecs_map_val_t* ecs_map_ensure(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_bucket_t *bucket = flecs_map_get_bucket(map, key);
    ecs_map_val_t *result = flecs_map_bucket_get(bucket, key);
    if (result) {
        return result;
    }

    int32_t map_count = ++map->count;
    int32_t tgt_bucket_count = flecs_map_get_bucket_count(map_count);
    int32_t bucket_count = map->bucket_count;
    if (tgt_bucket_count > bucket_count) {
        flecs_map_rehash(map, tgt_bucket_count);
        bucket = flecs_map_get_bucket(map, key);
    }

    ecs_map_val_t* v = flecs_map_bucket_add(map->entry_allocator, bucket, key);
    *v = 0;
    return v;
}

void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    ecs_map_val_t *val = ecs_map_ensure(map, key);
    if (!*val) {
        void *elem = ecs_os_calloc(elem_size);
        *val = (ecs_map_val_t)elem;
        return elem;
    } else {
        return (void*)*val;
    }
}

ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    return flecs_map_bucket_remove(map, flecs_map_get_bucket(map, key), key);
}

void ecs_map_remove_free(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_map_val_t val = ecs_map_remove(map, key);
    if (val) {
        ecs_os_free((void*)val);
    }
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t i, count = map->bucket_count;
    for (i = 0; i < count; i ++) {
        flecs_map_bucket_clear(map->entry_allocator, &map->buckets[i]);
    }
    if (map->allocator) {
        flecs_free_n(map->allocator, ecs_bucket_t, count, map->buckets);
    } else {
        ecs_os_free(map->buckets);
    }
    map->buckets = NULL;
    map->bucket_count = 0;
    map->count = 0;
    flecs_map_rehash(map, 2);
}

ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    if (ecs_map_is_init(map)) {
        return (ecs_map_iter_t){
            .map = map,
            .bucket = NULL,
            .entry = NULL
        };
    } else {
        return (ecs_map_iter_t){ 0 };
    }
}

bool ecs_map_next(
    ecs_map_iter_t *iter)
{
    const ecs_map_t *map = iter->map;
    ecs_bucket_t *end;
    if (!map || (iter->bucket == (end = &map->buckets[map->bucket_count]))) {
        return false;
    }

    ecs_bucket_entry_t *entry = NULL;
    if (!iter->bucket) {
        for (iter->bucket = map->buckets; 
            iter->bucket != end;
            ++iter->bucket) 
        {
            if (iter->bucket->first) {
                entry = iter->bucket->first;
                break;
            }
        }
        if (iter->bucket == end) {
            return false;
        }
    } else if ((entry = iter->entry) == NULL) {
        do {
            ++iter->bucket;
            if (iter->bucket == end) {
                return false;
            }
        } while(!iter->bucket->first);
        entry = iter->bucket->first;
    }

    ecs_assert(entry != NULL, ECS_INTERNAL_ERROR, NULL);
    iter->entry = entry->next;
    iter->res = &entry->key;

    return true;
}

void ecs_map_copy(
    ecs_map_t *dst,
    const ecs_map_t *src)
{
    if (ecs_map_is_init(dst)) {
        ecs_assert(ecs_map_count(dst) == 0, ECS_INVALID_PARAMETER, NULL);
        ecs_map_fini(dst);
    }
    
    if (!ecs_map_is_init(src)) {
        return;
    }

    ecs_map_init(dst, src->allocator);

    ecs_map_iter_t it = ecs_map_iter(src);
    while (ecs_map_next(&it)) {
        ecs_map_insert(dst, ecs_map_key(&it), ecs_map_value(&it));
    }
}
