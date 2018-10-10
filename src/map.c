
#include "include/private/types.h"

#define REFLECS_LOAD_FACTOR (3.0f / 4.0f)

typedef struct EcsMapNode {
    uint64_t key;           /* Key */
    uint64_t data;          /* Value */
    uint32_t next;          /* Next node index */
    uint32_t prev;          /* Previous node index (enables O(1) removal) */
} EcsMapNode;

struct EcsMap {
    uint32_t *buckets;      /* Array of buckets */
    EcsArray *nodes;        /* Array with memory for map nodes */
    size_t bucket_count;    /* number of buckets */
    uint32_t count;         /* number of elements */
};

static
void move_node(
    EcsArray *array,
    const EcsArrayParams *params,
    void *to,
    void *from,
    void *ctx);

const EcsArrayParams node_arr_params = {
    .element_size = sizeof(EcsMapNode),
    .move_action = move_node
};

/** Get map node from index */
static
EcsMapNode *node_from_index(
    EcsArray *nodes,
    uint32_t index)
{
    return ecs_array_get(nodes, &node_arr_params, index - 1);
}

/** Get a bucket for a given key */
static
uint32_t* get_bucket(
    EcsMap *map,
    uint64_t key)
{
    uint64_t index = key % map->bucket_count;
    return &map->buckets[index];
}

/** Callback that updates administration when node is moved in nodes array */
static
void move_node(
    EcsArray *array,
    const EcsArrayParams *params,
    void *to,
    void *from,
    void *ctx)
{
    EcsMapNode *node_p = to;
    uint32_t node = ecs_array_get_index(array, &node_arr_params, to);

    EcsMapNode *prev_p = ecs_array_get(array, &node_arr_params, node_p->prev - 1);
    if (prev_p) {
        prev_p->next = node + 1;
    } else {
        EcsMap *map = ctx;
        uint32_t *bucket = get_bucket(map, node_p->key);
        *bucket = node + 1;
    }
}

/** Allocate the buckets buffer */
static
void alloc_buffer(
    EcsMap *map,
    uint32_t bucket_count)
{
    if (bucket_count) {
        map->buckets = calloc(bucket_count * sizeof(uint32_t), 1);
    } else {
        map->buckets = NULL;
    }

    map->bucket_count = bucket_count;
}

/** Allocate a map object */
static
EcsMap *alloc_map(
    uint32_t bucket_count)
{
    EcsMap *result = malloc(sizeof(EcsMap));
    alloc_buffer(result, bucket_count);
    result->count = 0;
    result->nodes = ecs_array_new(&node_arr_params, ECS_MAP_INITIAL_NODE_COUNT);
    return result;
}

/** Find next non-empty bucket */
static
uint32_t next_bucket(
    EcsMap *map,
    uint32_t start_index)
{
    int i;
    for (i = start_index; i < map->bucket_count; i ++) {
        if (map->buckets[i]) {
            break;
        }
    }

    return i;
}

/** Add new node to bucket */
static
void add_node(
    EcsMap *map,
    uint32_t *bucket,
    uint64_t key,
    uint64_t data,
    EcsMapNode *elem_p)
{
    uint32_t elem;

    if (!elem_p) {
        elem_p = ecs_array_add(&map->nodes, &node_arr_params);
        elem = ecs_array_count(map->nodes);
    } else {
        elem = ecs_array_get_index(
            map->nodes,
            &node_arr_params,
            elem_p) + 1;
    }

    elem_p->key = key;
    elem_p->data = data;
    elem_p->next = 0;
    elem_p->prev = 0;

    if (!*bucket) {
        *bucket = elem;
    } else {
        uint32_t first = *bucket;
        EcsMapNode *first_p = node_from_index(map->nodes, first);
        first_p->prev = elem;
        elem_p->next = *bucket;
        *bucket = elem;
    }

    map->count ++;
}

/** Get map node for a given key */
static
EcsMapNode *get_node(
    EcsMap *map,
    uint32_t *bucket,
    uint64_t key)
{
    uint32_t node = *bucket;

    while (node) {
        EcsMapNode *node_p = node_from_index(map->nodes, node);
        if (node_p->key == key) {
            return node_p;
        }
        node = node_p->next;
    }

    return NULL;
}

/** Iterator hasnext callback */
static
bool hasnext(
    EcsIter *iter)
{
    EcsMap *map = iter->data;

    if (!map->buckets) {
        return false;
    }

    EcsMapIter *iter_data = iter->ctx;
    uint32_t bucket_index = iter_data->bucket_index;
    uint32_t node = iter_data->node;

    if (node) {
        EcsMapNode *node_p = node_from_index(map->nodes, node);
        node = node_p->next;
    }

    if (!node) {
        bucket_index = next_bucket(map, bucket_index + 1);
        if (bucket_index < map->bucket_count) {
            node = map->buckets[bucket_index];
        } else {
            node = 0;
        }
    }

    if (node) {
        iter_data->node = node;
        iter_data->bucket_index = bucket_index;
        return true;
    } else {
        return false;
    }
}

/** Iterator next callback */
static
void *next(
    EcsIter *iter)
{
    EcsMap *map = iter->data;
    EcsMapIter *iter_data = iter->ctx;
    EcsMapNode *node_p = node_from_index(map->nodes, iter_data->node);
    if (!node_p) {
        abort();
    }
    return (void*)node_p->data;
}

/** Resize number of buckets in a map */
static
void resize_map(
    EcsMap *map,
    uint32_t bucket_count)
{
    uint32_t *old_buckets = map->buckets;
    uint32_t old_bucket_count = map->bucket_count;

    alloc_buffer(map, bucket_count);
    map->count = 0;

    uint32_t bucket_index;
    for (bucket_index = 0; bucket_index < old_bucket_count; bucket_index ++) {
        uint32_t bucket = old_buckets[bucket_index];
        if (bucket) {
            uint32_t node = bucket;
            uint32_t next;
            uint64_t key;

            EcsMapNode *node_p;
            do {
                node_p = node_from_index(map->nodes, node);
                next = node_p->next;
                key = node_p->key;
                uint32_t *new_bucket = get_bucket(map, key);
                add_node(map, new_bucket, key, node_p->data, node_p);
            } while ((node = next));
        }
    }

    free(old_buckets);
}


/* -- Public functions -- */

EcsMap* ecs_map_new(
    uint32_t size)
{
    return alloc_map(size);
}

void ecs_map_clear(
    EcsMap *map)
{
    int i;
    for (i = 0; i < map->bucket_count; i ++) {
        map->buckets[i] = 0;
    }
    ecs_array_free(map->nodes);
}

void ecs_map_free(
    EcsMap *map)
{
    ecs_array_free(map->nodes);
    free(map);
}

void ecs_map_set64(
    EcsMap *map,
    uint64_t key,
    uint64_t data)
{
    uint32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        alloc_buffer(map, 2);
        bucket_count = map->bucket_count;
    }

    uint32_t *bucket = get_bucket(map, key);

    if (!*bucket) {
        add_node(map, bucket, key, data, NULL);
    } else {
        EcsMapNode *elem = get_node(map, bucket, key);
        if (elem) {
            elem->data = data;
        } else {
            add_node(map, bucket, key, data, NULL);
        }
    }

    if ((float)map->count / (float)bucket_count > REFLECS_LOAD_FACTOR) {
        resize_map(map, bucket_count * 2);
    }
}

EcsResult ecs_map_remove(
    EcsMap *map,
    uint64_t key)
{
    if (!map->count) {
        return EcsError;
    }

    uint32_t *bucket = get_bucket(map, key);

    if (*bucket) {
        EcsMapNode *node = get_node(map, bucket, key);
        if (node) {
            EcsMapNode *prev_node = node_from_index(map->nodes, node->prev);
            if (prev_node) {
                prev_node->next = node->next;
            } else {
                *bucket = node->next;
            }

            EcsArrayParams params = node_arr_params;
            params.move_ctx = map;

            ecs_array_remove(map->nodes, &params, node);
            map->count --;
            return EcsOk;
        }
    }

    return EcsError;
}

uint64_t ecs_map_get64(
    EcsMap *map,
    uint64_t key)
{
    if (!map->count) {
        return 0;
    }

    uint32_t *bucket = get_bucket(map, key);
    if (*bucket) {
        EcsMapNode *elem = get_node(map, bucket, key);
        if (elem) {
            return elem->data;
        }
    }

    return 0;
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

uint32_t ecs_map_set_size(
    EcsMap *map,
    uint32_t size)
{
    uint32_t result = ecs_array_set_size(&map->nodes, &node_arr_params, size);
    resize_map(map, size / REFLECS_LOAD_FACTOR);
    return result;
}

EcsIter _ecs_map_iter(
    EcsMap *map,
    EcsMapIter *iter_data)
{
    EcsIter result = {
        .data = map,
        .ctx = iter_data,
        .hasnext = hasnext,
        .next = next,
        .release = NULL
    };

    iter_data->bucket_index = -1;
    iter_data->node = 0;

    return result;
}
