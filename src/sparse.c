#include "types.h"

#define CHUNK_ALLOC_SIZE (4096)

typedef struct chunk_t {
    void *data;
    int32_t count;
} chunk_t;

typedef struct sparse_elem_t {
    int32_t dense;
    void *ptr;
} sparse_elem_t;

struct ecs_sparse_t {
    ecs_vector_t *chunks;       /* Vector with chunk pointers */
    ecs_vector_t *dense;        /* Dense array */
    ecs_vector_t *sparse;       /* Sparse array + element pointers */
    ecs_vector_t *unused_chunks;   /* Unused chunks */
    ecs_vector_t *unused_elements; /* Unused elements */
    size_t elem_size;           /* Side of payload */
    int32_t chunk_size;         /* Chunk size */
};

static
void add_chunk(
    ecs_sparse_t *sparse)
{
    /* Add chunk to sparse instance */
    int32_t chunk_count = ecs_vector_count(sparse->chunks);
    
    chunk_t recycled_chunk, *chunk = ecs_vector_add(&sparse->chunks, chunk_t);

    /* Check if we have chunks we can recycle */
    if (ecs_vector_pop(sparse->unused_chunks, &recycled_chunk)) {
        *chunk = recycled_chunk;
    } else {
        chunk->data = ecs_os_malloc(CHUNK_ALLOC_SIZE);
    }

    int32_t chunk_size = sparse->chunk_size;
    size_t elem_size = sparse->elem_size;

    /* Allocate data vector for chunk */
    chunk->count = 0;

    /* Create room in sparse array for chunk */
    int32_t prev_total = chunk_count * chunk_size;

    ecs_vector_set_count(&sparse->sparse, sparse_elem_t, prev_total + chunk_size);

    ecs_assert(sparse->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Prepare cached pointers to chunk elements in sparse array */
    sparse_elem_t *sparse_array = ecs_vector_first(sparse->sparse);
    ecs_assert(sparse_array != NULL, ECS_INTERNAL_ERROR, NULL);

    sparse_array = &sparse_array[prev_total];

    int32_t i;
    for (i = 0; i < chunk_size; i ++) {
        sparse_array[i].dense = 0;
        sparse_array[i].ptr = ECS_OFFSET(chunk->data, i * elem_size);
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
    int32_t index)
{
    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);

    sparse_arr[index].dense = ecs_vector_count(sparse->dense);
    int32_t *dense = ecs_vector_add(&sparse->dense, int32_t);
    *dense = index;

    return sparse_arr[index].ptr;
}

static
void* get_sparse(
    const ecs_sparse_t *sparse,
    int32_t index,
    bool remove)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        return NULL;
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);
    int32_t dense = sparse_arr[index].dense;

    int32_t *dense_array = ecs_vector_first(sparse->dense);
    int32_t dense_count = ecs_vector_count(sparse->dense);

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
    int32_t index,
    bool *is_new)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        ecs_sparse_set_size(sparse, index + 1);
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INTERNAL_ERROR, NULL);
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse);
    int32_t dense = sparse_arr[index].dense;

    int32_t *dense_array = ecs_vector_first(sparse->dense);
    int32_t dense_count = ecs_vector_count(sparse->dense);

    if (dense >= dense_count || dense_array[dense] != index) {
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INVALID_PARAMETER, NULL);

        ecs_vector_add(&sparse->dense, int32_t);

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
    size_t elem_size,
    int32_t element_count)
{
    ecs_sparse_t *result = ecs_os_calloc(1, sizeof(ecs_sparse_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->chunk_size = CHUNK_ALLOC_SIZE / elem_size;
    result->elem_size = elem_size;

    ecs_sparse_set_size(result, element_count);

    return result;
}

static
void free_chunks(ecs_vector_t *chunks) {
    int i, count = ecs_vector_count(chunks);
    chunk_t *array = ecs_vector_first(chunks);

    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i].data);
    }    

    ecs_vector_free(chunks);
}

void ecs_sparse_free(
    ecs_sparse_t *sparse)
{
    if (sparse) {
        free_chunks(sparse->chunks);
        free_chunks(sparse->unused_chunks);

        ecs_vector_free(sparse->unused_elements);
        ecs_vector_free(sparse->sparse);
        ecs_vector_free(sparse->dense);

        ecs_os_free(sparse);
    }
}

void ecs_sparse_clear(
    ecs_sparse_t *sparse)
{        
    /* Recycle chunks */
    if (!sparse->unused_chunks) {
        sparse->unused_chunks = sparse->chunks;
        sparse->chunks = NULL;
    } else {
        chunk_t chunk;
        while (ecs_vector_pop(sparse->chunks, &chunk)) {
            chunk_t *unused = ecs_vector_add(&sparse->unused_chunks, chunk_t);
            *unused = chunk;
        }
    }

    ecs_vector_clear(sparse->dense);
    ecs_vector_clear(sparse->sparse);
    ecs_vector_clear(sparse->unused_elements);
}

void* _ecs_sparse_recycle(
    ecs_sparse_t *sparse,
    size_t elem_size,
    int32_t *sparse_index_out)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    int32_t index = 0;
    
    if (ecs_vector_pop(sparse->unused_elements, &index)) {
        if (sparse_index_out) {
            *sparse_index_out = index;
        }
        return add_sparse(sparse, index);
    } else {
        return NULL;
    }
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    size_t elem_size)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    void *result = _ecs_sparse_recycle(sparse, elem_size, NULL);
    if (!result) {
        int32_t index = 0;
        chunk_t *chunk = last_chunk(sparse);

        int32_t elements_per_chunk = sparse->chunk_size;
        if (!chunk || chunk->count == elements_per_chunk) {
            add_chunk(sparse);
            chunk = last_chunk(sparse);
        }

        ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(chunk->count < elements_per_chunk, ECS_INTERNAL_ERROR, NULL);

        int32_t chunk_count = ecs_vector_count(sparse->chunks);
        index = (chunk_count - 1) * elements_per_chunk + chunk->count;
        chunk->count ++;

        result = add_sparse(sparse, index);
    }

    return result;
}

void* _ecs_sparse_remove(
    ecs_sparse_t *sparse,
    size_t elem_size,
    int32_t index)
{   
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    void *result = get_sparse(sparse, index, true);
    if (result) {
        /* Only add to unused elements if index was set */
        int32_t *free_elem = ecs_vector_add(&sparse->unused_elements, int32_t);
        *free_elem = index;
    }

    return result;
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    size_t elem_size,
    int32_t index)
{
    (void)elem_size;
    ecs_assert(index < ecs_vector_count(sparse->dense), 
        ECS_INVALID_PARAMETER, NULL);
    
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    const int32_t *it = ecs_vector_first(sparse->dense);

    void *result = get_sparse(sparse, it[index], false);

    ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

    return result;
}

void* _ecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    size_t elem_size,
    int32_t index)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_sparse(sparse, index, false);
}

void* _ecs_sparse_get_or_set_sparse(
    ecs_sparse_t *sparse,
    size_t elem_size,
    int32_t index,
    bool *is_new)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_or_set_sparse(sparse, index, is_new);
}

int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return ecs_vector_count(sparse->dense);
}

int32_t ecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vector_count(sparse->sparse);
}

const int32_t* ecs_sparse_indices(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_first(sparse->dense);
}

const int32_t* ecs_sparse_unused_indices(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_first(sparse->unused_elements);
}

const int32_t ecs_sparse_unused_count(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_count(sparse->unused_elements);
}

ecs_sparse_t* ecs_sparse_copy(
    const ecs_sparse_t *src)
{
    if (!src) {
        return NULL;
    }
    
    ecs_sparse_t *dst = ecs_os_memdup(src, sizeof(ecs_sparse_t));
    dst->chunks = ecs_vector_copy(src->chunks, chunk_t);
    dst->dense = ecs_vector_copy(src->dense, int32_t);
    dst->sparse = ecs_vector_copy(src->sparse, sparse_elem_t);
    dst->unused_elements = ecs_vector_copy(src->unused_elements, int32_t);

    /* Iterate chunks, copy data */
    sparse_elem_t *sparse_array = ecs_vector_first(dst->sparse);
    chunk_t *chunks = ecs_vector_first(dst->chunks);
    int32_t i, count = ecs_vector_count(dst->chunks);

    for (i = 0; i < count; i ++) {
        chunks[i].data = ecs_os_memdup(
            chunks[i].data, dst->chunk_size * dst->elem_size);
        
        int32_t j;
        for (j = 0; j < dst->chunk_size; j ++) {
            sparse_array[i * dst->chunk_size + j].ptr = 
                ECS_OFFSET(chunks[i].data, j * dst->elem_size);
        }
    }

    return dst;
}

void ecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used)
{
    if (!sparse) {
        return;
    }

    ecs_vector_memory(sparse->chunks, chunk_t, allocd, used);
    ecs_vector_memory(sparse->dense, int32_t, allocd, used);
    ecs_vector_memory(sparse->sparse, sparse_elem_t, allocd, used);
    ecs_vector_memory(sparse->unused_elements, int32_t, allocd, used);

    int32_t data_total = sparse->chunk_size * 
        sparse->elem_size * ecs_vector_count(sparse->chunks);

    int32_t data_not_used = ecs_vector_count(sparse->unused_elements) * 
        sparse->elem_size;

    if (allocd) {
        *allocd += data_total;
    }

    if (used) {
        *used += data_total - data_not_used;
    }
}

void ecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{   
    int32_t current = ecs_sparse_size(sparse);
    int32_t to_add = elem_count - current;

    if (to_add > 0) {
        ecs_vector_set_size(&sparse->sparse, sparse_elem_t, elem_count);
        int32_t cur = ecs_vector_count(sparse->chunks) * sparse->chunk_size;
        
        while (cur < elem_count) {
            add_chunk(sparse);
            cur += sparse->chunk_size;
        }
    }

    if (ecs_vector_size(sparse->dense) < elem_count) {
        ecs_vector_set_size(&sparse->dense, int32_t, elem_count);
    }
}

void ecs_sparse_grow(
    ecs_sparse_t *sparse,
    int32_t count)
{
    int32_t current = ecs_sparse_count(sparse);

    while (ecs_sparse_size(sparse) <= (count + current)) {
        add_chunk(sparse);
    }
}