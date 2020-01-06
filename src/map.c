
#include "flecs_private.h"

#define FLECS_LOAD_FACTOR (3.0f / 4.0f)

typedef struct ecs_map_node_t {
    uint64_t key;           /* Key */
    uint32_t next;          /* Next node index */
    uint32_t prev;          /* Previous node index (enables O(1) removal) */
} ecs_map_node_t;

struct ecs_map_t {
    uint32_t *buckets;      /* Array of buckets */
    ecs_vector_t *nodes;    /* Array with memory for map nodes */
    ecs_vector_params_t node_params; /* Parameters for node vector */
    size_t bucket_count;    /* number of buckets */
    uint32_t count;         /* number of elements */
    uint32_t min;           /* minimum number of elements */
};

static
size_t data_size(
    ecs_map_t *map)
{
    return map->node_params.element_size - sizeof(ecs_map_node_t);
}

/** Get map node from index */
static
ecs_map_node_t *node_from_index(
    ecs_map_t *map,
    ecs_vector_t *nodes,
    uint32_t index)
{
    return ecs_vector_get(nodes, &map->node_params, index - 1);
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
    ecs_map_t *map = ctx;
    ecs_map_node_t *node_p = to;
    uint32_t node = ecs_vector_get_index(array, &map->node_params, to) + 1;
    uint32_t prev = node_p->prev;
    uint32_t next = node_p->next;
    (void)params;
    (void)from;

    if (prev) {
        ecs_map_node_t *prev_p = node_from_index(map, array, prev);
        prev_p->next = node;
    } else {
        uint32_t *bucket = get_bucket(map, node_p->key);
        *bucket = node;
    }

    if (next) {
        ecs_map_node_t *next_p = node_from_index(map, array, next);
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
    uint32_t bucket_count,
    uint32_t data_size)
{
    ecs_map_t *result = ecs_os_malloc(sizeof(ecs_map_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    alloc_buffer(result, bucket_count);
    result->count = 0;
    result->min = bucket_count;
    result->node_params = (ecs_vector_params_t){
        .element_size = data_size + sizeof(ecs_map_node_t),
        .move_action = move_node
    };

    result->nodes = ecs_vector_new(&result->node_params, ECS_MAP_INITIAL_NODE_COUNT);
    
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

static
void *get_node_data(
    ecs_map_node_t *node)
{
    return ECS_OFFSET(node, sizeof(ecs_map_node_t));
}

static
void set_node_data(
    ecs_map_t *map,
    ecs_map_node_t *node,
    const void *data)
{
    void *node_data = get_node_data(node);
    if (data != node_data) { 
        if (data) {
            memcpy(node_data, data, data_size(map));
        } else {
            memset(node_data, 0, data_size(map));
        }
    }
}

/** Add new node to bucket */
static
void* add_node(
    ecs_map_t *map,
    uint32_t *bucket,
    uint64_t key,
    const void *data,
    ecs_map_node_t *elem_p)
{
    uint32_t elem;

    if (!elem_p) {
        elem_p = ecs_vector_add(&map->nodes, &map->node_params);
        elem = ecs_vector_count(map->nodes);
    } else {
        elem = ecs_vector_get_index(
            map->nodes,
            &map->node_params,
            elem_p) + 1;
    }

    elem_p->key = key;
    elem_p->next = 0;
    elem_p->prev = 0;
    set_node_data(map, elem_p, data);

    uint32_t first = *bucket;
    if (first) {
        ecs_map_node_t *first_p = node_from_index(map, map->nodes, first);
        first_p->prev = elem;
        elem_p->next = first;
    }

    *bucket = elem;

    map->count ++;

    return elem_p;
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
        ecs_map_node_t *node_p = node_from_index(map, map->nodes, node);

        if (node_p->prev) {
            assert(node_from_index(map, map->nodes, node_p->prev)->next == node);
        }
        if (node_p->next) {
            assert(node_from_index(map, map->nodes, node_p->next)->prev == node);
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
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

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
                node_p = node_from_index(map, map->nodes, node);
                next = node_p->next;
                key = node_p->key;
                uint32_t *new_bucket = get_bucket(map, key);
                add_node(map, new_bucket, key, get_node_data(node_p), node_p);
            } while ((node = next));
        }
    }

    ecs_os_free(old_buckets);
}


/* -- Public functions -- */

ecs_map_t* ecs_map_new(
    uint32_t size,
    uint32_t data_size)
{
    if (!data_size) {
        data_size = sizeof(uint64_t);
    }
    return alloc_map((float)size / FLECS_LOAD_FACTOR, data_size);
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

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

    ecs_vector_reclaim(&map->nodes, &map->node_params);
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

void* _ecs_map_set(
    ecs_map_t *map,
    uint64_t key,
    const void *data,
    uint32_t size)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    (void)size;
    ecs_assert(ecs_map_data_size(map) == size, ECS_INVALID_PARAMETER, NULL);

    uint32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        alloc_buffer(map, 2);
        bucket_count = map->bucket_count;
    }

    if ((((float)map->count) / (float)bucket_count) > FLECS_LOAD_FACTOR) {
        resize_map(map, bucket_count * 2);
    }

    uint32_t *bucket = get_bucket(map, key);
    ecs_map_node_t *node = NULL;

    if (!*bucket) {
        node = add_node(map, bucket, key, data, NULL);
    } else {
        node = get_node(map, bucket, key);
        if (node) {
            set_node_data(map, node, data);
        } else {
            node = add_node(map, bucket, key, data, NULL);
        }
    }

    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);

    return get_node_data(node);
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
            ecs_map_node_t *prev_node = node_from_index(map, map->nodes, node->prev);
            ecs_map_node_t *next_node = node_from_index(map, map->nodes, node->next);

            if (prev_node) {
                assert(node_from_index(map, map->nodes, prev_node->next) == node);
                prev_node->next = node->next;
            } else {
                *bucket = node->next;
            }

            if (next_node) {
                assert(node_from_index(map, map->nodes, next_node->prev) == node);
                next_node->prev = node->prev;
            }

            ecs_vector_params_t params = map->node_params;
            params.move_ctx = map;

            ecs_vector_remove(map->nodes, &params, node);
            map->count --;

            return 0;
        }
    }

    return -1;
}

void* ecs_map_get_ptr(
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
            return get_node_data(elem);
        }
    }

    return 0;
}

bool _ecs_map_has(
    ecs_map_t *map,
    uint64_t key_hash,
    void *value_out,
    uint32_t size)
{
    (void)size;

    if (!map) {
        return false;
    }
    
    if (!map->count) {
        return false;
    }

    ecs_assert(!value_out || (ecs_map_data_size(map) == size), ECS_INVALID_PARAMETER, NULL); 

    uint32_t *bucket = get_bucket(map, key_hash);
    if (*bucket) {
        ecs_map_node_t *elem = get_node(map, bucket, key_hash);
        if (elem) {
            if (value_out) {
                memcpy(value_out, get_node_data(elem), data_size(map));
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
    uint32_t result = ecs_vector_set_size(&map->nodes, &map->node_params, size);
    resize_map(map, size / FLECS_LOAD_FACTOR);
    return result;
}

uint32_t ecs_map_grow(
    ecs_map_t *map,
    uint32_t size)
{
    if (size > ecs_vector_size(map->nodes)) {
        return ecs_map_set_size(map, size);
    }

    return 0;
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
        ecs_vector_memory(map->nodes, &map->node_params, total, NULL);
    }

    if (used) {
        *used += map->count * sizeof(uint32_t);
        ecs_vector_memory(map->nodes, &map->node_params, NULL, used);
    }
}

ecs_map_t* ecs_map_copy(
    const ecs_map_t *map)
{
    ecs_map_t *dst = ecs_os_memdup(map, sizeof(ecs_map_t));
    dst->buckets = ecs_os_memdup(
        map->buckets, map->bucket_count * sizeof(uint32_t));
    
    dst->nodes = ecs_vector_copy(map->nodes, &map->node_params);

    return dst;
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
        ecs_map_node_t *node_p = node_from_index(map, map->nodes, node);
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

void* ecs_map_next_w_key_w_size(
    ecs_map_iter_t *iter_data,
    uint64_t *key_out,
    size_t size)
{
    (void)size;

    ecs_map_t *map = iter_data->map;
    ecs_assert(!size || data_size(map) == size, ECS_INTERNAL_ERROR, NULL);

    ecs_map_node_t *node_p = node_from_index(map, map->nodes, iter_data->node);
    assert(node_p != NULL);
    if (key_out) *key_out = node_p->key;
    return get_node_data(node_p);
}

void* ecs_map_next_w_key(
    ecs_map_iter_t *iter_data,
    uint64_t *key_out)
{
    return ecs_map_next_w_key_w_size(iter_data, key_out, 0);
}

void* ecs_map_next(
    ecs_map_iter_t *iter_data)
{
    return ecs_map_next_w_key_w_size(iter_data, NULL, 0);
}

void* ecs_map_next_w_size(
    ecs_map_iter_t *iter_data,
    size_t size)
{
    return ecs_map_next_w_key_w_size(iter_data, NULL, size);
}

uint32_t ecs_map_data_size(
    ecs_map_t *map)
{
    return data_size(map);
}
