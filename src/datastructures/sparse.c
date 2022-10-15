#include "../private_api.h"

/** Compute the chunk index from an id by stripping the first 12 bits */
#define CHUNK(index) ((int32_t)((uint32_t)index >> 12))

/** This computes the offset of an index inside a chunk */
#define OFFSET(index) ((int32_t)index & 0xFFF)

/* Utility to get a pointer to the payload */
#define DATA(array, size, offset) (ECS_OFFSET(array, size * offset))

typedef struct chunk_t {
    int32_t *sparse;            /* Sparse array with indices to dense array */
    void *data;                 /* Store data in sparse array to reduce  
                                 * indirection and provide stable pointers. */
} chunk_t;

static
chunk_t* flecs_sparse_chunk_new(
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
    if (sparse->chunk_allocator) {
        result->sparse = flecs_bcalloc(sparse->chunk_allocator);
    } else {
        result->sparse = ecs_os_calloc_n(int32_t, FLECS_SPARSE_CHUNK_SIZE);
    }

    /* Initialize the data array with zero's to guarantee that data is 
     * always initialized. When an entry is removed, data is reset back to
     * zero. Initialize now, as this can take advantage of calloc. */
    if (sparse->allocator) {
        result->data = flecs_calloc(sparse->allocator,
            sparse->size * FLECS_SPARSE_CHUNK_SIZE);
    } else {
        result->data = ecs_os_calloc(sparse->size * FLECS_SPARSE_CHUNK_SIZE);
    }

    ecs_assert(result->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

static
void flecs_sparse_chunk_free(
    ecs_sparse_t *sparse,
    chunk_t *chunk)
{
    if (sparse->chunk_allocator) {
        flecs_bfree(sparse->chunk_allocator, chunk->sparse);
    } else {
        ecs_os_free(chunk->sparse);
    }
    if (sparse->allocator) {
        flecs_free(sparse->allocator, sparse->size * FLECS_SPARSE_CHUNK_SIZE,
            chunk->data);
    } else {
        ecs_os_free(chunk->data);
    }
}

static
chunk_t* flecs_sparse_get_chunk(
    const ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    if (!sparse->chunks) {
        return NULL;
    }
    if (chunk_index >= ecs_vector_count(sparse->chunks)) {
        return NULL;
    }

    /* If chunk_index is below zero, application used an invalid entity id */
    ecs_assert(chunk_index >= 0, ECS_INVALID_PARAMETER, NULL);
    chunk_t *result = ecs_vector_get(sparse->chunks, chunk_t, chunk_index);
    if (result && !result->sparse) {
        return NULL;
    }

    return result;
}

static
chunk_t* flecs_sparse_get_or_create_chunk(
    ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, chunk_index);
    if (chunk) {
        return chunk;
    }

    return flecs_sparse_chunk_new(sparse, chunk_index);
}

static
void flecs_sparse_grow_dense(
    ecs_sparse_t *sparse)
{
    ecs_vector_add(&sparse->dense, uint64_t);
}

static
uint64_t flecs_sparse_strip_generation(
    uint64_t *index_out)
{
    uint64_t index = *index_out;
    uint64_t gen = index & ECS_GENERATION_MASK;
    /* Make sure there's no junk in the id */
    ecs_assert(gen == (index & (0xFFFFFFFFull << 32)),
        ECS_INVALID_PARAMETER, NULL);
    *index_out -= gen;
    return gen;
}

static
void flecs_sparse_assign_index(
    chunk_t * chunk, 
    uint64_t * dense_array, 
    uint64_t index, 
    int32_t dense)
{
    /* Initialize sparse-dense pair. This assigns the dense index to the sparse
     * array, and the sparse index to the dense array .*/
    chunk->sparse[OFFSET(index)] = dense;
    dense_array[dense] = index;
}

static
uint64_t flecs_sparse_inc_gen(
    uint64_t index)
{
    /* When an index is deleted, its generation is increased so that we can do
     * liveliness checking while recycling ids */
    return ECS_GENERATION_INC(index);
}

static
uint64_t flecs_sparse_inc_id(
    ecs_sparse_t *sparse)
{
    /* Generate a new id. The last issued id could be stored in an external
     * variable, such as is the case with the last issued entity id, which is
     * stored on the world. */
    return ++ (sparse->max_id[0]);
}

static
uint64_t flecs_sparse_get_id(
    const ecs_sparse_t *sparse)
{
    return sparse->max_id[0];
}

static
void flecs_sparse_set_id(
    ecs_sparse_t *sparse,
    uint64_t value)
{
    /* Sometimes the max id needs to be assigned directly, which typically 
     * happens when the API calls get_or_create for an id that hasn't been 
     * issued before. */
    sparse->max_id[0] = value;
}

/* Pair dense id with new sparse id */
static
uint64_t flecs_sparse_create_id(
    ecs_sparse_t *sparse,
    int32_t dense)
{
    uint64_t index = flecs_sparse_inc_id(sparse);
    flecs_sparse_grow_dense(sparse);

    chunk_t *chunk = flecs_sparse_get_or_create_chunk(sparse, CHUNK(index));
    ecs_assert(chunk->sparse[OFFSET(index)] == 0, ECS_INTERNAL_ERROR, NULL);
    
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    flecs_sparse_assign_index(chunk, dense_array, index, dense);
    
    return index;
}

/* Create new id */
static
uint64_t flecs_sparse_new_index(
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
        return flecs_sparse_create_id(sparse, count);
    }
}

/* Try obtaining a value from the sparse set, don't care about whether the
 * provided index matches the current generation count.  */
static
void* flecs_sparse_try_sparse_any(
    const ecs_sparse_t *sparse,
    uint64_t index)
{    
    flecs_sparse_strip_generation(&index);

    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
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

/* Try obtaining a value from the sparse set, make sure it's alive. */
static
void* flecs_sparse_try_sparse(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    uint64_t gen = flecs_sparse_strip_generation(&index);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;

    if (cur_gen != gen) {
        return NULL;
    }

    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, sparse->size, offset);
}

/* Get value from sparse set when it is guaranteed that the value exists. This
 * function is used when values are obtained using a dense index */
static
void* flecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    int32_t dense,
    uint64_t index)
{
    flecs_sparse_strip_generation(&index);
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }
    int32_t offset = OFFSET(index);
    
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    (void)dense;

    return DATA(chunk->data, sparse->size, offset);
}

/* Swap dense elements. A swap occurs when an element is removed, or when a
 * removed element is recycled. */
static
void flecs_sparse_swap_dense(
    ecs_sparse_t * sparse,
    chunk_t * chunk_a,
    int32_t a,
    int32_t b)
{
    ecs_assert(a != b, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t index_a = dense_array[a];
    uint64_t index_b = dense_array[b];

    chunk_t *chunk_b = flecs_sparse_get_or_create_chunk(sparse, CHUNK(index_b));
    flecs_sparse_assign_index(chunk_a, dense_array, index_a, b);
    flecs_sparse_assign_index(chunk_b, dense_array, index_b, a);
}

void _flecs_sparse_init(
    ecs_sparse_t *result,
    struct ecs_allocator_t *allocator,
    ecs_block_allocator_t *chunk_allocator,
    ecs_size_t size)
{
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    result->size = size;
    result->max_id_local = UINT64_MAX;
    result->max_id = &result->max_id_local;
    result->allocator = allocator;
    result->chunk_allocator = chunk_allocator;

    /* Consume first value in dense array as 0 is used in the sparse array to
     * indicate that a sparse element hasn't been paired yet. */
    uint64_t *first = ecs_vector_add(&result->dense, uint64_t);
    *first = 0;

    result->count = 1;
}

ecs_sparse_t* _flecs_sparse_new(
    struct ecs_allocator_t *allocator,
    ecs_block_allocator_t *chunk_allocator,
    ecs_size_t size)
{
    ecs_sparse_t *result = ecs_os_calloc_t(ecs_sparse_t);

    _flecs_sparse_init(result, allocator, chunk_allocator, size);

    return result;
}

void flecs_sparse_set_id_source(
    ecs_sparse_t * sparse,
    uint64_t * id_source)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    sparse->max_id = id_source;
}

void flecs_sparse_clear(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t i, count = ecs_vector_count(sparse->chunks);
    chunk_t *chunks = ecs_vector_first(sparse->chunks, chunk_t);
    for (i = 0; i < count; i ++) {
        int32_t *indices = chunks[i].sparse;
        if (indices) {
            ecs_os_memset_n(indices, 0, int32_t, FLECS_SPARSE_CHUNK_SIZE);
        }
    }

    ecs_vector_set_count(&sparse->dense, uint64_t, 1);

    sparse->count = 1;
    sparse->max_id_local = 0;
}

void _flecs_sparse_fini(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t i, count = ecs_vector_count(sparse->chunks);
    chunk_t *chunks = ecs_vector_first(sparse->chunks, chunk_t);
    for (i = 0; i < count; i ++) {
        flecs_sparse_chunk_free(sparse, &chunks[i]);
    }

    ecs_vector_free(sparse->chunks);
    ecs_vector_free(sparse->dense);

    sparse->chunks = NULL;
    sparse->dense = NULL;
}

void flecs_sparse_free(
    ecs_sparse_t *sparse)
{
    if (sparse) {
        _flecs_sparse_fini(sparse);
        ecs_os_free(sparse);
    }
}

uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_sparse_new_index(sparse);
}

const uint64_t* flecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t new_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t dense_count = ecs_vector_count(sparse->dense);
    int32_t count = sparse->count;
    int32_t recyclable = dense_count - count;
    int32_t i, to_create = new_count - recyclable;

    if (to_create > 0) {
        flecs_sparse_set_size(sparse, dense_count + to_create);
        for (i = 0; i < to_create; i ++) {
            flecs_sparse_create_id(sparse, count + recyclable + i);
        }
    }

    sparse->count += new_count;

    return ecs_vector_get(sparse->dense, uint64_t, count);
}

void* _flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t size)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    uint64_t index = flecs_sparse_new_index(sparse);
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, size, OFFSET(index));
}

uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return dense_array[sparse->count - 1];
}

void* _flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vector_count(sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint64_t gen = flecs_sparse_strip_generation(&index);
    chunk_t *chunk = flecs_sparse_get_or_create_chunk(sparse, CHUNK(index));
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
            flecs_sparse_swap_dense(sparse, chunk, dense, count);

            /* First unused element is now last used element */
            sparse->count ++;
        } else {
            /* Dense is already alive, nothing to be done */
        }

        /* Ensure provided generation matches current. Only allow mismatching
         * generations if the provided generation count is 0. This allows for
         * using the ensure function in combination with ids that have their
         * generation stripped. */
        ecs_vector_t *dense_vector = sparse->dense;
        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);    
        ecs_assert(!gen || dense_array[dense] == (index | gen), ECS_INTERNAL_ERROR, NULL);
        (void)dense_vector;
        (void)dense_array;
    } else {
        /* Element is not paired yet. Must add a new element to dense array */
        flecs_sparse_grow_dense(sparse);

        ecs_vector_t *dense_vector = sparse->dense;
        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);    
        int32_t dense_count = ecs_vector_count(dense_vector) - 1;
        int32_t count = sparse->count ++;

        /* If index is larger than max id, update max id */
        if (index >= flecs_sparse_get_id(sparse)) {
            flecs_sparse_set_id(sparse, index);
        }

        if (count < dense_count) {
            /* If there are unused elements in the list, move the first unused
             * element to the end of the list */
            uint64_t unused = dense_array[count];
            chunk_t *unused_chunk = flecs_sparse_get_or_create_chunk(sparse, CHUNK(unused));
            flecs_sparse_assign_index(unused_chunk, dense_array, unused, dense_count);
        }

        flecs_sparse_assign_index(chunk, dense_array, index, count);
        dense_array[count] |= gen;
    }

    return DATA(chunk->data, sparse->size, offset);
}

void* _flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index_long)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vector_count(sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint32_t index = (uint32_t)index_long;
    chunk_t *chunk = flecs_sparse_get_or_create_chunk(sparse, CHUNK(index));
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (!dense) {
        /* Element is not paired yet. Must add a new element to dense array */
        ecs_vector_t *dense_vector = sparse->dense;
        int32_t count = sparse->count ++;
        if (count == ecs_vector_count(dense_vector)) {
            flecs_sparse_grow_dense(sparse);
            dense_vector = sparse->dense;
        }

        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);
        flecs_sparse_assign_index(chunk, dense_array, index, count);
    }

    return DATA(chunk->data, sparse->size, offset);
}

void* _flecs_sparse_set(
    ecs_sparse_t * sparse,
    ecs_size_t elem_size,
    uint64_t index,
    void* value)
{
    void *ptr = _flecs_sparse_ensure(sparse, elem_size, index);
    ecs_os_memcpy(ptr, value, elem_size);
    return ptr;
}

void* _flecs_sparse_remove_get(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    uint64_t gen = flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
        uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
        if (gen != cur_gen) {
            /* Generation doesn't match which means that the provided entity is
             * already not alive. */
            return NULL;
        }

        /* Increase generation */
        dense_array[dense] = index | flecs_sparse_inc_gen(cur_gen);
        
        int32_t count = sparse->count;
        
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            flecs_sparse_swap_dense(sparse, chunk, dense, count - 1);
            sparse->count --;
        } else {
            /* Element is not alive, nothing to be done */
            return NULL;
        }

        /* Reset memory to zero on remove */
        return DATA(chunk->data, sparse->size, offset);
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
        return NULL;
    }
}

void* _flecs_sparse_remove_fast(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index_long)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    uint32_t index = (uint32_t)index_long;
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) { 
        int32_t count = -- sparse->count;
        if (dense != count) {
            flecs_sparse_swap_dense(sparse, chunk, dense, count);
        }

        return DATA(chunk->data, sparse->size, offset);
    }
    return NULL;
}

void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    void *ptr = _flecs_sparse_remove_get(sparse, 0, index);
    if (ptr) {
        ecs_os_memset(ptr, 0, sparse->size);
    }
}

void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = flecs_sparse_get_or_create_chunk(sparse, CHUNK(index));
    
    uint64_t index_w_gen = index;
    flecs_sparse_strip_generation(&index);
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

bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return false;
    }
    
    flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    return dense != 0;
}

bool flecs_sparse_is_valid(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return true; /* Doesn't exist yet, so is valid */
    }
    
    flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    if (!dense) {
        return true; /* Doesn't exist yet, so is valid */
    }

    /* If the id exists, it must be alive */
    return dense < sparse->count;
}

void* _flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    int32_t dense_index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense_index < sparse->count, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    dense_index ++;

    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return flecs_sparse_get_sparse(sparse, dense_index, dense_array[dense_index]);
}

bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    return flecs_sparse_try_sparse(sparse, index) != NULL;
}

uint64_t flecs_sparse_get_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    chunk_t *chunk = flecs_sparse_get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return 0;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);

    /* If dense is 0 (tombstone) this will return 0 */
    return dense_array[dense];
}

void* _flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    return flecs_sparse_try_sparse(sparse, index);
}

void* _flecs_sparse_get_any(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    return flecs_sparse_try_sparse_any(sparse, index);
}

int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return sparse->count - 1;
}

int32_t flecs_sparse_not_alive_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return ecs_vector_count(sparse->dense) - sparse->count;
}

int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vector_count(sparse->dense) - 1;
}

const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return &(ecs_vector_first(sparse->dense, uint64_t)[1]);
}

void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_vector_set_size(&sparse->dense, uint64_t, elem_count);
}

static
void sparse_copy(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    flecs_sparse_set_size(dst, flecs_sparse_size(src));
    const uint64_t *indices = flecs_sparse_ids(src);
    
    ecs_size_t size = src->size;
    int32_t i, count = src->count;

    for (i = 0; i < count - 1; i ++) {
        uint64_t index = indices[i];
        void *src_ptr = _flecs_sparse_get(src, size, index);
        void *dst_ptr = _flecs_sparse_ensure(dst, size, index);
        flecs_sparse_set_generation(dst, index);
        ecs_os_memcpy(dst_ptr, src_ptr, size);
    }

    flecs_sparse_set_id(dst, flecs_sparse_get_id(src));

    ecs_assert(src->count == dst->count, ECS_INTERNAL_ERROR, NULL);
}

ecs_sparse_t* flecs_sparse_copy(
    const ecs_sparse_t *src)
{
    if (!src) {
        return NULL;
    }

    ecs_sparse_t *dst = _flecs_sparse_new(
        src->allocator, src->chunk_allocator, src->size);
    sparse_copy(dst, src);

    return dst;
}

void flecs_sparse_restore(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    dst->count = 1;
    if (src) {
        sparse_copy(dst, src);
    }
}

ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size)
{
    return _flecs_sparse_new(NULL, NULL, elem_size);
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size)
{
    return _flecs_sparse_add(sparse, elem_size);
}

uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    return flecs_sparse_last_id(sparse);
}

int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    return flecs_sparse_count(sparse);
}

void* _ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{
    return _flecs_sparse_get_dense(sparse, elem_size, index);
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id)
{
    return _flecs_sparse_get(sparse, elem_size, id);
}
