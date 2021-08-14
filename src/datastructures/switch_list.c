#include "../private_api.h"

#ifdef FLECS_SANITIZE
static 
void verify_nodes(
    flecs_switch_header_t *hdr,
    flecs_switch_node_t *nodes)
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
#define verify_nodes(hdr, nodes)
#endif

static
flecs_switch_header_t *get_header(
    const ecs_switch_t *sw,
    uint64_t value)
{
    if (value == 0) {
        return NULL;
    }

    value = (uint32_t)value;

    ecs_assert(value >= sw->min, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value <= sw->max, ECS_INTERNAL_ERROR, NULL);

    uint64_t index = value - sw->min;

    return &sw->headers[index];
}

static
void remove_node(
    flecs_switch_header_t *hdr,
    flecs_switch_node_t *nodes,
    flecs_switch_node_t *node,
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
        flecs_switch_node_t *prev_node = &nodes[node->prev];
        prev_node->next = node->next;
    }

    /* Update next node */
    int32_t next = node->next;
    if (next != -1) {
        ecs_assert(next >= 0, ECS_INVALID_PARAMETER, NULL);
        /* If this is not the last node, update the next node to point to the
         * removed node's prev ptr */
        flecs_switch_node_t *next_node = &nodes[next];
        next_node->prev = node->prev;
    }

    /* Decrease count of current header */
    hdr->count --;
    ecs_assert(hdr->count >= 0, ECS_INTERNAL_ERROR, NULL);
}

ecs_switch_t* flecs_switch_new(
    uint64_t min, 
    uint64_t max,
    int32_t elements)
{
    ecs_assert(min <= max, ECS_INVALID_PARAMETER, NULL);

    /* Min must be larger than 0, as 0 is an invalid entity id, and should
     * therefore never occur as case id */
    ecs_assert(min > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_switch_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_switch_t));
    result->min = (uint32_t)min;
    result->max = (uint32_t)max;

    int32_t count = (int32_t)(max - min) + 1;
    result->headers = ecs_os_calloc(ECS_SIZEOF(flecs_switch_header_t) * count);
    result->nodes = ecs_vector_new(flecs_switch_node_t, elements);
    result->values = ecs_vector_new(uint64_t, elements);

    int64_t i;
    for (i = 0; i < count; i ++) {
        result->headers[i].element = -1;
        result->headers[i].count = 0;
    }

    flecs_switch_node_t *nodes = ecs_vector_first(
        result->nodes, flecs_switch_node_t);
    uint64_t *values = ecs_vector_first(
        result->values, uint64_t);        

    for (i = 0; i < elements; i ++) {
        nodes[i].prev = -1;
        nodes[i].next = -1;
        values[i] = 0;
    }

    return result;
}

void flecs_switch_free(
    ecs_switch_t *sw)
{
    ecs_os_free(sw->headers);
    ecs_vector_free(sw->nodes);
    ecs_vector_free(sw->values);
    ecs_os_free(sw);
}

void flecs_switch_add(
    ecs_switch_t *sw)
{
    flecs_switch_node_t *node = ecs_vector_add(&sw->nodes, flecs_switch_node_t);
    uint64_t *value = ecs_vector_add(&sw->values, uint64_t);
    node->prev = -1;
    node->next = -1;
    *value = 0;
}

void flecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    if (old_count == count) {
        return;
    }

    ecs_vector_set_count(&sw->nodes, flecs_switch_node_t, count);
    ecs_vector_set_count(&sw->values, uint64_t, count);

    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    uint64_t *values = ecs_vector_first(sw->values, uint64_t);

    int32_t i;
    for (i = old_count; i < count; i ++) {
        flecs_switch_node_t *node = &nodes[i];
        node->prev = -1;
        node->next = -1;
        values[i] = 0;
    }
}

void flecs_switch_ensure(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    if (old_count >= count) {
        return;
    }

    flecs_switch_set_count(sw, count);
}

void flecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    flecs_switch_set_count(sw, old_count + count);
}

void flecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    uint64_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    uint64_t cur_value = values[element];

    /* If the node is already assigned to the value, nothing to be done */
    if (cur_value == value) {
        return;
    }

    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    flecs_switch_node_t *node = &nodes[element];

    flecs_switch_header_t *cur_hdr = get_header(sw, cur_value);
    flecs_switch_header_t *dst_hdr = get_header(sw, value);

    verify_nodes(cur_hdr, nodes);
    verify_nodes(dst_hdr, nodes);

    /* If value is not 0, and dst_hdr is NULL, then this is not a valid value
     * for this switch */
    ecs_assert(dst_hdr != NULL || !value, ECS_INVALID_PARAMETER, NULL);

    if (cur_hdr) {
        remove_node(cur_hdr, nodes, node, element);
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
            flecs_switch_node_t *first_node = &nodes[first];
            first_node->prev = element;
        }

        dst_hdr->element = element;
        dst_hdr->count ++;        
    }
}

void flecs_switch_remove(
    ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    uint64_t value = values[element];
    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    flecs_switch_node_t *node = &nodes[element];

    /* If node is currently assigned to a case, remove it from the list */
    if (value != 0) {
        flecs_switch_header_t *hdr = get_header(sw, value);
        ecs_assert(hdr != NULL, ECS_INTERNAL_ERROR, NULL);

        verify_nodes(hdr, nodes);
        remove_node(hdr, nodes, node, element);
    }

    int32_t last_elem = ecs_vector_count(sw->nodes) - 1;
    if (last_elem != element) {
        flecs_switch_node_t *last = ecs_vector_last(sw->nodes, flecs_switch_node_t);
        int32_t next = last->next, prev = last->prev;
        if (next != -1) {
            flecs_switch_node_t *n = &nodes[next];
            n->prev = element;
        }

        if (prev != -1) {
            flecs_switch_node_t *n = &nodes[prev];
            n->next = element;
        } else {
            flecs_switch_header_t *hdr = get_header(sw, values[last_elem]);
            if (hdr && hdr->element != -1) {
                ecs_assert(hdr->element == last_elem, 
                    ECS_INTERNAL_ERROR, NULL);
                hdr->element = element;
            }
        }
    }

    /* Remove element from arrays */
    ecs_vector_remove(sw->nodes, flecs_switch_node_t, element);
    ecs_vector_remove(sw->values, uint64_t, element);
}

uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    return values[element];
}

ecs_vector_t* flecs_switch_values(
    const ecs_switch_t *sw)
{
    return sw->values;
}

int32_t flecs_switch_case_count(
    const ecs_switch_t *sw,
    uint64_t value)
{
    flecs_switch_header_t *hdr = get_header(sw, value);
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
    ecs_assert((uint32_t)value <= sw->max, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((uint32_t)value >= sw->min, ECS_INVALID_PARAMETER, NULL);
    
    flecs_switch_header_t *hdr = get_header(sw, value);
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);

    return hdr->element;
}

int32_t flecs_switch_next(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    flecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, flecs_switch_node_t);

    return nodes[element].next;
}
