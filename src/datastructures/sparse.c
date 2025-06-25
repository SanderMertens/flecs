/**
 * @file datastructures/sparse.c
 * @brief Sparse set data structure.
 */

#include "../private_api.h"

/* Utility to get a pointer to the payload */
#define DATA(array, size, offset) (ECS_OFFSET(array, size * offset))

typedef struct ecs_sparse_page_t {
    int32_t *sparse;            /* Sparse array with indices to dense array */
    void *data;                 /* Store data in sparse array to reduce  
                                 * indirection and provide stable pointers. */
} ecs_sparse_page_t;

static
ecs_sparse_page_t* flecs_sparse_page_new(
    ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_allocator_t *a = sparse->allocator;
    ecs_block_allocator_t *ca = sparse->page_allocator;
    int32_t count = ecs_vec_count(&sparse->pages);
    ecs_sparse_page_t *pages;

    if (count <= page_index) {
        ecs_vec_set_count_t(a, &sparse->pages, ecs_sparse_page_t, page_index + 1);
        pages = ecs_vec_first_t(&sparse->pages, ecs_sparse_page_t);
        ecs_os_memset_n(&pages[count], 0, ecs_sparse_page_t, (1 + page_index - count));
    } else {
        pages = ecs_vec_first_t(&sparse->pages, ecs_sparse_page_t);
    }

    ecs_assert(pages != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_sparse_page_t *result = &pages[page_index];
    ecs_assert(result->sparse == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data == NULL, ECS_INTERNAL_ERROR, NULL);

    /* Initialize sparse array with zero's, as zero is used to indicate that the
     * sparse element has not been paired with a dense element. Use zero
     * as this means we can take advantage of calloc having a possibly better 
     * performance than malloc + memset. */
    result->sparse = ca ? flecs_bcalloc(ca)
                        : ecs_os_calloc_n(int32_t, FLECS_SPARSE_PAGE_SIZE);

    /* Initialize the data array with zero's to guarantee that data is 
     * always initialized. When an entry is removed, data is reset back to
     * zero. Initialize now, as this can take advantage of calloc. */
    if (sparse->size) {
        result->data = a ? flecs_calloc(a, sparse->size * FLECS_SPARSE_PAGE_SIZE)
                        : ecs_os_calloc(sparse->size * FLECS_SPARSE_PAGE_SIZE);
        ecs_assert(result->data != NULL, ECS_INTERNAL_ERROR, NULL);
    } else {
        result->data = NULL;
    }

    ecs_assert(result->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

static
void flecs_sparse_page_free(
    ecs_sparse_t *sparse,
    ecs_sparse_page_t *page)
{
    ecs_allocator_t *a = sparse->allocator;
    ecs_block_allocator_t *ca = sparse->page_allocator;

    if (ca) {
        flecs_bfree(ca, page->sparse);
    } else {
        ecs_os_free(page->sparse);
    }
    if (a) {
        flecs_free(a, sparse->size * FLECS_SPARSE_PAGE_SIZE, page->data);
    } else {
        ecs_os_free(page->data);
    }

    page->sparse = NULL;
    page->data = NULL;
}

static
ecs_sparse_page_t* flecs_sparse_get_page(
    const ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_assert(page_index >= 0, ECS_INVALID_PARAMETER, NULL);
    if (page_index >= ecs_vec_count(&sparse->pages)) {
        return NULL;
    }
    return ecs_vec_get_t(&sparse->pages, ecs_sparse_page_t, page_index);
}

static
ecs_sparse_page_t* flecs_sparse_get_or_create_page(
    ecs_sparse_t *sparse,
    int32_t page_index)
{
    ecs_sparse_page_t *page = flecs_sparse_get_page(sparse, page_index);
    if (page && page->sparse) {
        ecs_assert(!sparse->size || page->data != NULL, 
            ECS_INTERNAL_ERROR, NULL);
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
void flecs_sparse_assign_index(
    ecs_sparse_page_t * page, 
    uint64_t * dense_array, 
    uint64_t id, 
    int32_t dense)
{
    /* Initialize sparse-dense pair. This assigns the dense index to the sparse
     * array, and the sparse index to the dense array .*/
    page->sparse[FLECS_SPARSE_OFFSET(id)] = dense;
    dense_array[dense] = id;
}

static
uint64_t flecs_sparse_inc_id(
    ecs_sparse_t *sparse)
{
    /* Generate a new id. The last issued id could be stored in an external
     * variable, such as is the case with the last issued entity id, which is
     * stored on the world. */
    return ++ sparse->max_id;
}

static
uint64_t flecs_sparse_get_id(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    return sparse->max_id;
}

static
void flecs_sparse_set_id(
    ecs_sparse_t *sparse,
    uint64_t value)
{
    /* Sometimes the max id needs to be assigned directly, which typically 
     * happens when the API calls get_or_create for an id that hasn't been 
     * issued before. */
    sparse->max_id = value;
}

/* Pair dense id with new sparse id */
static
uint64_t flecs_sparse_create_id(
    ecs_sparse_t *sparse,
    int32_t dense)
{
    uint64_t id = flecs_sparse_inc_id(sparse);
    flecs_sparse_grow_dense(sparse);

    ecs_sparse_page_t *page = flecs_sparse_get_or_create_page(
        sparse, FLECS_SPARSE_PAGE(id));
    ecs_assert(page->sparse[FLECS_SPARSE_OFFSET(id)] == 0, 
        ECS_INTERNAL_ERROR, NULL);
    
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    flecs_sparse_assign_index(page, dense_array, id, dense);
    
    return id;
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
    uint64_t id)
{
    uint64_t index = (uint32_t)id;
    ecs_sparse_page_t *page = flecs_sparse_get_page(
        sparse, FLECS_SPARSE_PAGE(index));
    if (!page || !page->sparse) {
        return NULL;
    }

    int32_t offset = FLECS_SPARSE_OFFSET(index);
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
    ecs_sparse_page_t * page_a,
    int32_t a,
    int32_t b)
{
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    uint64_t id_a = dense_array[a];
    uint64_t id_b = dense_array[b];

    ecs_sparse_page_t *page_b = flecs_sparse_get_or_create_page(
            sparse, FLECS_SPARSE_PAGE(id_b));
    flecs_sparse_assign_index(page_a, dense_array, id_a, b);
    flecs_sparse_assign_index(page_b, dense_array, id_b, a);
}

void flecs_sparse_init(
    ecs_sparse_t *result,
    struct ecs_allocator_t *allocator,
    ecs_block_allocator_t *page_allocator,
    ecs_size_t size)
{
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    result->size = size;
    result->max_id = UINT64_MAX;
    result->allocator = allocator;
    result->page_allocator = page_allocator;

    ecs_vec_init_t(allocator, &result->pages, ecs_sparse_page_t, 0);
    ecs_vec_init_t(allocator, &result->dense, uint64_t, 1);
    result->dense.count = 1;

    /* Consume first value in dense array as 0 is used in the sparse array to
     * indicate that a sparse element hasn't been paired yet. */
    ecs_vec_first_t(&result->dense, uint64_t)[0] = 0;

    result->count = 1;
}

void flecs_sparse_clear(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t i, count = ecs_vec_count(&sparse->pages);
    ecs_sparse_page_t *pages = ecs_vec_first_t(&sparse->pages, ecs_sparse_page_t);
    for (i = 0; i < count; i ++) {
        int32_t *indices = pages[i].sparse;
        if (indices) {
            ecs_os_memset_n(indices, 0, int32_t, FLECS_SPARSE_PAGE_SIZE);
        }
    }

    ecs_vec_set_count_t(sparse->allocator, &sparse->dense, uint64_t, 1);

    sparse->count = 1;
    sparse->max_id = 0;
}

void flecs_sparse_fini(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t i, count = ecs_vec_count(&sparse->pages);
    ecs_sparse_page_t *pages = ecs_vec_first_t(&sparse->pages, ecs_sparse_page_t);
    for (i = 0; i < count; i ++) {
        flecs_sparse_page_free(sparse, &pages[i]);
    }

    ecs_vec_fini_t(sparse->allocator, &sparse->pages, ecs_sparse_page_t);
    ecs_vec_fini_t(sparse->allocator, &sparse->dense, uint64_t);
}

uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_sparse_new_index(sparse);
}

void* flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t size)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    uint64_t id = flecs_sparse_new_index(sparse);
    ecs_sparse_page_t *page = flecs_sparse_get_page(sparse, FLECS_SPARSE_PAGE(id));
    ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(page->data != NULL, ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, size, FLECS_SPARSE_OFFSET(id));
}

uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
    return dense_array[sparse->count - 1];
}

void* flecs_sparse_insert(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id)
{
    bool is_new = true;
    void *result = flecs_sparse_ensure(sparse, size, id, &is_new);
    if (!is_new) {
        result = NULL;
    }
    return result;
}

void* flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id,
    bool *is_new)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vec_count(&sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint64_t index = (uint32_t)id;
    ecs_sparse_page_t *page = flecs_sparse_get_or_create_page(
        sparse, FLECS_SPARSE_PAGE(index));
    int32_t offset = FLECS_SPARSE_OFFSET(index);
    int32_t dense = page->sparse[offset];

    if (dense) {
        /* Check if element is alive. If element is not alive, update indices so
         * that the first unused dense element points to the sparse element. */
        int32_t count = sparse->count;
        if (dense >= count) {
            /* If dense is not alive, swap it with the first unused element. */
            flecs_sparse_swap_dense(sparse, page, dense, count);
            dense = count;

            /* First unused element is now last used element */
            sparse->count ++;

            /* Set dense element to new generation */
            ecs_vec_first_t(&sparse->dense, uint64_t)[dense] = id;
        } else {
            if (is_new) *is_new = false;
        }
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
            ecs_sparse_page_t *unused_page = flecs_sparse_get_or_create_page(
                sparse, FLECS_SPARSE_PAGE(unused));
            flecs_sparse_assign_index(
                unused_page, dense_array, unused, dense_count);
        }

        flecs_sparse_assign_index(page, dense_array, index, count);
    }

    return DATA(page->data, sparse->size, offset);
}

void* flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vec_count(&sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint32_t index = (uint32_t)id;
    ecs_sparse_page_t *page = flecs_sparse_get_or_create_page(
        sparse, FLECS_SPARSE_PAGE(index));
    int32_t offset = FLECS_SPARSE_OFFSET(index);
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

bool flecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    ecs_sparse_page_t *page = flecs_sparse_get_page(
        sparse, FLECS_SPARSE_PAGE(id));
    if (!page || !page->sparse) {
        return false;
    }

    uint64_t index = (uint32_t)id;
    int32_t offset = FLECS_SPARSE_OFFSET(index);
    int32_t dense = page->sparse[offset];

    if (dense) {
        int32_t count = sparse->count;
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            flecs_sparse_swap_dense(sparse, page, dense, count - 1);
            sparse->count --;
        }

        /* Reset memory to zero on remove */
        if (sparse->size) {
            void *ptr = DATA(page->data, sparse->size, offset);
            ecs_os_memset(ptr, 0, size);
        }

        /* Reset memory to zero on remove */
        return true;
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
        return false;
    }
}

static
uint64_t flecs_sparse_inc_gen(
    uint64_t index)
{
    /* When an index is deleted, its generation is increased so that we can do
     * liveliness checking while recycling ids */
    return ECS_GENERATION_INC(index);
}

bool flecs_sparse_remove_w_gen(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    ecs_sparse_page_t *page = flecs_sparse_get_page(
        sparse, FLECS_SPARSE_PAGE(id));
    if (!page || !page->sparse) {
        return false;
    }

    uint64_t index = (uint32_t)id;
    int32_t offset = FLECS_SPARSE_OFFSET(index);
    int32_t dense = page->sparse[offset];

    if (dense) {
        /* Increase generation */
        uint64_t *dense_array = ecs_vec_first_t(&sparse->dense, uint64_t);
        ecs_assert(dense_array[dense] == id, ECS_INVALID_PARAMETER, NULL);
        dense_array[dense] = flecs_sparse_inc_gen(id);

        int32_t count = sparse->count;
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            flecs_sparse_swap_dense(sparse, page, dense, count - 1);
            sparse->count --;
        } else {
            return false;
        }

        /* Reset memory to zero on remove */
        if (sparse->size) {
            void *ptr = DATA(page->data, sparse->size, offset);
            ecs_os_memset(ptr, 0, size);
        }

        return true;
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
        return false;
    }
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
    return flecs_sparse_get_sparse(
        sparse, dense_index, dense_array[dense_index]);
}

bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id)
{
    ecs_sparse_page_t *page = flecs_sparse_get_page(
        sparse, FLECS_SPARSE_PAGE(id));
    if (!page || !page->sparse) {
        return false;
    }

    int32_t offset = FLECS_SPARSE_OFFSET(id);
    int32_t dense = page->sparse[offset];
    if (!dense || (dense >= sparse->count)) {
        return false;
    }

    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return true;
}

void* flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    uint64_t index = (uint32_t)id;
    ecs_sparse_page_t *page = flecs_sparse_get_page(
        sparse, FLECS_SPARSE_PAGE(index));
    if (!page || !page->sparse) {
        return NULL;
    }

    int32_t offset = FLECS_SPARSE_OFFSET(index);
    int32_t dense = page->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    ecs_assert(dense == page->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(page->data, sparse->size, offset);
}

bool flecs_sparse_has(
    const ecs_sparse_t *sparse,
    uint64_t id)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    
    uint64_t index = (uint32_t)id;
    ecs_sparse_page_t *page = flecs_sparse_get_page(sparse, 
        FLECS_SPARSE_PAGE(index));
    if (!page || !page->sparse) {
        return false;
    }

    int32_t offset = FLECS_SPARSE_OFFSET(id);
    int32_t dense = page->sparse[offset];
    return dense && (dense < sparse->count);
}

int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse || !sparse->count) {
        return 0;
    }

    return sparse->count - 1;
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

void flecs_sparse_shrink(
    ecs_sparse_t *sparse)
{
    int32_t i, e, max_page_index = -1, count = ecs_vec_count(&sparse->pages);
    ecs_sparse_page_t *pages = ecs_vec_first_t(&sparse->pages,
        ecs_sparse_page_t);
    for (i = 0; i < count; i ++) {
        ecs_sparse_page_t *page = &pages[i];
        if (!page->sparse) {
            ecs_assert(page->data == NULL, ECS_INTERNAL_ERROR, NULL);
            continue;
        }

        bool has_alive = false;
        for (e = 0; e < FLECS_SPARSE_PAGE_SIZE; e ++) {
            uint64_t id = 
                flecs_ito(uint64_t, (i * FLECS_SPARSE_PAGE_SIZE) + e);

            if (flecs_sparse_is_alive(sparse, id)) {
                has_alive = true;
                break;
            }
        }

        if (!has_alive) {
            flecs_sparse_page_free(sparse, page);
        } else {
            max_page_index = i;
        }
    }

    ecs_vec_set_count_t(
        sparse->allocator, &sparse->pages, ecs_sparse_page_t, 
        max_page_index + 1);
    ecs_vec_reclaim_t(sparse->allocator, &sparse->pages, ecs_sparse_page_t);
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
