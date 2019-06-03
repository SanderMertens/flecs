
#include "flecs_private.h"

#define FLECS_LOAD_FACTOR (3.0f / 4.0f)

typedef struct ecs_map_node_t {
    uint64_t key;           /* Key */
    uint64_t data;          /* Value */
    uint32_t next;          /* Next node index */
    uint32_t prev;          /* Previous node index (enables O(1) removal) */
} ecs_map_node_t;

struct ecs_map_t {
    uint32_t *buckets;      /* Array of buckets */
    ecs_vector_t *nodes;        /* Array with memory for map nodes */
    size_t bucket_count;    /* number of buckets */
    uint32_t count;         /* number of elements */
    uint32_t min;           /* minimum number of elements */
};

static
void move_node(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *to,
    void *from,
    void *ctx);

const ecs_vector_params_t node_arr_params = {
    .element_size = sizeof(ecs_map_node_t),
    .move_action = move_node
};

/** Get map node from index */
static
ecs_map_node_t *node_from_index(
    ecs_vector_t *nodes,
    uint32_t index)
{
    return ecs_vector_get(nodes, &node_arr_params, index - 1);
}

/** Get a bucket for a given key */
static
uint32_t* get_bucket(
    ecs_map_t *map,
    uint64_t key)
{
    uint64_t index = key % map->bucket_count;
    return &map->buckets[index];
}

/** Callback that updates administration when node is moved in nodes array */
static
void move_node(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *to,
    void *from,
    void *ctx)
{
    ecs_map_node_t *node_p = to;
    uint32_t node = ecs_vector_get_index(array, &node_arr_params, to) + 1;
    uint32_t prev = node_p->prev;
    uint32_t next = node_p->next;
    (void)params;
    (void)from;

    if (prev) {
        ecs_map_node_t *prev_p = node_from_index(array, prev);
        prev_p->next = node;
    } else {
        ecs_map_t *map = ctx;
        uint32_t *bucket = get_bucket(map, node_p->key);
        *bucket = node;
    }

    if (next) {
        ecs_map_node_t *next_p = node_from_index(array, next);
        next_p->prev = node;
    }
}

/** Allocate the buckets buffer */
static
void alloc_buffer(
    ecs_map_t *map,
    uint32_t bucket_count)
{
    if (bucket_count) {
        map->buckets = ecs_os_calloc(bucket_count * sizeof(uint32_t), 1);
        ecs_assert(map->buckets != NULL, ECS_OUT_OF_MEMORY, 0);
    } else {
        map->buckets = NULL;
    }

    map->bucket_count = bucket_count;
}

/** Allocate a map object */
static
ecs_map_t *alloc_map(
    uint32_t bucket_count)
{
    ecs_map_t *result = ecs_os_malloc(sizeof(ecs_map_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    alloc_buffer(result, bucket_count);
    result->count = 0;
    result->min = bucket_count;
    result->nodes = ecs_vector_new(&node_arr_params, ECS_MAP_INITIAL_NODE_COUNT);
    return result;
}

/** Find next non-empty bucket */
static
uint32_t next_bucket(
    ecs_map_t *map,
    uint32_t start_index)
{
    size_t i;
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
    ecs_map_t *map,
    uint32_t *bucket,
    uint64_t key,
    uint64_t data,
    ecs_map_node_t *elem_p)
{
    uint32_t elem;

    if (!elem_p) {
        elem_p = ecs_vector_add(&map->nodes, &node_arr_params);
        elem = ecs_vector_count(map->nodes);
    } else {
        elem = ecs_vector_get_index(
            map->nodes,
            &node_arr_params,
            elem_p) + 1;
    }

    elem_p->key = key;
    elem_p->data = data;
    elem_p->next = 0;
    elem_p->prev = 0;

    uint32_t first = *bucket;
    if (first) {
        ecs_map_node_t *first_p = node_from_index(map->nodes, first);
        first_p->prev = elem;
        elem_p->next = first;
    }

    *bucket = elem;

    map->count ++;
}

/** Get map node for a given key */
static
ecs_map_node_t *get_node(
    ecs_map_t *map,
    uint32_t *bucket,
    uint64_t key)
{
    uint32_t node = *bucket;

    while (node) {
        ecs_map_node_t *node_p = node_from_index(map->nodes, node);

        if (node_p->prev) {
            assert(node_from_index(map->nodes, node_p->prev)->next == node);
        }
        if (node_p->next) {
            assert(node_from_index(map->nodes, node_p->next)->prev == node);
        }

        if (node_p->key == key) {
            return node_p;
        }
        node = node_p->next;
    }

    return NULL;
}

/** Resize number of buckets in a map */
static
void resize_map(
    ecs_map_t *map,
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

            ecs_map_node_t *node_p;
            do {
                node_p = node_from_index(map->nodes, node);
                next = node_p->next;
                key = node_p->key;
                uint32_t *new_bucket = get_bucket(map, key);
                add_node(map, new_bucket, key, node_p->data, node_p);
            } while ((node = next));
        }
    }

    ecs_os_free(old_buckets);
}


/* -- Public functions -- */

ecs_map_t* ecs_map_new(
    uint32_t size)
{
    return alloc_map((float)size / FLECS_LOAD_FACTOR);
}

void ecs_map_clear(
    ecs_map_t *map)
{
    uint32_t target_size = (float)map->count / FLECS_LOAD_FACTOR;

    if (target_size < map->min) {
        target_size = map->min;
    }

    if (target_size < (float)map->bucket_count * 0.75) {
        ecs_os_free(map->buckets);
        alloc_buffer(map, target_size);
    } else {
        memset(map->buckets, 0, sizeof(int32_t) * map->bucket_count);
    }

    ecs_vector_reclaim(&map->nodes, &node_arr_params);
    ecs_vector_clear(map->nodes);

    map->count = 0;
}

void ecs_map_free(
    ecs_map_t *map)
{
    ecs_vector_free(map->nodes);
    ecs_os_free(map->buckets);
    ecs_os_free(map);
}

void ecs_map_set64(
    ecs_map_t *map,
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
        ecs_map_node_t *elem = get_node(map, bucket, key);
        if (elem) {
            elem->data = data;
        } else {
            add_node(map, bucket, key, data, NULL);
        }
    }

    if ((float)map->count / (float)bucket_count > FLECS_LOAD_FACTOR) {
        resize_map(map, bucket_count * 2);
    }
}

int ecs_map_remove(
    ecs_map_t *map,
    uint64_t key)
{
    if (!map->count) {
        return -1;
    }

    uint32_t *bucket = get_bucket(map, key);

    if (*bucket) {
        ecs_map_node_t *node = get_node(map, bucket, key);
        if (node) {
            ecs_map_node_t *prev_node = node_from_index(map->nodes, node->prev);
            ecs_map_node_t *next_node = node_from_index(map->nodes, node->next);

            if (prev_node) {
                assert(node_from_index(map->nodes, prev_node->next) == node);
                prev_node->next = node->next;
            } else {
                *bucket = node->next;
            }

            if (next_node) {
                assert(node_from_index(map->nodes, next_node->prev) == node);
                next_node->prev = node->prev;
            }

            ecs_vector_params_t params = node_arr_params;
            params.move_ctx = map;

            ecs_vector_remove(map->nodes, &params, node);
            map->count --;

            return 0;
        }
    }

    return -1;
}

uint64_t ecs_map_get64(
    ecs_map_t *map,
    uint64_t key)
{
    if (!map->count) {
        return 0;
    }

    uint32_t *bucket = get_bucket(map, key);
    if (*bucket) {
        ecs_map_node_t *elem = get_node(map, bucket, key);
        if (elem) {
            return elem->data;
        }
    }

    return 0;
}

uint64_t* ecs_map_get_ptr(
    ecs_map_t *map,
    uint64_t key)
{
    if (!map->count) {
        return 0;
    }

    uint32_t *bucket = get_bucket(map, key);
    if (*bucket) {
        ecs_map_node_t *elem = get_node(map, bucket, key);
        if (elem) {
            return &elem->data;
        }
    }

    return 0;
}

bool ecs_map_has(
    ecs_map_t *map,
    uint64_t key_hash,
    uint64_t *value_out)
{
    if (!map->count) {
        return false;
    }

    uint32_t *bucket = get_bucket(map, key_hash);
    if (*bucket) {
        ecs_map_node_t *elem = get_node(map, bucket, key_hash);
        if (elem) {
            if (value_out) {
                *value_out = elem->data;
            }
            return true;
        }
    }

    return false;
}

uint32_t ecs_map_count(
    ecs_map_t *map)
{
    return map->count;
}

uint32_t ecs_map_bucket_count(
    ecs_map_t *map)
{
    return map->bucket_count;
}

uint32_t ecs_map_set_size(
    ecs_map_t *map,
    uint32_t size)
{
    uint32_t result = ecs_vector_set_size(&map->nodes, &node_arr_params, size);
    resize_map(map, size / FLECS_LOAD_FACTOR);
    return result;
}

void ecs_map_memory(
    ecs_map_t *map,
    uint32_t *total,
    uint32_t *used)
{
    if (!map) {
        return;
    }

    if (total) {

        *total += map->bucket_count * sizeof(uint32_t) + sizeof(ecs_map_t);
        ecs_vector_memory(map->nodes, &node_arr_params, total, NULL);
    }

    if (used) {
        *used += map->count * sizeof(uint32_t);
        ecs_vector_memory(map->nodes, &node_arr_params, NULL, used);
    }
}

ecs_map_iter_t ecs_map_iter(
    ecs_map_t *map)
{
    ecs_map_iter_t result = {
        .map = map,
        .bucket_index = -1,
        .node = 0
    };

    return result;
}

bool ecs_map_hasnext(
    ecs_map_iter_t *iter_data)
{
    ecs_map_t *map = iter_data->map;
    if (!map->count) {
        return false;
    }

    if (!map->buckets) {
        return false;
    }

    uint32_t bucket_index = iter_data->bucket_index;
    uint32_t node = iter_data->node;

    if (node) {
        ecs_map_node_t *node_p = node_from_index(map->nodes, node);
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

uint64_t ecs_map_next_w_key(
    ecs_map_iter_t *iter_data,
    uint64_t *key_out)
{
    ecs_map_t *map = iter_data->map;
    ecs_map_node_t *node_p = node_from_index(map->nodes, iter_data->node);
    assert(node_p != NULL);
    if (key_out) *key_out = node_p->key;
    return node_p->data;
}

uint64_t ecs_map_next(
    ecs_map_iter_t *iter_data)
{
    return ecs_map_next_w_key(iter_data, NULL);
}

void* ecs_map_next_ptr(
    ecs_map_iter_t *iter_data)
{
    return (void*)(uintptr_t)ecs_map_next_w_key(iter_data, NULL);
}

void* ecs_map_next_ptr_w_key(
    ecs_map_iter_t *iter_data,
    uint64_t *key_out)
{
    return (void*)(uintptr_t)ecs_map_next_w_key(iter_data, key_out);
}
