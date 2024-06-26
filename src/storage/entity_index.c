#include "../private_api.h"

static
ecs_entity_index_page_t* flecs_entity_index_ensure_page(
    ecs_entity_index_t *index,
    uint32_t id)
{
    int32_t page_index = (int32_t)(id >> FLECS_ENTITY_PAGE_BITS);
    if (page_index >= ecs_vec_count(&index->pages)) {
        ecs_vec_set_min_count_zeromem_t(index->allocator, &index->pages, 
            ecs_entity_index_page_t*, page_index + 1);
    }

    ecs_entity_index_page_t **page_ptr = ecs_vec_get_t(&index->pages, 
        ecs_entity_index_page_t*, page_index);
    ecs_entity_index_page_t *page = *page_ptr;
    if (!page) {
        page = *page_ptr = flecs_bcalloc(&index->page_allocator);
        ecs_assert(page != NULL, ECS_OUT_OF_MEMORY, NULL);
    }

    return page;
}

void flecs_entity_index_init(
    ecs_allocator_t *allocator,
    ecs_entity_index_t *index)
{
    index->allocator = allocator;
    index->alive_count = 1;
    ecs_vec_init_t(allocator, &index->dense, uint64_t, 1);
    ecs_vec_set_count_t(allocator, &index->dense, uint64_t, 1);
    ecs_vec_init_t(allocator, &index->pages, ecs_entity_index_page_t*, 0);
    flecs_ballocator_init(&index->page_allocator, 
        ECS_SIZEOF(ecs_entity_index_page_t));
}

void flecs_entity_index_fini(
    ecs_entity_index_t *index)
{
    ecs_vec_fini_t(index->allocator, &index->dense, uint64_t);
#if defined(FLECS_SANITIZE) || defined(FLECS_USE_OS_ALLOC)
    int32_t i, count = ecs_vec_count(&index->pages);
    ecs_entity_index_page_t **pages = ecs_vec_first(&index->pages);
    for (i = 0; i < count; i ++) {
        flecs_bfree(&index->page_allocator, pages[i]);
    }
#endif
    ecs_vec_fini_t(index->allocator, &index->pages, ecs_entity_index_page_t*);
    flecs_ballocator_fini(&index->page_allocator);
}

ecs_record_t* flecs_entity_index_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    uint32_t id = (uint32_t)entity;
    int32_t page_index = (int32_t)(id >> FLECS_ENTITY_PAGE_BITS);
    ecs_entity_index_page_t *page = ecs_vec_get_t(&index->pages, 
        ecs_entity_index_page_t*, page_index)[0];
    ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];
    ecs_assert(r->dense != 0, ECS_INVALID_PARAMETER, 
        "entity %u does not exist", (uint32_t)entity);
    return r;
}

ecs_record_t* flecs_entity_index_get(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    ecs_record_t *r = flecs_entity_index_get_any(index, entity);
    ecs_assert(r->dense < index->alive_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vec_get_t(&index->dense, uint64_t, r->dense)[0] == entity, 
        ECS_INVALID_PARAMETER, "mismatching liveliness generation for entity");
    return r;
}

ecs_record_t* flecs_entity_index_try_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    uint32_t id = (uint32_t)entity;
    int32_t page_index = (int32_t)(id >> FLECS_ENTITY_PAGE_BITS);
    if (page_index >= ecs_vec_count(&index->pages)) {
        return NULL;
    }

    ecs_entity_index_page_t *page = ecs_vec_get_t(&index->pages, 
        ecs_entity_index_page_t*, page_index)[0];
    if (!page) {
        return NULL;
    }

    ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];
    if (!r->dense) {
        return NULL;
    }

    return r;
}

ecs_record_t* flecs_entity_index_try_get(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    ecs_record_t *r = flecs_entity_index_try_get_any(index, entity);
    if (r) {
        if (r->dense >= index->alive_count) {
            return NULL;
        }
        if (ecs_vec_get_t(&index->dense, uint64_t, r->dense)[0] != entity) {
            return NULL;
        }
    }
    return r;
}

ecs_record_t* flecs_entity_index_ensure(
    ecs_entity_index_t *index,
    uint64_t entity)
{
    uint32_t id = (uint32_t)entity;
    ecs_entity_index_page_t *page = flecs_entity_index_ensure_page(index, id);
    ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];

    int32_t dense = r->dense;
    if (dense) {
        /* Entity is already alive, nothing to be done */
        if (dense < index->alive_count) {
            ecs_assert(
                ecs_vec_get_t(&index->dense, uint64_t, dense)[0] == entity, 
                ECS_INTERNAL_ERROR, NULL);
            return r;
        }
    } else {
        /* Entity doesn't have a dense index yet */
        ecs_vec_append_t(index->allocator, &index->dense, uint64_t)[0] = entity;
        r->dense = dense = ecs_vec_count(&index->dense) - 1;
        index->max_id = id > index->max_id ? id : index->max_id;
    }

    ecs_assert(dense != 0, ECS_INTERNAL_ERROR, NULL);

    /* Entity is not alive, swap with first not alive element */
    uint64_t *ids = ecs_vec_first(&index->dense);
    uint64_t e_swap = ids[index->alive_count];
    ecs_record_t *r_swap = flecs_entity_index_get_any(index, e_swap);
    ecs_assert(r_swap->dense == index->alive_count, 
        ECS_INTERNAL_ERROR, NULL);

    r_swap->dense = dense;
    r->dense = index->alive_count;
    ids[dense] = e_swap;
    ids[index->alive_count ++] = entity;

    ecs_assert(flecs_entity_index_is_alive(index, entity), 
        ECS_INTERNAL_ERROR, NULL);

    return r;
}

void flecs_entity_index_remove(
    ecs_entity_index_t *index,
    uint64_t entity)
{
    ecs_record_t *r = flecs_entity_index_try_get(index, entity);
    if (!r) {
        /* Entity is not alive or doesn't exist, nothing to be done */
        return;
    }

    int32_t dense = r->dense;
    int32_t i_swap = -- index->alive_count;
    uint64_t *e_swap_ptr = ecs_vec_get_t(&index->dense, uint64_t, i_swap);
    uint64_t e_swap = e_swap_ptr[0];
    ecs_record_t *r_swap = flecs_entity_index_get_any(index, e_swap);
    ecs_assert(r_swap->dense == i_swap, ECS_INTERNAL_ERROR, NULL);

    r_swap->dense = dense;
    r->table = NULL;
    r->idr = NULL;
    r->row = 0;
    r->dense = i_swap;
    ecs_vec_get_t(&index->dense, uint64_t, dense)[0] = e_swap;
    e_swap_ptr[0] = ECS_GENERATION_INC(entity);
    ecs_assert(!flecs_entity_index_is_alive(index, entity), 
        ECS_INTERNAL_ERROR, NULL);
}

void flecs_entity_index_make_alive(
    ecs_entity_index_t *index,
    uint64_t entity)
{
    ecs_record_t *r = flecs_entity_index_try_get_any(index, entity);
    if (r) {
        ecs_vec_get_t(&index->dense, uint64_t, r->dense)[0] = entity;
    }
}

uint64_t flecs_entity_index_get_alive(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    ecs_record_t *r = flecs_entity_index_try_get_any(index, entity);
    if (r) {
        return ecs_vec_get_t(&index->dense, uint64_t, r->dense)[0];
    } else {
        return 0;
    }
}

bool flecs_entity_index_is_alive(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    return flecs_entity_index_try_get(index, entity) != NULL;
}

bool flecs_entity_index_is_valid(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    uint32_t id = (uint32_t)entity;    
    ecs_record_t *r = flecs_entity_index_try_get_any(index, id);
    if (!r || !r->dense) {
        /* Doesn't exist yet, so is valid */
        return true;
    }

    /* If the id exists, it must be alive */
    return r->dense < index->alive_count;
}

bool flecs_entity_index_exists(
    const ecs_entity_index_t *index,
    uint64_t entity)
{
    return flecs_entity_index_try_get_any(index, entity) != NULL;
}

uint64_t flecs_entity_index_new_id(
    ecs_entity_index_t *index)
{
    if (index->alive_count != ecs_vec_count(&index->dense)) {
        /* Recycle id */
        return ecs_vec_get_t(&index->dense, uint64_t, index->alive_count ++)[0];
    }

    /* Create new id */
    uint32_t id = (uint32_t)++ index->max_id;
    ecs_vec_append_t(index->allocator, &index->dense, uint64_t)[0] = id;

    ecs_entity_index_page_t *page = flecs_entity_index_ensure_page(index, id);
    ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];
    r->dense = index->alive_count ++;
    ecs_assert(index->alive_count == ecs_vec_count(&index->dense), 
        ECS_INTERNAL_ERROR, NULL);

    return id;
}

uint64_t* flecs_entity_index_new_ids(
    ecs_entity_index_t *index,
    int32_t count)
{
    int32_t alive_count = index->alive_count;
    int32_t new_count = alive_count + count;
    int32_t dense_count = ecs_vec_count(&index->dense);

    if (new_count < dense_count) {
        /* Recycle ids */
        index->alive_count = new_count;
        return ecs_vec_get_t(&index->dense, uint64_t, alive_count);
    }

    /* Allocate new ids */
    ecs_vec_set_count_t(index->allocator, &index->dense, uint64_t, new_count);
    int32_t i, to_add = new_count - dense_count;
    for (i = 0; i < to_add; i ++) {
        uint32_t id = (uint32_t)++ index->max_id;
        int32_t dense = dense_count + i;
        ecs_vec_get_t(&index->dense, uint64_t, dense)[0] = id;
        ecs_entity_index_page_t *page = flecs_entity_index_ensure_page(index, id);
        ecs_assert(page != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];
        r->dense = dense;
    }

    index->alive_count = new_count;
    return ecs_vec_get_t(&index->dense, uint64_t, alive_count);
}

void flecs_entity_index_set_size(
    ecs_entity_index_t *index,
    int32_t size)
{
    ecs_vec_set_size_t(index->allocator, &index->dense, uint64_t, size);
}

int32_t flecs_entity_index_count(
    const ecs_entity_index_t *index)
{
    return index->alive_count - 1;
}

int32_t flecs_entity_index_size(
    const ecs_entity_index_t *index)
{
    return ecs_vec_count(&index->dense) - 1;
}

int32_t flecs_entity_index_not_alive_count(
    const ecs_entity_index_t *index)
{
    return ecs_vec_count(&index->dense) - index->alive_count;
}

void flecs_entity_index_clear(
    ecs_entity_index_t *index)
{
    int32_t i, count = ecs_vec_count(&index->pages);
    ecs_entity_index_page_t **pages = ecs_vec_first_t(&index->pages, 
        ecs_entity_index_page_t*);
    for (i = 0; i < count; i ++) {
        ecs_entity_index_page_t *page = pages[i];
        if (page) {
            ecs_os_zeromem(page);
        }
    }

    ecs_vec_set_count_t(index->allocator, &index->dense, uint64_t, 1);

    index->alive_count = 1;
    index->max_id = 0;
}

const uint64_t* flecs_entity_index_ids(
    const ecs_entity_index_t *index)
{
    return ecs_vec_get_t(&index->dense, uint64_t, 1);
}
