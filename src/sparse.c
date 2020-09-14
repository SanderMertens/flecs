#include "flecs.h"

#define CHUNK_COUNT (4096)
#define CHUNK(index) (int32_t)(index >> 12)
#define OFFSET(index) (int32_t)(index & 0xFFF)
#define DATA(array, size, offset) (ECS_OFFSET(array, size * offset))

typedef struct chunk_t {
    int32_t *sparse;            /* Sparse array with indices to dense array */
    void *data;                 /* Store data in sparse array to reduce  
                                 * indirection and provide stable pointers. */
} chunk_t;

struct ecs_sparse_t {
    ecs_vector_t *dense;        /* Dense array with indices to sparse array. The
                                 * dense array stores both alive and not alive
                                 * sparse indices. The 'count' member keeps
                                 * track of which indices are alive. */

    ecs_vector_t *chunks;       /* Chunks with sparse arrays & data */
    ecs_size_t size;            /* Element size */
    int32_t count;              /* Number of alive entries */
    uint64_t max_id_local;      /* Local max index (if no global is set) */
    uint64_t *max_id;           /* Maximum issued sparse index */
};

static
chunk_t* chunk_new(
    ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    int32_t count = ecs_vector_count(sparse->chunks);
    chunk_t *chunks;

    if (count <= chunk_index) {
        ecs_vector_set_count(&sparse->chunks, chunk_t, chunk_index + 1);
        chunks = ecs_vector_first(sparse->chunks, chunk_t);
        ecs_os_memset(&chunks[count], 0, (1 + chunk_index - count) * ECS_SIZEOF(chunk_t));
    } else {
        chunks = ecs_vector_first(sparse->chunks, chunk_t);
    }

    ecs_assert(chunks != NULL, ECS_INTERNAL_ERROR, NULL);

    chunk_t *result = &chunks[chunk_index];
    ecs_assert(result->sparse == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data == NULL, ECS_INTERNAL_ERROR, NULL);

    /* Initialize sparse array with zero's, as zero is used to indicate that the
     * sparse element has not been paired with a dense element. Use zero
     * as this means we can take advantage of calloc having a possibly better 
     * performance than malloc + memset. */
    result->sparse = ecs_os_calloc(ECS_SIZEOF(int32_t) * CHUNK_COUNT);

    /* Initialize the data array with zero's to guarantee that data is 
     * always initialized. When an entry is removed, data is reset back to
     * zero. Initialize now, as this can take advantage of calloc. */
    result->data = ecs_os_calloc(sparse->size * CHUNK_COUNT);

    ecs_assert(result->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

static
void chunk_free(
    chunk_t *chunk)
{
    ecs_os_free(chunk->sparse);
    ecs_os_free(chunk->data);
}

static
chunk_t* get_chunk(
    const ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    chunk_t *result = ecs_vector_get(sparse->chunks, chunk_t, chunk_index);
    if (result && !result->sparse) {
        return NULL;
    }

    return result;
}

static
chunk_t* get_or_create_chunk(
    ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    chunk_t *chunk = get_chunk(sparse, chunk_index);
    if (chunk) {
        return chunk;
    }

    return chunk_new(sparse, chunk_index);
}

static
void grow_dense(
    ecs_sparse_t *sparse)
{
    ecs_vector_add(&sparse->dense, uint64_t);
}

static
uint64_t strip_generation(
    uint64_t *index_out)
{
    uint64_t index = *index_out;
    uint64_t gen = index & ECS_GENERATION_MASK;
    *index_out -= gen;
    return gen;
}

static
void assign_index(
    chunk_t *chunk, 
    uint64_t *dense_array, 
    uint64_t index, 
    int32_t dense)
{
    chunk->sparse[OFFSET(index)] = dense;
    dense_array[dense] = index;
}

static
uint64_t inc_gen(
    uint64_t index)
{
    return ECS_GENERATION_INC(index);
}

static
uint64_t inc_id(
    ecs_sparse_t *sparse)
{
    return ++ (sparse->max_id[0]);
}

static
uint64_t get_id(
    const ecs_sparse_t *sparse)
{
    return sparse->max_id[0];
}

static
void set_id(
    ecs_sparse_t *sparse,
    uint64_t value)
{
    sparse->max_id[0] = value;
}

static
uint64_t create_id(
    ecs_sparse_t *sparse,
    int32_t dense)
{
    uint64_t index = inc_id(sparse);
    grow_dense(sparse);

    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    ecs_assert(chunk->sparse[OFFSET(index)] == 0, ECS_INTERNAL_ERROR, NULL);
    
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    assign_index(chunk, dense_array, index, dense);
    
    return index;
}

static
uint64_t new_index(
    ecs_sparse_t *sparse)
{
    ecs_vector_t *dense = sparse->dense;
    int32_t dense_count = ecs_vector_count(dense);
    int32_t count = sparse->count ++;

    ecs_assert(count <= dense_count, ECS_INTERNAL_ERROR, NULL);

    if (count < dense_count) {
        /* If there are unused elements in the dense array, return first */
        uint64_t *dense_array = ecs_vector_first(dense, uint64_t);
        return dense_array[count];
    } else {
        return create_id(sparse, count);
    }
}

static
void* try_sparse_any(
    const ecs_sparse_t *sparse,
    uint64_t index)
{    
    strip_generation(&index);

    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, sparse->size, offset);
}

static
void* try_sparse(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    uint64_t gen = strip_generation(&index);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
    if (cur_gen != gen) {
        return NULL;
    }

    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return try_sparse_any(sparse, index);
}

static
void* get_sparse(
    const ecs_sparse_t *sparse,
    int32_t dense,
    uint64_t index)
{
    strip_generation(&index);
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    int32_t offset = OFFSET(index);
    
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);

    return DATA(chunk->data, sparse->size, offset);
}

static
void swap_dense(
    ecs_sparse_t *sparse,
    chunk_t *chunk_a,
    int32_t a,
    int32_t b,
    uint64_t index_a)
{
    ecs_assert(a != b, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t index_b = dense_array[b];

    chunk_t *chunk_b = get_or_create_chunk(sparse, CHUNK(index_b));
    assign_index(chunk_a, dense_array, index_a, b);
    assign_index(chunk_b, dense_array, index_b, a);
}

ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t size)
{
    ecs_sparse_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_sparse_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    result->size = size;
    result->max_id_local = UINT64_MAX;
    result->max_id = &result->max_id_local;

    /* Consume first value in dense array as 0 is used in the sparse array to
     * indicate that a sparse element hasn't been paired yet. */
    ecs_vector_add(&result->dense, uint64_t);
    result->count = 1;

    return result;
}

void ecs_sparse_set_id_source(
    ecs_sparse_t *sparse,
    uint64_t *id_source)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    sparse->max_id = id_source;
}

void ecs_sparse_clear(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_vector_each(sparse->chunks, chunk_t, chunk, {
        chunk_free(chunk);
    });

    ecs_vector_free(sparse->chunks);
    ecs_vector_set_count(&sparse->dense, uint64_t, 1);

    sparse->chunks = NULL;   
    sparse->count = 1;
    sparse->max_id_local = 0;
}

void ecs_sparse_free(
    ecs_sparse_t *sparse)
{
    if (sparse) {
        ecs_sparse_clear(sparse);
        ecs_vector_free(sparse->dense);
        ecs_os_free(sparse);
    }
}

uint64_t ecs_sparse_new_id(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return new_index(sparse);
}

const uint64_t* ecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t new_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t dense_count = ecs_vector_count(sparse->dense);
    int32_t count = sparse->count;
    int32_t remaining = dense_count - count;
    int32_t i, to_create = new_count - remaining;

    if (to_create > 0) {
        ecs_sparse_set_size(sparse, dense_count + to_create);
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);

        for (i = 0; i < to_create; i ++) {
            uint64_t index = create_id(sparse, count + i);
            dense_array[dense_count + i] = index;
        }
    }

    sparse->count += new_count;

    return ecs_vector_get(sparse->dense, uint64_t, count);
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t size)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    uint64_t index = new_index(sparse);
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, size, OFFSET(index));
}

uint64_t ecs_sparse_last_id(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return dense_array[sparse->count - 1];
}

void* _ecs_sparse_get_or_create(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vector_count(sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t gen = strip_generation(&index);
    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        /* Check if element is alive. If element is not alive, update indices so
         * that the first unused dense element points to the sparse element. */
        int32_t count = sparse->count;
        if (dense == count) {
            /* If dense is the next unused element in the array, simply increase
             * the count to make it part of the alive set. */
            sparse->count ++;
        } else if (dense > count) {
            /* If dense is not alive, swap it with the first unused element. */
            swap_dense(sparse, chunk, dense, count, index);

            /* First unused element is now last used element */
            sparse->count ++;
        } else {
            /* Dense is already alive, nothing to be done */
        }
    } else {
        /* Element is not paired yet. Must add a new element to dense array */
        grow_dense(sparse);

        ecs_vector_t *dense_vector = sparse->dense;
        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);
        int32_t dense_count = ecs_vector_count(dense_vector) - 1;
        int32_t count = sparse->count ++;

        /* If index is larger than max id, update max id */
        if (index >= get_id(sparse)) {
            set_id(sparse, index + 1);
        }

        if (count < dense_count) {
            /* If there are unused elements in the list, move the first unused
             * element to the end of the list */
            uint64_t unused = dense_array[count];
            chunk_t *unused_chunk = get_or_create_chunk(sparse, CHUNK(unused));
            assign_index(unused_chunk, dense_array, unused, dense_count);
        }

        assign_index(chunk, dense_array, index, count);
        dense_array[count] |= gen;
    }

    return DATA(chunk->data, sparse->size, offset);
}

void* _ecs_sparse_set(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t index,
    void *value)
{
    void *ptr = _ecs_sparse_get_or_create(sparse, elem_size, index);
    ecs_os_memcpy(ptr, value, elem_size);
    return ptr;
}

void _ecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    uint64_t gen = strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
        uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
        if (gen != cur_gen) {
            /* Generation doesn't match which means that the provided entity is
             * already not alive. */
            return;
        }

        /* Increase generation */
        dense_array[dense] = index | inc_gen(cur_gen);
        
        int32_t count = sparse->count;
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            swap_dense(sparse, chunk, dense, count - 1, index);
            sparse->count --;
        } else {
            /* Element is not alive, nothing to be done */
        }

        /* Reset memory to zero on remove */
        ecs_size_t size = sparse->size;
        void *ptr = DATA(chunk->data, size, offset);
        ecs_os_memset(ptr, 0, size);
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
    }
}

void ecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    
    uint64_t index_w_gen = index;
    strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        /* Increase generation */
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
        dense_array[dense] = index_w_gen;
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
    }
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    int32_t dense_index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense_index < sparse->count, ECS_INVALID_PARAMETER, NULL);

    dense_index ++;

    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return get_sparse(sparse, dense_index, dense_array[dense_index]);
}

bool ecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    return try_sparse(sparse, index) != NULL;
}

void* _ecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    return try_sparse(sparse, index);
}

void* _ecs_sparse_get_sparse_any(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    return try_sparse_any(sparse, index);
}

int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return sparse->count - 1;
}

int32_t ecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vector_count(sparse->dense) - 1;
}

const uint64_t* ecs_sparse_ids(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return &(ecs_vector_first(sparse->dense, uint64_t)[1]);
}

void ecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_vector_set_size(&sparse->dense, uint64_t, elem_count);
}

void ecs_sparse_grow(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_vector_grow(&sparse->dense, uint64_t, elem_count);
}

static
void sparse_copy(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src)
{
    ecs_sparse_set_size(dst, ecs_sparse_size(src));
    const uint64_t *indices = ecs_sparse_ids(src);
    
    ecs_size_t size = src->size;
    int32_t i, count = src->count;

    for (i = 0; i < count - 1; i ++) {
        uint64_t index = indices[i];
        void *src_ptr = _ecs_sparse_get_sparse(src, size, index);
        void *dst_ptr = _ecs_sparse_get_or_create(dst, size, index);
        ecs_sparse_set_generation(dst, index);
        ecs_os_memcpy(dst_ptr, src_ptr, size);
    }

    set_id(dst, get_id(src));

    ecs_assert(src->count == dst->count, ECS_INTERNAL_ERROR, NULL);
}

ecs_sparse_t* ecs_sparse_copy(
    const ecs_sparse_t *src)
{
    if (!src) {
        return NULL;
    }

    ecs_sparse_t *dst = _ecs_sparse_new(src->size);
    sparse_copy(dst, src);

    return dst;
}

void ecs_sparse_restore(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    dst->count = 1;
    if (src) {
        sparse_copy(dst, src);
    }
}

void ecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used)
{
    (void)sparse;
    (void)allocd;
    (void)used;
}
