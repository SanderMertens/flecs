#include <reflecs/map.h>
#include <reflecs/vector.h>

#define REFLECS_LOAD_FACTOR (2.0f / 3.0f)

typedef struct EcsMapData {
    uint64_t key_hash;
    const void *data;
} EcsMapData;

typedef struct EcsMapBucket {
    EcsVector *elems;
} EcsMapBucket;

struct EcsMap {
    EcsMapBucket *buckets;  /* array of buckets */
    size_t bucket_count;    /* number of buckets */
    uint32_t count;         /* number of elements */
};

const EcsVectorParams bucket_vec_params = {
    .element_size = sizeof(void*),
    .chunk_count = 4
};

static
EcsMap *ecs_map_alloc(
    uint32_t bucket_count)
{
    EcsMap *result = malloc(sizeof(EcsMap));
    result->buckets = calloc(bucket_count * sizeof(EcsMapBucket), 1);
    result->bucket_count = bucket_count;
    result->count = 0;
    return result;
}

static
void ecs_map_add_elem(
    EcsMap *map,
    EcsMapBucket *bucket,
    uint64_t key_hash,
    const void *data)
{
    EcsMapData *elem = ecs_vector_add(bucket->elems, &bucket_vec_params);
    elem->key_hash = key_hash;
    elem->data = data;
    map->count ++;
}

static
EcsMapBucket *ecs_map_get_bucket(
    EcsMap *map,
    uint64_t key_hash)
{
    uint64_t index = key_hash % map->bucket_count;
    return &map->buckets[index];
}

static
EcsMapData *ecs_map_get_elem(
    EcsMapBucket *bucket,
    uint64_t key_hash)
{
    EcsIter it = ecs_vector_iter(bucket->elems, &bucket_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsMapData *data = ecs_iter_next(&it);
        if (data->key_hash == key_hash) {
            return data;
        }
    }

    return NULL;
}

static
uint32_t ecs_map_next_bucket(
    EcsMap *map,
    uint32_t start_index)
{
    int i;
    for (i = start_index; i < map->bucket_count; i ++) {
        if (map->buckets[i].elems) {
            break;
        }
    }

    return i;
}

static
void ecs_map_get_bucket_iter(
    EcsMap *map,
    EcsMapIter *iter_data,
    uint32_t bucket_index)
{
    iter_data->bucket_iter =
        _ecs_vector_iter(
            map->buckets[bucket_index].elems,
            &bucket_vec_params,
            &iter_data->bucket_iter_data);
}

static
bool ecs_map_hasnext(
    EcsIter *iter)
{
    EcsMap *map = iter->data;
    EcsMapIter *iter_data = iter->ctx;
    uint32_t bucket_index = iter_data->bucket_index;

    if (!map->buckets[bucket_index].elems) {
        bucket_index = ecs_map_next_bucket(map, bucket_index + 1);
        if (bucket_index >= map->bucket_count) {
            return false;
        }

        ecs_map_get_bucket_iter(map, iter_data, bucket_index);
    }

    if (!iter_data->bucket_iter.hasnext) {
        ecs_map_get_bucket_iter(map, iter_data, bucket_index);
    }

    if (ecs_iter_hasnext(&iter_data->bucket_iter)) {
        iter_data->bucket_index = bucket_index;
        return true;
    } else {
        bucket_index = ecs_map_next_bucket(map, bucket_index + 1);
        if (bucket_index < map->bucket_count) {
            ecs_map_get_bucket_iter(map, iter_data, bucket_index);
            iter_data->bucket_index = bucket_index;
            return ecs_iter_hasnext(&iter_data->bucket_iter);
        } else {
            return false;
        }
    }
}

static
void *ecs_map_next(
    EcsIter *iter)
{
    EcsMapIter *iter_data = iter->ctx;
    EcsMapData *elem = ecs_iter_next(&iter_data->bucket_iter);
    return (void*)elem->data;
}

static
void ecs_map_resize(
    EcsMap *map,
    uint32_t bucket_count)
{
    EcsMapBucket *old_buckets = map->buckets;
    uint32_t old_bucket_count = map->bucket_count;

    map->buckets = malloc(sizeof(EcsMapBucket) * bucket_count);
    map->bucket_count = bucket_count;
    map->count = 0;

    uint32_t bucket_index;
    for (bucket_index = 0; bucket_index < old_bucket_count; bucket_index ++) {
        EcsMapBucket *bucket = &old_buckets[bucket_index];
        if (bucket->elems) {
            EcsIter it = ecs_vector_iter(bucket->elems, &bucket_vec_params);
            while (ecs_iter_hasnext(&it)) {
                EcsMapData *data = ecs_iter_next(&it);
                ecs_map_set(map, data->key_hash, data->data);
            }
            ecs_vector_free(bucket->elems);
        }
    }

    free(old_buckets);
}

EcsMap* ecs_map_new(
    uint32_t size)
{
    return ecs_map_alloc(size);
}

void ecs_map_clear(
    EcsMap *map)
{
    int i;
    for (i = 0; i < map->bucket_count; i ++) {
        if (map->buckets[i].elems) {
            ecs_vector_free(map->buckets[i].elems);
            map->buckets[i].elems = NULL;
        }
    }
}

void ecs_map_free(
    EcsMap *map)
{
    ecs_map_clear(map);
    free(map);
}

void ecs_map_set(
    EcsMap *map,
    uint64_t key_hash,
    const void *data)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (!bucket->elems) {
        bucket->elems = ecs_vector_new(&bucket_vec_params);;
        ecs_map_add_elem(map, bucket, key_hash, data);
    } else {
        EcsMapData *elem = ecs_map_get_elem(bucket, key_hash);
        if (elem) {
            elem->data = data;
        } else {
            ecs_map_add_elem(map, bucket, key_hash, data);
        }
    }

    if ((float)map->count / (float)map->bucket_count > REFLECS_LOAD_FACTOR) {
        ecs_map_resize(map, map->bucket_count * 2);
    }
}

void ecs_map_remove(
    EcsMap *map,
    uint64_t key_hash)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (bucket->elems) {
        EcsMapData *elem = ecs_map_get_elem(bucket, key_hash);
        if (elem) {
            ecs_vector_remove(bucket->elems, &bucket_vec_params, elem);
            if (!ecs_vector_count(bucket->elems)) {
                ecs_vector_free(bucket->elems);
                bucket->elems = NULL;
            }
        }
    }
}

void* ecs_map_get(
    EcsMap *map,
    uint64_t key_hash)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (bucket->elems) {
        EcsMapData *elem = ecs_map_get_elem(bucket, key_hash);
        if (elem) {
            return (void*)elem->data;
        }
    }

    return NULL;
}


uint32_t ecs_map_count(
    EcsMap *map)
{
    return map->count;
}

uint32_t ecs_map_bucket_count(
    EcsMap *map)
{
    return map->bucket_count;
}

EcsIter _ecs_map_iter(
    EcsMap *map,
    EcsMapIter *iter_data)
{
    EcsIter result = {
        .data = map,
        .ctx = iter_data,
        .hasnext = ecs_map_hasnext,
        .next = ecs_map_next,
        .release = NULL
    };

    iter_data->bucket_iter.hasnext = NULL;
    iter_data->bucket_iter.next = NULL;
    iter_data->bucket_iter.release = NULL;
    iter_data->bucket_index = 0;

    return result;
}
