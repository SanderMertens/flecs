#include "../private_api.h"

static
ecs_switch_page_t* flecs_switch_page_ensure(
    ecs_switch_t* sw,
    uint32_t elem)
{
    int32_t page_index = FLECS_SPARSE_PAGE(elem);
    ecs_vec_set_min_count_zeromem_t(
        sw->hdrs.allocator, &sw->pages, ecs_switch_page_t, page_index + 1);

    ecs_switch_page_t *page = ecs_vec_get_t(
        &sw->pages, ecs_switch_page_t, page_index);
    if (!ecs_vec_count(&page->nodes)) {
        ecs_vec_init_t(sw->hdrs.allocator, &page->nodes, ecs_switch_node_t,
            FLECS_SPARSE_PAGE_SIZE);
        ecs_vec_init_t(sw->hdrs.allocator, &page->values, uint64_t,
            FLECS_SPARSE_PAGE_SIZE);
        ecs_vec_set_min_count_zeromem_t(sw->hdrs.allocator, &page->nodes, 
            ecs_switch_node_t, FLECS_SPARSE_PAGE_SIZE);
        ecs_vec_set_min_count_zeromem_t(sw->hdrs.allocator, &page->values, 
            uint64_t, FLECS_SPARSE_PAGE_SIZE);
    }

    return page;
}

static
ecs_switch_page_t* flecs_switch_page_get(
    const ecs_switch_t* sw,
    uint32_t elem)
{
    int32_t page_index = FLECS_SPARSE_PAGE(elem);
    if (page_index >= ecs_vec_count(&sw->pages)) {
        return NULL;
    }

    ecs_switch_page_t *page = ecs_vec_get_t(
        &sw->pages, ecs_switch_page_t, page_index);
    if (!ecs_vec_count(&page->nodes)) {
        return NULL;
    }

    return page;
}

static
void flecs_switch_page_fini(
    ecs_switch_t* sw,
    ecs_switch_page_t *page)
{
    if (ecs_vec_count(&page->nodes)) {
        ecs_vec_fini_t(sw->hdrs.allocator, &page->nodes, ecs_switch_node_t);
        ecs_vec_fini_t(sw->hdrs.allocator, &page->values, uint64_t);
    }
}

static
ecs_switch_node_t* flecs_switch_get_node(
    ecs_switch_t* sw,
    uint32_t element)
{
    if (!element) {
        return NULL;
    }

    ecs_switch_page_t *page = flecs_switch_page_ensure(sw, element);
    int32_t page_offset = FLECS_SPARSE_OFFSET(element);
    return ecs_vec_get_t(&page->nodes, ecs_switch_node_t, page_offset);
}

void flecs_switch_init(
    ecs_switch_t* sw,
    ecs_allocator_t *allocator)
{
    ecs_map_init(&sw->hdrs, allocator);
    ecs_vec_init_t(allocator, &sw->pages, ecs_switch_page_t, 0);
}

void flecs_switch_fini(
    ecs_switch_t* sw)
{
    int32_t i, count = ecs_vec_count(&sw->pages);
    ecs_switch_page_t *pages = ecs_vec_first(&sw->pages);
    for (i = 0; i < count; i ++) {
        flecs_switch_page_fini(sw, &pages[i]);
    }
    ecs_vec_fini_t(sw->hdrs.allocator, &sw->pages, ecs_switch_page_t);
    ecs_map_fini(&sw->hdrs);
}

bool flecs_switch_set(
    ecs_switch_t *sw,
    uint32_t element,
    uint64_t value)
{
    ecs_switch_page_t *page = flecs_switch_page_ensure(sw, element);
    int32_t page_offset = FLECS_SPARSE_OFFSET(element);

    uint64_t *elem = ecs_vec_get_t(&page->values, uint64_t, page_offset);
    if (elem[0] == value) {
        return false;
    }

    ecs_switch_node_t *node = ecs_vec_get_t(
        &page->nodes, ecs_switch_node_t, page_offset);

    uint64_t prev_value = elem[0];
    if (prev_value) {
        ecs_switch_node_t *prev = flecs_switch_get_node(sw, node->prev);
        if (prev) {
            prev->next = node->next;
        }

        ecs_switch_node_t *next = flecs_switch_get_node(sw, node->next);
        if (next) {
            next->prev = node->prev;
        }

        if (!prev) {
            uint64_t *hdr = ecs_map_get(&sw->hdrs, prev_value);
            ecs_assert(hdr[0] == (uint64_t)element, ECS_INTERNAL_ERROR, NULL);
            hdr[0] = (uint64_t)node->next;
        }
    }

    elem[0] = value;

    if (value) {
        uint64_t *hdr = ecs_map_ensure(&sw->hdrs, value);

        if (!hdr[0]) {
            hdr[0] = (uint64_t)element;
        } else {
            ecs_switch_node_t *head = flecs_switch_get_node(sw, (uint32_t)hdr[0]);
            ecs_assert(head->prev == 0, ECS_INTERNAL_ERROR, NULL);
            head->prev = element;

            node->next = (uint32_t)hdr[0];
            hdr[0] = (uint64_t)element;
        }

        node->prev = 0;
    }

    return true;
}

bool flecs_switch_reset(
    ecs_switch_t *sw,
    uint32_t element)
{
    return flecs_switch_set(sw, element, 0);
}

uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    uint32_t element)
{
    ecs_switch_page_t *page = flecs_switch_page_get(sw, element);
    if (!page) {
        return 0;
    }

    int32_t page_offset = FLECS_SPARSE_OFFSET(element);
    uint64_t *elem = ecs_vec_get_t(&page->values, uint64_t, page_offset);
    return elem[0];
}

uint32_t flecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value)
{
    uint64_t *hdr = ecs_map_get(&sw->hdrs, value);
    if (!hdr) {
        return 0;
    }

    return (uint32_t)hdr[0];
}

FLECS_DBG_API
uint32_t flecs_switch_next(
    const ecs_switch_t *sw,
    uint32_t previous)
{
    ecs_switch_page_t *page = flecs_switch_page_get(sw, previous);
    if (!page) {
        return 0;
    }

    int32_t offset = FLECS_SPARSE_OFFSET(previous);
    ecs_switch_node_t *elem = ecs_vec_get_t(
        &page->nodes, ecs_switch_node_t, offset);
    return elem->next;
}

ecs_map_iter_t flecs_switch_targets(
    const ecs_switch_t *sw)
{
    return ecs_map_iter(&sw->hdrs);
}
