/**
 * @file datastructures/switch_list.c
 * @brief Interleaved linked list for storing mutually exclusive values.
 * 
 * Datastructure that stores N interleaved linked lists in an array. 
 * This allows for efficient storage of elements with mutually exclusive values.
 * Each linked list has a header element which points to the index in the array
 * that stores the first node of the list. Each list node points to the next
 * array element.
 *
 * The datastructure allows for efficient storage and retrieval for values with
 * mutually exclusive values, such as enumeration values. The linked list allows
 * an application to obtain all elements for a given (enumeration) value without
 * having to search.
 *
 * While the list accepts 64 bit values, it only uses the lower 32bits of the
 * value for selecting the correct linked list.
 * 
 * The switch list is used to store union relationships.
 */

#include "../private_api.h"

#ifdef FLECS_SANITIZE
static 
void flecs_switch_verify_nodes(
    ecs_switch_header_t *hdr,
    ecs_switch_node_t *nodes)
{
    if (!hdr) {
        return;
    }

    int32_t prev = -1, elem = hdr->element, count = 0;
    while (elem != -1) {
        ecs_assert(prev == nodes[elem].prev, ECS_INTERNAL_ERROR, NULL);
        prev = elem;
        elem = nodes[elem].next;
        count ++;
    }

    ecs_assert(count == hdr->count, ECS_INTERNAL_ERROR, NULL);
}
#else
#define flecs_switch_verify_nodes(hdr, nodes)
#endif

static
ecs_switch_header_t* flecs_switch_get_header(
    const ecs_switch_t *sw,
    uint64_t value)
{
    if (value == 0) {
        return NULL;
    }
    return (ecs_switch_header_t*)ecs_map_get(&sw->hdrs, value);
}

static
ecs_switch_header_t *flecs_switch_ensure_header(
    ecs_switch_t *sw,
    uint64_t value)
{
    ecs_switch_header_t *node = flecs_switch_get_header(sw, value);
    if (!node && (value != 0)) {
        node = (ecs_switch_header_t*)ecs_map_ensure(&sw->hdrs, value);
        node->count = 0;
        node->element = -1;
    }

    return node;
}

static
void flecs_switch_remove_node(
    ecs_switch_header_t *hdr,
    ecs_switch_node_t *nodes,
    ecs_switch_node_t *node,
    int32_t element)
{
    ecs_assert(&nodes[element] == node, ECS_INTERNAL_ERROR, NULL);

    /* Update previous node/header */
    if (hdr->element == element) {
        ecs_assert(node->prev == -1, ECS_INVALID_PARAMETER, NULL);
        /* If this is the first node, update the header */
        hdr->element = node->next;
    } else {
        /* If this is not the first node, update the previous node to the 
         * removed node's next ptr */
        ecs_assert(node->prev != -1, ECS_INVALID_PARAMETER, NULL);
        ecs_switch_node_t *prev_node = &nodes[node->prev];
        prev_node->next = node->next;
    }

    /* Update next node */
    int32_t next = node->next;
    if (next != -1) {
        ecs_assert(next >= 0, ECS_INVALID_PARAMETER, NULL);
        /* If this is not the last node, update the next node to point to the
         * removed node's prev ptr */
        ecs_switch_node_t *next_node = &nodes[next];
        next_node->prev = node->prev;
    }

    /* Decrease count of current header */
    hdr->count --;
    ecs_assert(hdr->count >= 0, ECS_INTERNAL_ERROR, NULL);
}

void flecs_switch_init(
    ecs_switch_t *sw,
    ecs_allocator_t *allocator,
    int32_t elements)
{
    ecs_map_init(&sw->hdrs, allocator);
    ecs_vec_init_t(allocator, &sw->nodes, ecs_switch_node_t, elements);
    ecs_vec_init_t(allocator, &sw->values, uint64_t, elements);

    ecs_switch_node_t *nodes = ecs_vec_first(&sw->nodes);
    uint64_t *values = ecs_vec_first(&sw->values);

    int i;
    for (i = 0; i < elements; i ++) {
        nodes[i].prev = -1;
        nodes[i].next = -1;
        values[i] = 0;
    }
}

void flecs_switch_clear(
    ecs_switch_t *sw)
{
    ecs_map_clear(&sw->hdrs);
    ecs_vec_fini_t(sw->hdrs.allocator, &sw->nodes, ecs_switch_node_t);
    ecs_vec_fini_t(sw->hdrs.allocator, &sw->values, uint64_t);
}

void flecs_switch_fini(
    ecs_switch_t *sw)
{
    ecs_map_fini(&sw->hdrs);
    ecs_vec_fini_t(sw->hdrs.allocator, &sw->nodes, ecs_switch_node_t);
    ecs_vec_fini_t(sw->hdrs.allocator, &sw->values, uint64_t);
}

void flecs_switch_add(
    ecs_switch_t *sw)
{
    ecs_switch_node_t *node = ecs_vec_append_t(sw->hdrs.allocator, 
        &sw->nodes, ecs_switch_node_t);
    uint64_t *value = ecs_vec_append_t(sw->hdrs.allocator, 
        &sw->values, uint64_t);
    node->prev = -1;
    node->next = -1;
    *value = 0;
}

void flecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vec_count(&sw->nodes);
    if (old_count == count) {
        return;
    }

    ecs_vec_set_count_t(sw->hdrs.allocator, &sw->nodes, ecs_switch_node_t, count);
    ecs_vec_set_count_t(sw->hdrs.allocator, &sw->values, uint64_t, count);

    ecs_switch_node_t *nodes = ecs_vec_first(&sw->nodes);
    uint64_t *values = ecs_vec_first(&sw->values);

    int32_t i;
    for (i = old_count; i < count; i ++) {
        ecs_switch_node_t *node = &nodes[i];
        node->prev = -1;
        node->next = -1;
        values[i] = 0;
    }
}

int32_t flecs_switch_count(
    ecs_switch_t *sw)
{
    ecs_assert(ecs_vec_count(&sw->values) == ecs_vec_count(&sw->nodes),
        ECS_INTERNAL_ERROR, NULL);
    return ecs_vec_count(&sw->values);
}

void flecs_switch_ensure(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vec_count(&sw->nodes);
    if (old_count >= count) {
        return;
    }

    flecs_switch_set_count(sw, count);
}

void flecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vec_count(&sw->nodes);
    flecs_switch_set_count(sw, old_count + count);
}

void flecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    uint64_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vec_count(&sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vec_count(&sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vec_first(&sw->values);
    uint64_t cur_value = values[element];

    /* If the node is already assigned to the value, nothing to be done */
    if (cur_value == value) {
        return;
    }

    ecs_switch_node_t *nodes = ecs_vec_first(&sw->nodes);
    ecs_switch_node_t *node = &nodes[element];

    ecs_switch_header_t *dst_hdr = flecs_switch_ensure_header(sw, value);
    ecs_switch_header_t *cur_hdr = flecs_switch_get_header(sw, cur_value);

    flecs_switch_verify_nodes(cur_hdr, nodes);
    flecs_switch_verify_nodes(dst_hdr, nodes);

    /* If value is not 0, and dst_hdr is NULL, then this is not a valid value
     * for this switch */
    ecs_assert(dst_hdr != NULL || !value, ECS_INVALID_PARAMETER, NULL);

    if (cur_hdr) {
        flecs_switch_remove_node(cur_hdr, nodes, node, element);
    }

    /* Now update the node itself by adding it as the first node of dst */
    node->prev = -1;
    values[element] = value;

    if (dst_hdr) {
        node->next = dst_hdr->element;

        /* Also update the dst header */
        int32_t first = dst_hdr->element;
        if (first != -1) {
            ecs_assert(first >= 0, ECS_INTERNAL_ERROR, NULL);
            ecs_switch_node_t *first_node = &nodes[first];
            first_node->prev = element;
        }

        dst_hdr->element = element;
        dst_hdr->count ++;        
    }
}

void flecs_switch_remove(
    ecs_switch_t *sw,
    int32_t elem)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem < ecs_vec_count(&sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vec_first(&sw->values);
    uint64_t value = values[elem];
    ecs_switch_node_t *nodes = ecs_vec_first(&sw->nodes);
    ecs_switch_node_t *node = &nodes[elem];

    /* If node is currently assigned to a case, remove it from the list */
    if (value != 0) {
        ecs_switch_header_t *hdr = flecs_switch_get_header(sw, value);
        ecs_assert(hdr != NULL, ECS_INTERNAL_ERROR, NULL);

        flecs_switch_verify_nodes(hdr, nodes);
        flecs_switch_remove_node(hdr, nodes, node, elem);
    }

    int32_t last_elem = ecs_vec_count(&sw->nodes) - 1;
    if (last_elem != elem) {
        ecs_switch_node_t *last = ecs_vec_last_t(&sw->nodes, ecs_switch_node_t);
        int32_t next = last->next, prev = last->prev;
        if (next != -1) {
            ecs_switch_node_t *n = &nodes[next];
            n->prev = elem;
        }

        if (prev != -1) {
            ecs_switch_node_t *n = &nodes[prev];
            n->next = elem;
        } else {
            ecs_switch_header_t *hdr = flecs_switch_get_header(sw, values[last_elem]);
            if (hdr && hdr->element != -1) {
                ecs_assert(hdr->element == last_elem, 
                    ECS_INTERNAL_ERROR, NULL);
                hdr->element = elem;
            }
        }
    }

    /* Remove element from arrays */
    ecs_vec_remove_t(&sw->nodes, ecs_switch_node_t, elem);
    ecs_vec_remove_t(&sw->values, uint64_t, elem);
}

uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vec_count(&sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vec_count(&sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vec_first(&sw->values);
    return values[element];
}

ecs_vec_t* flecs_switch_values(
    const ecs_switch_t *sw)
{
    return (ecs_vec_t*)&sw->values;
}

int32_t flecs_switch_case_count(
    const ecs_switch_t *sw,
    uint64_t value)
{
    ecs_switch_header_t *hdr = flecs_switch_get_header(sw, value);
    if (!hdr) {
        return 0;
    }

    return hdr->count;
}

void flecs_switch_swap(
    ecs_switch_t *sw,
    int32_t elem_1,
    int32_t elem_2)
{
    uint64_t v1 = flecs_switch_get(sw, elem_1);
    uint64_t v2 = flecs_switch_get(sw, elem_2);

    flecs_switch_set(sw, elem_2, v1);
    flecs_switch_set(sw, elem_1, v2);
}

int32_t flecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_switch_header_t *hdr = flecs_switch_get_header(sw, value);
    if (!hdr) {
        return -1;
    }

    return hdr->element;
}

int32_t flecs_switch_next(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vec_count(&sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    ecs_switch_node_t *nodes = ecs_vec_first(&sw->nodes);

    return nodes[element].next;
}
