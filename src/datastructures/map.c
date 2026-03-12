/**
 * @file datastructures/map.c
 * @brief Hash map with 64-bit keys and 64-bit values.
 */

#include "../private_api.h"

/* Rehash when element_count * LOAD_FACTOR * 0.1 exceeds bucket_count.
 * With LOAD_FACTOR=12, this triggers at ~0.83 elements per bucket. */
#define ECS_LOAD_FACTOR (12)
#define ECS_BUCKET_END(b, c) ECS_ELEM_T(b, ecs_bucket_t, c)
#define ECS_MAP_ALLOC(a, T) a ? flecs_alloc_t(a, T) : ecs_os_malloc_t(T)
#define ECS_MAP_CALLOC_N(a, T, n) a ? flecs_calloc_n(a, T, n) : ecs_os_calloc_n(T, n)
#define ECS_MAP_FREE(a, T, ptr) a ? flecs_free_t(a, T, ptr) : ecs_os_free(ptr)
#define ECS_MAP_FREE_N(a, T, n, ptr) a ? flecs_free_n(a, T, n, ptr) : ecs_os_free(ptr)

/* Compute floor(log2(v)) using De Bruijn multiplication.
 * Rounds v up to the next power-of-2 minus one, then uses a magic
 * constant to map the top 5 bits to the correct log2 value. */
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

static
int32_t flecs_map_get_bucket_count(
    int32_t count)
{
    return flecs_next_pow_of_2((int32_t)(count * ECS_LOAD_FACTOR * 0.1));
}

static
uint8_t flecs_map_get_bucket_shift(
    int32_t bucket_count)
{
    return (uint8_t)(64u - flecs_log2((uint32_t)bucket_count));
}

/* Get bucket index for key using Fibonacci hashing. */
static
int32_t flecs_map_get_bucket_index(
    uint16_t bucket_shift,
    ecs_map_key_t key)
{
    ecs_assert(bucket_shift != 0, ECS_INTERNAL_ERROR, NULL);
    /* 11400714819323198485 = 2^64 / golden ratio (Fibonacci hashing constant) */
    return (int32_t)((11400714819323198485ull * key) >> bucket_shift);
}

static
ecs_bucket_t* flecs_map_get_bucket(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t bucket_id = flecs_map_get_bucket_index((uint16_t)map->bucket_shift, key);
    ecs_assert(bucket_id < map->bucket_count, ECS_INTERNAL_ERROR, NULL);
    return &map->buckets[bucket_id];
}

static
ecs_map_val_t* flecs_map_bucket_add(
    ecs_allocator_t *a,
    ecs_bucket_t *bucket,
    ecs_map_key_t key)
{
    ecs_bucket_entry_t *new_entry = ECS_MAP_ALLOC(a, ecs_bucket_entry_t);
    new_entry->key = key;
    new_entry->next = bucket->first;
    bucket->first = new_entry;
    return &new_entry->value;
}

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
            ECS_MAP_FREE(map->allocator, ecs_bucket_entry_t, entry);
            map->count --;
            return value;
        }
    }
    
    return 0;
}

static
void flecs_map_bucket_clear(
    ecs_allocator_t *allocator,
    ecs_bucket_t *bucket)
{
    ecs_bucket_entry_t *entry = bucket->first;
    while(entry) {
        ecs_bucket_entry_t *next = entry->next;
        ECS_MAP_FREE(allocator, ecs_bucket_entry_t, entry);
        entry = next;
    }
}

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

static
void flecs_map_rehash(
    ecs_map_t *map,
    int32_t count)
{
    count = flecs_next_pow_of_2(count);
    if (count < 2) {
        count = 2;
    }
    
    int32_t old_count = map->bucket_count;
    ecs_bucket_t *buckets = map->buckets, *b, *end = ECS_BUCKET_END(buckets, old_count);

    map->buckets = ECS_MAP_CALLOC_N(map->allocator, ecs_bucket_t, count);
    map->bucket_count = count;
    map->bucket_shift = flecs_map_get_bucket_shift(count) & 0x3fu;

    /* Redistribute entries into new bucket array */
    for (b = buckets; b < end; b++) {
        ecs_bucket_entry_t* entry;
        for (entry = b->first; entry;) {
            ecs_bucket_entry_t* next = entry->next;
            int32_t bucket_index = flecs_map_get_bucket_index(
                (uint16_t)map->bucket_shift, entry->key);
            ecs_bucket_t *bucket = &map->buckets[bucket_index];
            entry->next = bucket->first;
            bucket->first = entry;
            entry = next;
        }
    }

    ECS_MAP_FREE_N(map->allocator, ecs_bucket_t, old_count, buckets);
}

/* Initialize a map with an initial bucket allocation. */
void ecs_map_init(
    ecs_map_t *result,
    ecs_allocator_t *allocator)
{
    ecs_os_zeromem(result);

    result->allocator = allocator;

    flecs_map_rehash(result, 0);
}

/* Initialize a map only if it has not been initialized yet. */
void ecs_map_init_if(
    ecs_map_t *result,
    ecs_allocator_t *allocator)
{
    if (!ecs_map_is_init(result)) {
        ecs_map_init(result, allocator);
    }   
}

/* Finalize a map and free all bucket memory. */
void ecs_map_fini(
    ecs_map_t *map)
{
    if (!ecs_map_is_init(map)) {
        return;
    }

    ecs_allocator_t *a = map->allocator;
    ecs_bucket_t *bucket = map->buckets, *end = &bucket[map->bucket_count];
    while (bucket != end) {
        flecs_map_bucket_clear(a, bucket);
        bucket ++;
    }

    map->bucket_shift = 0;

    ECS_MAP_FREE_N(a, ecs_bucket_t, map->bucket_count, map->buckets);
}

/* Get value pointer for key, or NULL if not found. */
ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    return flecs_map_bucket_get(flecs_map_get_bucket(map, key), key);
}

/* Get dereferenced pointer value for key, or NULL if not found. */
void* ecs_map_get_deref_(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_map_val_t* ptr = flecs_map_bucket_get(
        flecs_map_get_bucket(map, key), key);
    if (ptr) {
        return (void*)(uintptr_t)ptr[0];
    }
    return NULL;
}

/* Insert a new key-value pair. Asserts the key does not already exist. */
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
    flecs_map_bucket_add(map->allocator, bucket, key)[0] = value;

#ifdef FLECS_DEBUG
    ecs_os_linc(&map->change_count);
#endif
}

/* Insert a key with a newly heap-allocated value. */
void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    void *elem = ecs_os_calloc(elem_size);
    ecs_map_insert_ptr(map, key, (uintptr_t)elem);
    return elem;
}

/* Get or insert a zero-initialized value for the given key. */
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

    ecs_map_val_t* v = flecs_map_bucket_add(map->allocator, bucket, key);
    *v = 0;

#ifdef FLECS_DEBUG
    ecs_os_linc(&map->change_count);
#endif

    return v;
}

/* Get or insert a heap-allocated value for the given key. */
void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    ecs_map_val_t *val = ecs_map_ensure(map, key);
    if (!*val) {
        void *elem = ecs_os_calloc(elem_size);
        *val = (ecs_map_val_t)(uintptr_t)elem;
        return elem;
    } else {
        return (void*)(uintptr_t)*val;
    }
}

/* Remove a key and return its value (0 if not found). */
ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
#ifdef FLECS_DEBUG
    if (map->last_iterated != key) {
        ecs_os_linc(&map->change_count);
    }
#endif    
    return flecs_map_bucket_remove(map, flecs_map_get_bucket(map, key), key);
}

/* Shrink bucket array to match current element count. */
void ecs_map_reclaim(
    ecs_map_t *map)
{
    int32_t tgt_bucket_count = flecs_map_get_bucket_count(map->count - 1);
    if (tgt_bucket_count != map->bucket_count) {
        flecs_map_rehash(map, tgt_bucket_count);
    }
}

/* Remove a key and free its pointer value if non-NULL. */
void ecs_map_remove_free(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_map_val_t val = ecs_map_remove(map, key);
    if (val) {
        ecs_os_free((void*)(uintptr_t)val);
    }
}

/* Remove all elements from the map and rehash. */
void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t i, count = map->bucket_count;
    for (i = 0; i < count; i ++) {
        flecs_map_bucket_clear(map->allocator, &map->buckets[i]);
    }
    ECS_MAP_FREE_N(map->allocator, ecs_bucket_t, count, map->buckets);
    map->buckets = NULL;
    map->bucket_count = 0;
    map->count = 0;
    flecs_map_rehash(map, 2);

#ifdef FLECS_DEBUG
    ecs_os_linc(&map->change_count);
#endif
}

/* Create an iterator positioned before the first element. */
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    if (ecs_map_is_init(map)) {
        return (ecs_map_iter_t){
            .map = map,
            .bucket = NULL,
            .entry = NULL,
#ifdef FLECS_DEBUG
            .change_count = map->change_count
#endif
        };
    } else {
        return (ecs_map_iter_t){ 0 };
    }
}

/* Return whether the iterator is still valid (no modifications during iteration). */
bool ecs_map_iter_valid(
    ecs_map_iter_t *iter)
{
    const ecs_map_t *map = iter->map;
    if (!map) {
        return false;
    }

#ifdef FLECS_DEBUG
    if (map->change_count != iter->change_count) {
        return false;
    }
#endif

    return true;
}

/* Advance iterator to next element. Returns false when done. */
bool ecs_map_next(
    ecs_map_iter_t *iter)
{
    const ecs_map_t *map = iter->map;
    if (!map) {
        return false;
    }

    ecs_dbg_assert(map->change_count == iter->change_count, ECS_INVALID_PARAMETER, 
        "map cannot be modified while it is being iterated");

    ecs_bucket_t *end;
    if (iter->bucket == (end = &map->buckets[map->bucket_count])) {
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

#ifdef FLECS_DEBUG
    /* Track current key to detect removal of non-current elements */
    ECS_CONST_CAST(ecs_map_t*, map)->last_iterated = entry->key;
#endif

    return true;
}

/* Copy all entries from src to dst. dst must be empty or uninitialized. */
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
