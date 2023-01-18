/**
 * @file datastructures/sparse.c
 * @brief Sparse set data structure.
 * 
 * The sparse set data structure allows for fast lookups by 64bit key with 
 * variable payload size. Lookup operations are guaranteed to be O(1), in 
 * contrast to ecs_map_t which has O(1) lookup time on average. At a high level
 * the sparse set works with two arrays:
 * 
 * dense  [ - ][ 3 ][ 1 ][ 4 ]
 * sparse [   ][ 2 ][   ][ 1 ][ 3 ]
 * 
 * Indices in the dense array point to the sparse array, and vice versa. The
 * dense array is guaranteed to contain no holes. By iterating the dense array, 
 * all populated elements in the sparse set can be found without having to skip
 * empty elements.
 * 
 * The sparse array is paged, which means that it is split up in memory blocks
 * (pages, not to be confused with OS pages) of equal size. The page size is
 * set to 4096 elements. Paging prevents the sparse array from having to grow
 * to N elements, where N is the largest key used in the set. It also ensures
 * that payload pointers are stable.
 * 
 * The sparse set recycles deleted keys. It does this by moving not alive keys
 * to the end of the dense array, and using a count member that indicates the
 * last alive member in the dense array. This approach makes it possible to
 * recycle keys in bulk, by increasing the alive count.
 * 
 * When a key is deleted, the sparse set increases its generation count. This
 * generation count is used to test whether a key passed to the sparse set is
 * still valid, or whether it has been deleted.
 * 
 * The sparse set is used in a number of places, like for retrieving entity
 * administration, tables and allocators.
 */

#include "../private_api.h"

/** Compute the page index from an id by stripping the first 12 bits */
#define PAGE(index) ((int32_t)((uint32_t)index >> 12))

/** This computes the offset of an index inside a page */
#define OFFSET(index) ((int32_t)index & 0xFFF)

/* Utility to get a pointer to the payload */
#define DATA(array, size, offset) (ECS_OFFSET(array, size * offset))

typedef struct ecs_page_t {
    int32_t *sparse;            /* Sparse array with indices to dense array */
    void *data;                 /* Store data in sparse array to reduce  
                                 * indirection and provide stable pointers. */
} ecs_page_t;

static
ecs_page_t* flecs_sparse_page_new(
    ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_allocator_t *a = sparse->allocator;
    ecs_block_allocator_t *ca = sparse->page_allocator;
    int32_t count = ecs_vec_count(&sparse->pages);
    ecs_page_t *pages;

    if (count <= page_index) {
        ecs_vec_set_count_t(a, &sparse->pages, ecs_page_t, page_index + 1);
        pages = ecs_vec_first_t(&sparse->pages, ecs_page_t);
        ecs_os_memset_n(&pages[count], 0, ecs_page_t, (1 + page_index - count));
    } else {
        pages = ecs_vec_first_t(&sparse->pages, ecs_page_t);
    }

    ecs_assert(pages != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_page_t *result = &pages[page_index];
    ecs_assert(result->sparse == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data == NULL, ECS_INTERNAL_ERROR, NULL);

    /* Initialize sparse array with zero's, as zero is used to indicate that the
     * sparse element has not been paired with a dense element. Use zero
     * as this means we can take advantage of calloc having a possibly better 
     * performance than malloc + memset. */
    result->sparse = ca ? flecs_bcalloc(ca)
                        : ecs_os_calloc_n(int32_t, FLECS_SPARSE_CHUNK_SIZE);

    /* Initialize the data array with zero's to guarantee that data is 
     * always initialized. When an entry is removed, data is reset back to
     * zero. Initialize now, as this can take advantage of calloc. */
    result->data = a ? flecs_calloc(a, sparse->size * FLECS_SPARSE_CHUNK_SIZE)
                     : ecs_os_calloc(sparse->size * FLECS_SPARSE_CHUNK_SIZE);

    ecs_assert(result->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

static
void flecs_sparse_page_free(
    ecs_sparse_t *sparse,
    ecs_page_t *page)
{
    ecs_allocator_t *a = sparse->allocator;
    ecs_block_allocator_t *ca = sparse->page_allocator;

    if (ca) {
        flecs_bfree(ca, page->sparse);
    } else {
        ecs_os_free(page->sparse);
    }
    if (a) {
        flecs_free(a, sparse->size * FLECS_SPARSE_CHUNK_SIZE, page->data);
    } else {
        ecs_os_free(page->data);
    }
}

static
ecs_page_t* flecs_sparse_get_page(
    const ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_assert(page_index >= 0, ECS_INVALID_PARAMETER, NULL);
    if (page_index >= ecs_vec_count(&sparse->pages)) {
        return NULL;
    }
    return ecs_vec_get_t(&sparse->pages, ecs_page_t, page_index);;
}

static
ecs_page_t* flecs_sparse_get_or_create_page(
    ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_page_t *page = flecs_sparse_get_page(sparse, page_index);
    if (page && page->sparse) {
        return page;
    }

    return flecs_sparse_page_new(sparse, page_index);
}

static
void flecs_sparse_grow_dense(
    ecs_sparse_t *sparse)
{
    ecs_vec_append_t(sparse->allocator, &sparse->dense, uint64_t);
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
    ecs_page_t * page, 
    uint64_t * dense_array, 
    uint64_t index, 
    int32_t dense)
{
    /* Initialize sparse-dense pair. This assigns the dense index to the sparse
     * array, and the sparse index to the dense array .*/
    page->sparse[OFFSET(index)] = dense;
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
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(sparse->max_id != NULL, ECS_INTERNAL_ERROR, NULL);
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

    ecs_page_t *page = flecs_sparse_get_or_create_page(sparse, PAGE(index));
    ecs_assert(page->sparse[OFFSET(index)] == 0, ECS_INTERNAL_ERROR, NULL);
    
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    flecs_sparse_assign_index(page, dense_array, index, dense);
    
    return index;
}

/* Create new id */
static
uint64_t flecs_sparse_new_index(
    ecs_sparse_t *sparse)
{
    int32_t dense_count = ecs_vec_count(&sparse->dense);
    int32_t count = sparse->count ++;

    ecs_assert(count <= dense_count, ECS_INTERNAL_ERROR, NULL);
    if (count < dense_count) {
        /* If there are unused elements in the dense array, return first */
        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
        return dense_array[count];
    } else {
        return flecs_sparse_create_id(sparse, count);
    }
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
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    (void)dense;

    return DATA(page->data, sparse->size, offset);
}

/* Swap dense elements. A swap occurs when an element is removed, or when a
 * removed element is recycled. */
static
void flecs_sparse_swap_dense(
    ecs_sparse_t * sparse,
    ecs_page_t * page_a,
    int32_t a,
    int32_t b)
{
    ecs_assert(a != b, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    uint64_t index_a = dense_array[a];
    uint64_t index_b = dense_array[b];

    ecs_page_t *page_b = flecs_sparse_get_or_create_page(sparse, PAGE(index_b));
    flecs_sparse_assign_index(page_a, dense_array, index_a, b);
    flecs_sparse_assign_index(page_b, dense_array, index_b, a);
}

void flecs_sparse_init(
    ecs_sparse_t *result,
    struct ecs_allocator_t *allocator,
    ecs_block_allocator_t *page_allocator,
    ecs_size_t size)
{
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    result->size = size;
    result->max_id_local = UINT64_MAX;
    result->max_id = &result->max_id_local;
    result->allocator = allocator;
    result->page_allocator = page_allocator;

    ecs_vec_init_t(allocator, &result->pages, ecs_page_t, 0);
    ecs_vec_init_t(allocator, &result->dense, uint64_t, 1);
    result->dense.count = 1;

    /* Consume first value in dense array as 0 is used in the sparse array to
     * indicate that a sparse element hasn't been paired yet. */
    ecs_vec_first_t(&result->dense, uint64_t)[0] = 0;

    result->count = 1;
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

    int32_t i, count = ecs_vec_count(&sparse->pages);
    ecs_page_t *pages = ecs_vec_first_t(&sparse->pages, ecs_page_t);
    for (i = 0; i < count; i ++) {
        int32_t *indices = pages[i].sparse;
        if (indices) {
            ecs_os_memset_n(indices, 0, int32_t, FLECS_SPARSE_CHUNK_SIZE);
        }
    }

    ecs_vec_set_count_t(sparse->allocator, &sparse->dense, uint64_t, 1);

    sparse->count = 1;
    sparse->max_id_local = 0;
}

void flecs_sparse_fini(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t i, count = ecs_vec_count(&sparse->pages);
    ecs_page_t *pages = ecs_vec_first_t(&sparse->pages, ecs_page_t);
    for (i = 0; i < count; i ++) {
        flecs_sparse_page_free(sparse, &pages[i]);
    }

    ecs_vec_fini_t(sparse->allocator, &sparse->pages, ecs_page_t);
    ecs_vec_fini_t(sparse->allocator, &sparse->dense, uint64_t);
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
    int32_t dense_count = ecs_vec_count(&sparse->dense);
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

    return ecs_vec_get_t(&sparse->dense, uint64_t, count);
}

void* flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t size)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    uint64_t index = flecs_sparse_new_index(sparse);
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, size, OFFSET(index));
}

uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    return dense_array[sparse->count - 1];
}

void* flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vec_count(&sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint64_t gen = flecs_sparse_strip_generation(&index);
    ecs_page_t *page = flecs_sparse_get_or_create_page(sparse, PAGE(index));
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];

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
            flecs_sparse_swap_dense(sparse, page, dense, count);

            /* First unused element is now last used element */
            sparse->count ++;
        } else {
            /* Dense is already alive, nothing to be done */
        }

        /* Ensure provided generation matches current. Only allow mismatching
         * generations if the provided generation count is 0. This allows for
         * using the ensure function in combination with ids that have their
         * generation stripped. */
#ifdef FLECS_DEBUG
        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
        ecs_assert(!gen || dense_array[dense] == (index | gen), ECS_INTERNAL_ERROR, NULL);
#endif
    } else {
        /* Element is not paired yet. Must add a new element to dense array */
        flecs_sparse_grow_dense(sparse);

        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);    
        int32_t dense_count = ecs_vec_count(&sparse->dense) - 1;
        int32_t count = sparse->count ++;

        /* If index is larger than max id, update max id */
        if (index >= flecs_sparse_get_id(sparse)) {
            flecs_sparse_set_id(sparse, index);
        }

        if (count < dense_count) {
            /* If there are unused elements in the list, move the first unused
             * element to the end of the list */
            uint64_t unused = dense_array[count];
            ecs_page_t *unused_page = flecs_sparse_get_or_create_page(sparse, PAGE(unused));
            flecs_sparse_assign_index(unused_page, dense_array, unused, dense_count);
        }

        flecs_sparse_assign_index(page, dense_array, index, count);
        dense_array[count] |= gen;
    }

    return DATA(page->data, sparse->size, offset);
}

void* flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index_long)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vec_count(&sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint32_t index = (uint32_t)index_long;
    ecs_page_t *page = flecs_sparse_get_or_create_page(sparse, PAGE(index));
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    int32_t count = sparse->count;

    if (!dense) {
        /* Element is not paired yet. Must add a new element to dense array */
        sparse->count = count + 1;
        if (count == ecs_vec_count(&sparse->dense)) {
            flecs_sparse_grow_dense(sparse);
        }

        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
        flecs_sparse_assign_index(page, dense_array, index, count);
    }

    return DATA(page->data, sparse->size, offset);
}

void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return;
    }

    uint64_t gen = flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];

    if (dense) {
        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
        uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
        if (gen != cur_gen) {
            /* Generation doesn't match which means that the provided entity is
             * already not alive. */
            return;
        }

        /* Increase generation */
        dense_array[dense] = index | flecs_sparse_inc_gen(cur_gen);
        
        int32_t count = sparse->count;
        
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            flecs_sparse_swap_dense(sparse, page, dense, count - 1);
            sparse->count --;
        } else {
            /* Element is not alive, nothing to be done */
            return;
        }

        /* Reset memory to zero on remove */
        void *ptr = DATA(page->data, sparse->size, offset);
        ecs_os_memset(ptr, 0, size);
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
        return;
    }
}

void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_page_t *page = flecs_sparse_get_or_create_page(sparse, PAGE(index));
    
    uint64_t index_w_gen = index;
    flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];

    if (dense) {
        /* Increase generation */
        ecs_vec_get_t(&sparse->dense, uint64_t, dense)[0] = index_w_gen;
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
    }
}

bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return false;
    }
    
    flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];

    return dense != 0;
}

bool flecs_sparse_is_valid(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return true; /* Doesn't exist yet, so is valid */
    }
    
    flecs_sparse_strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    if (!dense) {
        return true; /* Doesn't exist yet, so is valid */
    }

    /* If the id exists, it must be alive */
    return dense < sparse->count;
}

void* flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    int32_t dense_index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense_index < sparse->count, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    dense_index ++;

    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    return flecs_sparse_get_sparse(sparse, dense_index, dense_array[dense_index]);
}

bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return false;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    if (!dense || (dense >= sparse->count)) {
        return false;
    }

    uint64_t gen = flecs_sparse_strip_generation(&index);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;

    if (cur_gen != gen) {
        return false;
    }

    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return true;
}

uint64_t flecs_sparse_get_current(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return 0;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);

    /* If dense is 0 (tombstone) this will return 0 */
    return dense_array[dense];
}

void* flecs_sparse_try(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    if (!dense || (dense >= sparse->count)) {
        return NULL;
    }

    uint64_t gen = flecs_sparse_strip_generation(&index);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
    if (cur_gen != gen) {
        return NULL;
    }

    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, sparse->size, offset);
}

void* flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    ecs_page_t *page = ecs_vec_get_t(&sparse->pages, ecs_page_t, PAGE(index));
    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    ecs_assert(dense != 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t gen = flecs_sparse_strip_generation(&index);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
    (void)cur_gen; (void)gen;

    ecs_assert(cur_gen == gen, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dense < sparse->count, ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, sparse->size, offset);
}

void* flecs_sparse_get_any(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    flecs_sparse_strip_generation(&index);
    ecs_page_t *page = flecs_sparse_get_page(sparse, PAGE(index));
    if (!page || !page->sparse) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = page->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, sparse->size, offset);
}

int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse || !sparse->count) {
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

    return ecs_vec_count(&sparse->dense) - sparse->count;
}

int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vec_count(&sparse->dense) - 1;
}

const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    if (sparse->dense.array) {
        return &(ecs_vec_first_t(&sparse->dense, uint64_t)[1]);
    } else {
        return NULL;
    }
}

void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_vec_set_size_t(sparse->allocator, &sparse->dense, uint64_t, elem_count);
}

static
void flecs_sparse_copy_intern(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    flecs_sparse_set_size(dst, flecs_sparse_size(src));
    const uint64_t *indices = flecs_sparse_ids(src);
    
    ecs_size_t size = src->size;
    int32_t i, count = src->count;

    for (i = 0; i < count - 1; i ++) {
        uint64_t index = indices[i];
        void *src_ptr = flecs_sparse_get(src, size, index);
        void *dst_ptr = flecs_sparse_ensure(dst, size, index);
        flecs_sparse_set_generation(dst, index);
        ecs_os_memcpy(dst_ptr, src_ptr, size);
    }

    flecs_sparse_set_id(dst, flecs_sparse_get_id(src));

    ecs_assert(src->count == dst->count, ECS_INTERNAL_ERROR, NULL);
}

void flecs_sparse_copy(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src)
{
    if (!src) {
        return;
    }

    flecs_sparse_init(dst, src->allocator, src->page_allocator, src->size);
    flecs_sparse_copy_intern(dst, src);
}

void flecs_sparse_restore(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    dst->count = 1;
    if (src) {
        flecs_sparse_copy_intern(dst, src);
    }
}

void ecs_sparse_init(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size)
{
    flecs_sparse_init(sparse, NULL, NULL, elem_size);
}

void* ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size)
{
    return flecs_sparse_add(sparse, elem_size);
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

void* ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{
    return flecs_sparse_get_dense(sparse, elem_size, index);
}

void* ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id)
{
    return flecs_sparse_get(sparse, elem_size, id);
}
