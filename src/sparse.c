#include "types.h"

#define CHUNK_ALLOC_SIZE (4096)

typedef struct chunk_t {
    void *data;
    uint32_t count;
} chunk_t;

typedef struct sparse_elem_t {
    uint32_t dense;
    void *ptr;
} sparse_elem_t;

struct ecs_sparse_t {
    ecs_vector_t *chunks;       /* Vector with chunk pointers */
    ecs_vector_t *dense;        /* Dense array */
    ecs_vector_t *sparse;       /* Sparse array + element pointers */
    ecs_vector_t *free_stack;   /* Free elements */
    uint32_t element_size;      /* Side of payload */
    uint32_t chunk_size;        /* Chunk size */
};

static
void add_chunk(
    ecs_sparse_t *sparse)
{
    /* Add chunk to sparse instance */
    uint32_t chunk_count = ecs_vector_count(sparse->chunks);
    chunk_t *chunk = ecs_vector_add(&sparse->chunks, chunk_t);
    uint32_t chunk_size = sparse->chunk_size;
    uint32_t element_size = sparse->element_size;

    /* Allocate data vector for chunk */
    chunk->data = ecs_os_malloc(CHUNK_ALLOC_SIZE);
    chunk->count = 0;

    /* Create room in sparse array for chunk */
    uint32_t prev_total = chunk_count * chunk_size;

    ecs_vector_set_count(&sparse->sparse, sparse_elem_t, prev_total + chunk_size);

    ecs_assert(sparse->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Prepare cached pointers to chunk elements in sparse array */
    sparse_elem_t *sparse_array = ecs_vector_first(sparse->sparse);
    ecs_assert(sparse_array != NULL, ECS_INTERNAL_ERROR, NULL);

    sparse_array = &sparse_array[prev_total];

    uint32_t i;
    for (i = 0; i < chunk_size; i ++) {
        sparse_array[i].dense = 0;
        sparse_array[i].ptr = ECS_OFFSET(chunk->data, i * element_size);
    }
}

static
chunk_t* last_chunk(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_last(sparse->chunks, chunk_t);
}

static
void* add_sparse(
    ecs_sparse_t *sparse,
    uint32_t index)
{
    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);
    sparse_arr[index].dense = ecs_vector_count(sparse->dense);
    uint32_t *dense = ecs_vector_add(&sparse->dense, uint32_t);
    *dense = index;

    return sparse_arr[index].ptr;
}

static
void* get_sparse(
    const ecs_sparse_t *sparse,
    uint32_t index,
    bool remove)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        return NULL;
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);
    uint32_t dense = sparse_arr[index].dense;

    uint32_t *dense_array = ecs_vector_first(sparse->dense);
    uint32_t dense_count = ecs_vector_count(sparse->dense);

    if (dense >= dense_count) {
        return NULL;
    }

    if (dense_array[dense] != index) {
        return NULL;
    }

    if (remove) {
        int32_t last_sparse_index = dense_array[dense_count - 1];
        dense_array[dense] = last_sparse_index;
        sparse_arr[last_sparse_index].dense = dense;
        ecs_vector_remove_last(sparse->dense);
    }

    return sparse_arr[index].ptr;  
}

static
void* get_or_set_sparse(
    ecs_sparse_t *sparse,
    uint32_t index,
    bool *is_new)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        ecs_sparse_set_size(sparse, index + 1);
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INTERNAL_ERROR, NULL);
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);
    uint32_t dense = sparse_arr[index].dense;

    uint32_t *dense_array = ecs_vector_first(sparse->dense);
    uint32_t dense_count = ecs_vector_count(sparse->dense);

    if (dense >= dense_count || dense_array[dense] != index) {
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INVALID_PARAMETER, NULL);
        ecs_vector_add(&sparse->dense, uint32_t);

        dense_array = ecs_vector_first(sparse->dense);
        sparse_arr[index].dense = dense_count;
        dense_array[dense_count] = index;

        if (is_new) {
            *is_new = true;
        }
    }

    return sparse_arr[index].ptr;  
}

ecs_sparse_t* _ecs_sparse_new(
    uint32_t element_size,
    uint32_t element_count)
{
    ecs_sparse_t *result = ecs_os_calloc(1, sizeof(ecs_sparse_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->chunk_size = CHUNK_ALLOC_SIZE / element_size;
    result->element_size = element_size;

    while (ecs_sparse_size(result) < element_count) {
        add_chunk(result);
    }

    return result;
}

void ecs_sparse_free(
    ecs_sparse_t *sparse)
{
    int i, count = ecs_vector_count(sparse->chunks);
    chunk_t *array = ecs_vector_first(sparse->chunks);

    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i].data);
    }

    ecs_vector_free(sparse->chunks);
    ecs_vector_free(sparse->free_stack);
    ecs_vector_free(sparse->sparse);
    ecs_vector_free(sparse->dense);

    ecs_os_free(sparse);
}

void ecs_sparse_clear(
    ecs_sparse_t *sparse)
{
    uint32_t i, count = ecs_vector_count(sparse->dense);
    uint32_t *dense = ecs_vector_first(sparse->dense);

    /* Recycle elements in dense set before clearing */
    for (i = 0; i < count; i ++) {
        _ecs_sparse_remove(sparse, 0, dense[i]);
    }

    ecs_vector_clear(sparse->dense);
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    uint32_t element_size)
{
    (void)element_size;
    ecs_assert(!element_size || element_size == sparse->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    uint32_t index = 0;
    
    if (!ecs_vector_pop(sparse->free_stack, &index)) {
        chunk_t *chunk = last_chunk(sparse);

        uint32_t elements_per_chunk = sparse->chunk_size;
        if (!chunk || chunk->count == elements_per_chunk) {
            add_chunk(sparse);
            chunk = last_chunk(sparse);
        }

        ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(chunk->count < elements_per_chunk, ECS_INTERNAL_ERROR, NULL);

        uint32_t chunk_count = ecs_vector_count(sparse->chunks);
        index = (chunk_count - 1) * elements_per_chunk + chunk->count;
        chunk->count ++;
    }

    return add_sparse(sparse, index);
}

void* _ecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint32_t element_size,
    uint32_t index)
{   
    (void)element_size;
    ecs_assert(!element_size || element_size == sparse->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    uint32_t *free_elem = ecs_vector_add(&sparse->free_stack, uint32_t);
    *free_elem = index;

    return get_sparse(sparse, index, true);
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    uint32_t element_size,
    uint32_t index)
{
    (void)element_size;
    ecs_assert(index < ecs_vector_count(sparse->dense), 
        ECS_INVALID_PARAMETER, NULL);
    
    ecs_assert(!element_size || element_size == sparse->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    const uint32_t *it = ecs_vector_first(sparse->dense);

    void *result = get_sparse(sparse, it[index], false);

    ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

    return result;
}

void* _ecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    uint32_t element_size,
    uint32_t index)
{
    (void)element_size;
    ecs_assert(!element_size || element_size == sparse->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_sparse(sparse, index, false);
}

void* _ecs_sparse_get_or_set_sparse(
    ecs_sparse_t *sparse,
    uint32_t element_size,
    uint32_t index,
    bool *is_new)
{
    (void)element_size;
    ecs_assert(!element_size || element_size == sparse->element_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_or_set_sparse(sparse, index, is_new);
}

uint32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_count(sparse->dense);
}

uint32_t ecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_count(sparse->sparse);
}

const uint32_t* ecs_sparse_indices(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_first(sparse->dense);
}

ecs_sparse_t* ecs_sparse_copy(
    const ecs_sparse_t *src)
{
    ecs_sparse_t *dst = ecs_os_memdup(src, sizeof(ecs_sparse_t));
    dst->chunks = ecs_vector_copy(src->chunks, chunk_t);
    dst->dense = ecs_vector_copy(src->dense, uint32_t);
    dst->sparse = ecs_vector_copy(src->sparse, sparse_elem_t);
    dst->free_stack = ecs_vector_copy(src->free_stack, uint32_t);

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

void ecs_sparse_memory(
    ecs_sparse_t *sparse,
    uint32_t *allocd,
    uint32_t *used)
{
    if (!sparse) {
        return;
    }

    ecs_vector_memory(sparse->chunks, chunk_t, allocd, used);
    ecs_vector_memory(sparse->dense, uint32_t, allocd, used);
    ecs_vector_memory(sparse->sparse, sparse_elem_t, allocd, used);
    ecs_vector_memory(sparse->free_stack, uint32_t, allocd, used);

    uint32_t data_total = sparse->chunk_size * 
        sparse->element_size * ecs_vector_count(sparse->chunks);

    uint32_t data_not_used = ecs_vector_count(sparse->free_stack) * 
        sparse->element_size;

    if (allocd) {
        *allocd += data_total;
    }

    if (used) {
        *used += data_total - data_not_used;
    }
}

void ecs_sparse_set_size(
    ecs_sparse_t *sparse,
    uint32_t size)
{   
    int32_t current = ecs_sparse_size(sparse);
    int32_t to_add = size - current;

    if (to_add > 0) {
        ecs_vector_set_size(&sparse->dense, uint32_t, size);
        ecs_vector_set_size(&sparse->sparse, sparse_elem_t, size);
        int32_t cur = ecs_vector_count(sparse->chunks) * sparse->chunk_size;

        while (cur < size) {
            add_chunk(sparse);
            cur += sparse->chunk_size;
        }
    }
}

void ecs_sparse_grow(
    ecs_sparse_t *sparse,
    uint32_t count)
{
    uint32_t current = ecs_sparse_count(sparse);

    while (ecs_sparse_size(sparse) <= (count + current)) {
        add_chunk(sparse);
    }
}