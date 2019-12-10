#include "types.h"

typedef struct chunk_t {
    void *data;
    uint32_t count;
} chunk_t;

typedef struct sparse_elem_t {
    uint32_t dense;
    void *ptr;
} sparse_elem_t;

struct ecs_chunked_t {
    ecs_vector_t *chunks;
    ecs_vector_t *dense;
    ecs_vector_t *sparse;
    ecs_vector_t *free_stack;
    uint32_t element_size;
    uint32_t chunk_size;
};

static ecs_vector_params_t free_param = {.element_size = sizeof(uint32_t)};
static ecs_vector_params_t dense_param = {.element_size = sizeof(uint32_t)};
static ecs_vector_params_t sparse_param = {.element_size = sizeof(sparse_elem_t)};
static ecs_vector_params_t chunk_param = {.element_size = sizeof(chunk_t)};

static
void add_chunk(
    ecs_chunked_t *chunked)
{
    /* Add chunk to chunked instance */
    chunk_t *chunk = ecs_vector_add(&chunked->chunks, &chunk_param);
    uint32_t chunk_size = chunked->chunk_size;
    uint32_t element_size = chunked->element_size;

    /* Allocate data vector for chunk */
    chunk->data = ecs_os_malloc(element_size * chunk_size);
    chunk->count = 0;

    /* Create room in sparse array for chunk */
    uint32_t chunk_count = ecs_vector_count(chunked->chunks);
    ecs_vector_set_count(&chunked->sparse, &sparse_param, 
        chunk_count * chunk_size);

    ecs_assert(chunked->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Prepare cached pointers to chunk elements in sparse array */
    sparse_elem_t *sparse_array = ecs_vector_first(chunked->sparse);
    ecs_assert(sparse_array != NULL, ECS_INTERNAL_ERROR, NULL);

    sparse_array = &sparse_array[(chunk_count - 1) * chunk_size];

    uint32_t i;
    for (i = 0; i < chunk_size; i ++) {
        sparse_array[i].ptr = ECS_OFFSET(chunk->data, i * element_size);
    }
}

static
chunk_t* last_chunk(
    const ecs_chunked_t *chunked)
{
    return ecs_vector_last(chunked->chunks, &chunk_param);
}

static
void* add_sparse(
    ecs_chunked_t *chunked,
    uint32_t index)
{
    sparse_elem_t *sparse = ecs_vector_first(chunked->sparse);
    sparse[index].dense = ecs_vector_count(chunked->dense);

    uint32_t *dense = ecs_vector_add(&chunked->dense, &dense_param);
    *dense = index;

    return sparse[index].ptr;
}

static
void* get_sparse(
    const ecs_chunked_t *chunked,
    uint32_t index,
    bool remove)
{
    sparse_elem_t *sparse = ecs_vector_first(chunked->sparse);
    uint32_t dense = sparse[index].dense;

    uint32_t *dense_array = ecs_vector_first(chunked->dense);
    uint32_t dense_count = ecs_vector_count(chunked->dense);

    ecs_assert(dense < dense_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense_array[dense] == index, ECS_INVALID_PARAMETER, NULL);

    if (remove) {
        int32_t last_sparse_index = dense_array[dense_count - 1];
        dense_array[dense] = last_sparse_index;
        sparse[last_sparse_index].dense = dense;
        ecs_vector_remove_last(chunked->dense);
    }

    return sparse[index].ptr;  
}

ecs_chunked_t* _ecs_chunked_new(
    uint32_t element_size,
    uint32_t chunk_size,
    uint32_t chunk_count)
{
    ecs_chunked_t *result = ecs_os_calloc(1, sizeof(ecs_chunked_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->element_size = element_size;
    result->chunk_size = chunk_size;

    uint32_t i;
    for (i = 0; i < chunk_count; i ++) {
        add_chunk(result);
    }

    return result;
}

void ecs_chunked_free(
    ecs_chunked_t *chunked)
{
    int i, count = ecs_vector_count(chunked->chunks);
    chunk_t *array = ecs_vector_first(chunked->chunks);

    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i].data);
    }

    ecs_vector_free(chunked->chunks);
    ecs_vector_free(chunked->free_stack);
    ecs_vector_free(chunked->sparse);
    ecs_vector_free(chunked->dense);

    ecs_os_free(chunked);
}

FLECS_EXPORT
void ecs_chunked_clear(
    ecs_chunked_t *chunked)
{
    uint32_t i, count = ecs_vector_count(chunked->dense);
    uint32_t *dense = ecs_vector_first(chunked->dense);

    for (i = 0; i < count; i ++) {
        _ecs_chunked_remove(chunked, 0, dense[i]);
    }

    ecs_vector_clear(chunked->dense);
}

void* _ecs_chunked_add(
    ecs_chunked_t *chunked,
    uint32_t element_size)
{
    (void)element_size;
    ecs_assert(
        element_size == chunked->element_size, ECS_INVALID_PARAMETER, NULL);

    uint32_t index = 0;
    
    if (!ecs_vector_pop(chunked->free_stack, &free_param, &index)) {
        chunk_t *chunk = last_chunk(chunked);
        if (!chunk) {
            add_chunk(chunked);
            chunk = last_chunk(chunked);
        }

        ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);

        if (chunk->count == chunked->chunk_size) {
            add_chunk(chunked);
            chunk = last_chunk(chunked);
        }

        ecs_assert(chunk->count < chunked->chunk_size, ECS_INTERNAL_ERROR, NULL);

        uint32_t chunk_size = chunked->chunk_size;
        uint32_t chunk_count = ecs_vector_count(chunked->chunks);
        uint32_t chunk_elem_count = chunk->count;

        index = chunk_elem_count + ((chunk_count - 1) * chunk_size);
        chunk->count ++;
    }

    return add_sparse(chunked, index);;
}

void* _ecs_chunked_remove(
    ecs_chunked_t *chunked,
    uint32_t element_size,
    uint32_t index)
{   
    (void)element_size;
    ecs_assert(!element_size || element_size == chunked->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    uint32_t *free_elem = ecs_vector_add(&chunked->free_stack, &free_param);
    *free_elem = index;

    return get_sparse(chunked, index, true);
}

void* _ecs_chunked_get(
    const ecs_chunked_t *chunked,
    uint32_t element_size,
    uint32_t index)
{
    (void)element_size;
    ecs_assert(index < ecs_vector_count(chunked->dense), 
        ECS_INVALID_PARAMETER, NULL);
    
    ecs_assert(element_size == chunked->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    const uint32_t *it = ecs_vector_first(chunked->dense);

    return get_sparse(chunked, it[index], false);
}

void* _ecs_chunked_get_sparse(
    const ecs_chunked_t *chunked,
    uint32_t element_size,
    uint32_t index)
{
    (void)element_size;
    ecs_assert(element_size == chunked->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_sparse(chunked, index, false);
}

uint32_t ecs_chunked_count(
    const ecs_chunked_t *chunked)
{
    return ecs_vector_count(chunked->dense);
}

const uint32_t* ecs_chunked_indices(
    const ecs_chunked_t *chunked)
{
    return ecs_vector_first(chunked->dense);
}

ecs_chunked_t* ecs_chunked_copy(
    const ecs_chunked_t *src)
{
    ecs_chunked_t *dst = ecs_os_memdup(src, sizeof(ecs_chunked_t));
    dst->chunks = ecs_vector_copy(src->chunks, &chunk_param);
    dst->dense = ecs_vector_copy(src->dense, &dense_param);
    dst->sparse = ecs_vector_copy(src->sparse, &sparse_param);
    dst->free_stack = ecs_vector_copy(src->free_stack, &free_param);

    /* Iterate chunks, copy data */
    sparse_elem_t *sparse_array = ecs_vector_first(dst->sparse);
    chunk_t *chunks = ecs_vector_first(dst->chunks);
    uint32_t i, count = ecs_vector_count(dst->chunks);

    for (i = 0; i < count; i ++) {
        chunks[i].data = ecs_os_memdup(
            chunks[i].data, dst->chunk_size * dst->element_size);
        
        uint32_t j;
        for (j = 0; j < dst->chunk_size; j ++) {
            sparse_array[i * dst->chunk_size + j].ptr = 
                ECS_OFFSET(chunks[i].data, j * dst->element_size);
        }
    }

    return dst;
}

void ecs_chunked_memory(
    ecs_chunked_t *chunked,
    uint32_t *allocd,
    uint32_t *used)
{
    if (!chunked) {
        return;
    }

    ecs_vector_memory(chunked->chunks, &chunk_param, allocd, used);
    ecs_vector_memory(chunked->dense, &dense_param, allocd, used);
    ecs_vector_memory(chunked->sparse, &sparse_param, allocd, used);
    ecs_vector_memory(chunked->free_stack, &free_param, allocd, used);

    uint32_t data_total = chunked->chunk_size * 
        chunked->element_size * ecs_vector_count(chunked->chunks);

    uint32_t data_not_used = ecs_vector_count(chunked->free_stack) * 
        chunked->element_size;

    if (allocd) {
        *allocd += data_total;
    }

    if (used) {
        *used += data_total - data_not_used;
    }
}
