#include <ecs/map.h>
#include <ecs/vector.h>

#define ECS_LOAD_FACTOR (2f / 3f)

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
    uint32_t size)
{
    EcsMap *result = malloc(sizeof(EcsMap));
    result->buckets = malloc(size * sizeof(EcsMapBucket));
    result->bucket_count = size;
    result->count = 0;
    return result;
}

static
void ecs_map_add_elem(
    EcsMapBucket *bucket,
    uint64_t key_hash,
    const void *data)
{
    EcsMapData *elem = ecs_vector_add(bucket->elems, &bucket_vec_params);
    elem->key_hash = key_hash;
    elem->data = data;
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

void ecs_map_add(
    EcsMap *map,
    uint64_t key_hash,
    const void *data)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (!bucket->elems) {
        bucket->elems = ecs_vector_new(&bucket_vec_params);;
        ecs_map_add_elem(bucket, key_hash, data);
    } else {
        EcsMapData *elem = ecs_map_get_elem(bucket, key_hash);
        if (elem) {
            elem->data = data;
        } else {
            ecs_map_add_elem(bucket, key_hash, data);
        }
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
        }
    }
}
