#include "types.h"

#define LOAD_FACTOR (1.5)
#define KEY_SIZE (sizeof(ecs_map_key_t))
#define BUCKET_COUNT (8)
#define CHUNK_SIZE (64)
#define ELEM_SIZE(payload_size) (KEY_SIZE + payload_size)
#define BUCKET_SIZE(payload_size)\
    (sizeof(ecs_bucket_t) + BUCKET_COUNT * (ELEM_SIZE(payload_size)))

#define NEXT_ELEM(elem, payload_size) \
    ECS_OFFSET(elem, ELEM_SIZE(payload_size))

#define GET_ELEM(array, payload_size, index) \
    ECS_OFFSET(array, ELEM_SIZE(payload_size) * index)

#define PAYLOAD_ARRAY(bucket) \
    ECS_OFFSET(bucket, sizeof(ecs_bucket_t))

#define PAYLOAD(elem) \
    ECS_OFFSET(elem, KEY_SIZE)

struct ecs_bucket_t {
    uint32_t count;
};

struct ecs_map_t {
    ecs_sparse_t *buckets;
    uint32_t payload_size;
    uint32_t bucket_size;
    uint32_t bucket_count;
    uint32_t count;
};

static
uint32_t next_pow_of_2(
    uint32_t n)
{
    n --;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n ++;

    return n;
}

static
uint32_t get_bucket_count(
    uint32_t element_count)
{
    return next_pow_of_2((float)element_count * LOAD_FACTOR);
}

static
uint32_t get_bucket_id(
    uint32_t bucket_count,
    ecs_map_key_t key) 
{
    return key & (bucket_count - 1);
}

static
ecs_bucket_t* find_bucket(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_sparse_t *buckets = map->buckets;
    uint32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        return NULL;
    }

    uint32_t bucket_id = get_bucket_id(bucket_count, key);

    return _ecs_sparse_get_sparse(buckets, 0, bucket_id);
}

static
ecs_bucket_t* find_or_create_bucket(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_sparse_t *buckets = map->buckets;
    uint32_t bucket_count = map->bucket_count;

    if (!bucket_count) {
        ecs_sparse_set_size(buckets, 8);
        bucket_count = 8;
    }

    uint32_t bucket_id = get_bucket_id(bucket_count, key);
    
    bool is_new = false;
    ecs_bucket_t *bucket = _ecs_sparse_get_or_set_sparse(
        buckets, 0, bucket_id, &is_new);

    if (is_new) {
        bucket->count = 0;
    }

    return bucket;    
}

static
void remove_bucket(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    uint32_t bucket_count = map->bucket_count;
    uint32_t bucket_id = get_bucket_id(bucket_count, key);
    _ecs_sparse_remove(map->buckets, 0, bucket_id);
}

static
uint32_t add_to_bucket(
    ecs_bucket_t *bucket,
    uint32_t payload_size,
    ecs_map_key_t key,
    void *payload)
{
    ecs_assert(bucket->count < BUCKET_COUNT, ECS_INTERNAL_ERROR, NULL);

    void *array = PAYLOAD_ARRAY(bucket);
    ecs_map_key_t *elem = GET_ELEM(array, payload_size, bucket->count);
    *elem = key;
    memcpy(PAYLOAD(elem), payload, payload_size);
    return ++ bucket->count;
}

static
void remove_from_bucket(
    ecs_bucket_t *bucket,
    uint32_t payload_size,
    ecs_map_key_t key,
    uint32_t index)
{
    ecs_assert(bucket->count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(index < bucket->count, ECS_INTERNAL_ERROR, NULL);

    bucket->count--;

    if (index != bucket->count) {
        void *array = PAYLOAD_ARRAY(bucket);
        ecs_map_key_t *elem = GET_ELEM(array, payload_size, index);
        ecs_map_key_t *last_elem = GET_ELEM(array, payload_size, bucket->count);

        ecs_assert(key == *elem, ECS_INTERNAL_ERROR, NULL);

        memcpy(elem, last_elem, ELEM_SIZE(payload_size));
    }
}

static
void rehash(
    ecs_map_t *map,
    uint32_t bucket_count)
{
    bool rehash_again;

    ecs_assert(bucket_count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(bucket_count > map->bucket_count, ECS_INTERNAL_ERROR, NULL);

    do {
        rehash_again = false;

        ecs_sparse_t *buckets = map->buckets;
        uint32_t payload_size = map->payload_size;

        ecs_sparse_set_size(buckets, bucket_count);
        map->bucket_count = bucket_count;

        /* Only iterate over old buckets with elements */
        int32_t b, filled_bucket_count = ecs_sparse_count(buckets);
        const uint32_t *indices = ecs_sparse_indices(buckets);

        /* Iterate backwards as elements could otherwise be moved to existing
         * buckets which could temporarily cause the number of elements in a
         * bucket to exceed BUCKET_COUNT. */
        for (b = filled_bucket_count - 1; b >= 0; b --) {
            uint32_t bucket_id = indices[b];
            ecs_bucket_t *bucket = _ecs_sparse_get_sparse(buckets, 0, bucket_id);

            int i, count = bucket->count;
            ecs_map_key_t *array = PAYLOAD_ARRAY(bucket);

            for (i = 0; i < count; i ++) {
                ecs_map_key_t *elem = GET_ELEM(array, payload_size, i);
                ecs_map_key_t key = *elem;
                uint32_t new_bucket_id = get_bucket_id(bucket_count, key);

                if (new_bucket_id != bucket_id) {
                    bool is_new = false;
                    ecs_bucket_t *new_bucket = _ecs_sparse_get_or_set_sparse(
                        buckets, 0, new_bucket_id, &is_new);

                    if (is_new) {
                        new_bucket->count = 0;
                        indices = ecs_sparse_indices(buckets);
                    }

                    if (add_to_bucket(
                        new_bucket, payload_size, key, PAYLOAD(elem)) == 
                        BUCKET_COUNT) 
                    {
                        rehash_again = true;
                    }

                    remove_from_bucket(bucket, payload_size, key, i);

                    count --;
                    i --;
                }
            }
        }

        bucket_count *= 2;        
    } while (rehash_again);
}

ecs_map_t* _ecs_map_new(
    size_t payload_size, 
    uint32_t element_count)
{
    ecs_map_t *result = ecs_os_calloc(sizeof(ecs_map_t), 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    uint32_t bucket_count = get_bucket_count(element_count);

    result->count = 0;
    result->payload_size = payload_size;
    result->bucket_count = bucket_count;
    result->buckets = _ecs_sparse_new(BUCKET_SIZE(payload_size), bucket_count);

    return result;
}

void ecs_map_free(
    ecs_map_t *map)
{
    if (map) {
        ecs_sparse_free(map->buckets);
        free(map);
    }
}

void* _ecs_map_get(
    const ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key)
{
    if (!map) {
        return NULL;
    }

    ecs_assert(payload_size == map->payload_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_bucket(map, key);

    if (!bucket) {
        return NULL;
    }

    ecs_map_key_t *elem = PAYLOAD_ARRAY(bucket);

    uint8_t i = 0;
    while (i++ < bucket->count) {
        if (*elem == key) {
            return PAYLOAD(elem);
        }

        elem = NEXT_ELEM(elem, payload_size);
    }

    return NULL;
}

bool _ecs_map_has(
    const ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key,
    void *payload)
{
    const void *result = _ecs_map_get(map, payload_size, key);
    if (result) {
        memcpy(payload, result, payload_size);
        return true;
    } else {
        return false;
    }
}

void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    void * ptr_ptr = _ecs_map_get(map, sizeof(void*), key);

    if (ptr_ptr) {
        return *(void**)ptr_ptr;
    } else {
        return NULL;
    }
}

void _ecs_map_set(
    ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key,
    const void *payload)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(payload_size == map->payload_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_or_create_bucket(map, key);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);
    
    uint32_t bucket_count = bucket->count;
    void *array = PAYLOAD_ARRAY(bucket);
    ecs_map_key_t *elem = array, *found = NULL;

    uint8_t i = 0;
    while (i++ < bucket_count) {
        if (*elem == key) {
            found = elem;
            break;
        }

        elem = NEXT_ELEM(elem, payload_size);
    }

    if (!found) {
        ecs_assert(bucket->count < BUCKET_COUNT, ECS_INTERNAL_ERROR, NULL);

        uint32_t bucket_count = ++bucket->count;
        uint32_t map_count = ++map->count;
        
        *elem = key;
        memcpy(PAYLOAD(elem), payload, payload_size);

        uint32_t target_bucket_count = get_bucket_count(map_count);
        uint32_t map_bucket_count = map->bucket_count;

        if (bucket_count == BUCKET_COUNT) {
            rehash(map, map_bucket_count * 2);
        } else {
            if (target_bucket_count > map_bucket_count) {
                rehash(map, target_bucket_count);
            }
        }
    } else {
        *found = key;
        memcpy(PAYLOAD(found), payload, payload_size);
    }

    ecs_assert(map->bucket_count != 0, ECS_INTERNAL_ERROR, NULL);
}

void ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_bucket(map, key);
    if (!bucket) {
        return;
    }
   
    uint32_t payload_size = map->payload_size;
    void *array = PAYLOAD_ARRAY(bucket);
    ecs_map_key_t *elem = array;
    uint32_t bucket_count = bucket->count;

    ecs_assert(bucket_count > 0, ECS_INTERNAL_ERROR, NULL);

    uint8_t i = 0;
    do {
        if (*elem == key) {
            ecs_map_key_t *last_elem = GET_ELEM(array, payload_size, (bucket_count - 1));
            if (last_elem > elem) {
                memcpy(elem, last_elem, ELEM_SIZE(payload_size));
            }

            map->count --;
            if (!--bucket->count) {
                remove_bucket(map, key);
            }

            break;
        }

        elem = NEXT_ELEM(elem, payload_size);
    } while (++i < bucket_count);   
}

uint32_t ecs_map_count(
    const ecs_map_t *map)
{
    return map ? map->count : 0;
}

uint32_t ecs_map_bucket_count(
    const ecs_map_t *map)
{
    return map ? map->bucket_count : 0;
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_sparse_clear(map->buckets);
    map->count = 0;
}

ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    return (ecs_map_iter_t){
        .map = map,
        .bucket = NULL,
        .bucket_index = 0,
        .element_index = 0
    };
}

void* _ecs_map_next(
    ecs_map_iter_t *iter,
    size_t payload_size,
    ecs_map_key_t *key_out)
{
    const ecs_map_t *map = iter->map;
    
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(payload_size == map->payload_size, ECS_INVALID_PARAMETER, NULL);
 
    ecs_bucket_t *bucket = iter->bucket;
    uint32_t element_index = iter->element_index;

    do {
        if (!bucket) {
            uint32_t bucket_index = iter->bucket_index;
            ecs_sparse_t *buckets = map->buckets;
            if (bucket_index < ecs_sparse_count(buckets)) {
                bucket = _ecs_sparse_get(buckets, 0, bucket_index);
                iter->bucket = bucket;

                element_index = 0;
                iter->element_index = 0;
            } else {
                return NULL;
            }
        }

        if (element_index < bucket->count) {
            iter->element_index = element_index + 1;
            break;
        } else {
            bucket = NULL;
            iter->bucket_index ++;
        }
    } while (true);

    void *array = PAYLOAD_ARRAY(bucket);
    ecs_map_key_t *elem = GET_ELEM(array, payload_size, element_index);
    
    if (key_out) {
        *key_out = *elem;
    }

    return PAYLOAD(elem);
}

void* _ecs_map_next_ptr(
    ecs_map_iter_t *iter,
    ecs_map_key_t *key_out)
{
    void *result = _ecs_map_next(iter, sizeof(void*), key_out);
    if (result) {
        return *(void**)result;
    } else {
        return NULL;
    }
}

void ecs_map_grow(
    ecs_map_t *map, 
    uint32_t element_count)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    uint32_t target_count = map->count + element_count;
    uint32_t bucket_count = get_bucket_count(target_count);

    if (bucket_count > map->bucket_count) {
        rehash(map, bucket_count);
    }
}

void ecs_map_set_size(
    ecs_map_t *map, 
    uint32_t element_count)
{    
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    uint32_t bucket_count = get_bucket_count(element_count);
    rehash(map, bucket_count);
}

void ecs_map_memory(
    ecs_map_t *map, 
    uint32_t *allocd,
    uint32_t *used)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_sparse_memory(map->buckets, allocd, NULL);

    if (used) {
        *used = map->count * ELEM_SIZE(map->payload_size);
    }
}

ecs_map_t* ecs_map_copy(
    const ecs_map_t *src)
{
    ecs_map_t *dst = ecs_os_memdup(src, sizeof(ecs_map_t));
    dst->buckets = ecs_os_memdup(
        src->buckets, src->bucket_count * sizeof(uint32_t));
    
    dst->buckets = ecs_sparse_copy(src->buckets);

    return dst;
}
