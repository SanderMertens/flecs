
#include "include/private/types.h"

#define REFLECS_LOAD_FACTOR (3.0f / 4.0f)

typedef struct EcsMapNode {
    uint64_t key_hash;
    const void *data;
    struct EcsMapNode *next;
} EcsMapNode;

typedef struct EcsMapBucket {
    EcsMapNode *nodes;
} EcsMapBucket;

struct EcsMap {
    EcsMapBucket *buckets;  /* array of buckets */
    size_t bucket_count;    /* number of buckets */
    uint32_t count;         /* number of elements */
    EcsVector *data_elems;  /* Vector with memory for data elements */
};

const EcsVectorParams bucket_vec_params = {
    .chunk_count = 128,
    .element_size = sizeof(EcsMapNode)
};

static
void ecs_map_alloc_buffer(
    EcsMap *map,
    uint32_t bucket_count)
{
    if (bucket_count) {
        map->buckets = calloc(bucket_count * sizeof(EcsMapBucket), 1);
    } else {
        map->buckets = NULL;
    }

    map->bucket_count = bucket_count;
}

static
EcsMap *ecs_map_alloc(
    uint32_t bucket_count)
{
    EcsMap *result = malloc(sizeof(EcsMap));
    ecs_map_alloc_buffer(result, bucket_count);
    result->count = 0;
    result->data_elems = ecs_vector_new(&bucket_vec_params);
    return result;
}

static
void ecs_map_add_elem(
    EcsMap *map,
    EcsMapBucket *bucket,
    uint64_t key_hash,
    const void *data,
    EcsMapNode *elem)
{
    if (!elem) {
        elem = ecs_vector_add(map->data_elems, &bucket_vec_params);
    }

    elem->key_hash = key_hash;
    elem->data = data;
    elem->next = NULL;

    if (!bucket->nodes) {
        bucket->nodes = elem;
    } else {
        elem->next = bucket->nodes;
        bucket->nodes = elem;
    }

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
EcsMapNode *ecs_map_get_node(
    EcsMapBucket *bucket,
    uint64_t key_hash,
    EcsMapNode **prev_node_out)
{
    EcsMapNode *prev = NULL, *node = bucket->nodes;

    while (node) {
        if (node->key_hash == key_hash) {
            if (prev_node_out) {
                *prev_node_out = prev;
            }
            return node;
        }
        prev = node;
        node = node->next;
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
        if (map->buckets[i].nodes) {
            break;
        }
    }

    return i;
}

static
bool ecs_map_hasnext(
    EcsIter *iter)
{
    EcsMap *map = iter->data;

    if (!map->buckets) {
        return false;
    }

    EcsMapIter *iter_data = iter->ctx;
    uint32_t bucket_index = iter_data->bucket_index;
    EcsMapNode *node = iter_data->node;

    if (node) {
        node = node->next;
    }

    if (!node) {
        bucket_index = ecs_map_next_bucket(map, bucket_index + 1);
        node = map->buckets[bucket_index].nodes;
    }

    if (node) {
        iter_data->node = node;
        iter_data->bucket_index = bucket_index;
        return true;
    } else {
        return false;
    }
}

static
void *ecs_map_next(
    EcsIter *iter)
{
    EcsMapIter *iter_data = iter->ctx;
    EcsMapNode *node = iter_data->node;
    return (void*)node->data;
}

static
void ecs_map_resize(
    EcsMap *map,
    uint32_t bucket_count)
{
    EcsMapBucket *old_buckets = map->buckets;
    uint32_t old_bucket_count = map->bucket_count;

    ecs_map_alloc_buffer(map, bucket_count);
    map->count = 0;

    uint32_t bucket_index;
    for (bucket_index = 0; bucket_index < old_bucket_count; bucket_index ++) {
        EcsMapBucket *bucket = &old_buckets[bucket_index];
        if (bucket->nodes) {
            EcsMapNode *node = bucket->nodes;

            EcsMapNode *next;
            do {
                next = node->next;
                EcsMapBucket *new_bucket =
                    ecs_map_get_bucket(map, node->key_hash);
                ecs_map_add_elem(map, new_bucket, node->key_hash, node->data, node);
            } while ((node = next));
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
        map->buckets[i].nodes = NULL;
    }
    ecs_vector_free(map->data_elems);
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
    uint32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        ecs_map_alloc_buffer(map, 2);
        bucket_count = map->bucket_count;
    }

    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);

    if (!bucket->nodes) {
        ecs_map_add_elem(map, bucket, key_hash, data, NULL);
    } else {
        EcsMapNode *elem = ecs_map_get_node(bucket, key_hash, NULL);
        if (elem) {
            elem->data = data;
        } else {
            ecs_map_add_elem(map, bucket, key_hash, data, NULL);
        }
    }

    if ((float)map->count / (float)bucket_count > REFLECS_LOAD_FACTOR) {
        ecs_map_resize(map, bucket_count * 2);
    }
}

EcsResult ecs_map_remove(
    EcsMap *map,
    uint64_t key_hash)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (bucket->nodes) {
        EcsMapNode *prev_node = NULL, *node = ecs_map_get_node(bucket, key_hash, &prev_node);
        if (node) {
            if (prev_node) {
                prev_node->next = node->next;
            } else {
                bucket->nodes = node->next;
            }

            ecs_vector_remove(map->data_elems, &bucket_vec_params, node);
            map->count --;
            return EcsOk;
        }
    }
    return EcsError;
}

void* ecs_map_get(
    EcsMap *map,
    uint64_t key_hash)
{
    EcsMapBucket *bucket = ecs_map_get_bucket(map, key_hash);
    if (bucket->nodes) {
        EcsMapNode *elem = ecs_map_get_node(bucket, key_hash, NULL);
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

    iter_data->bucket_index = 0;
    iter_data->node = NULL;

    return result;
}
