#include "flecs.h"

static
ecs_switch_header_t *get_header(
    const ecs_switch_t *sw,
    int32_t value)
{
    if (value == 0) {
        return NULL;
    }

    int32_t index = value - sw->min;
    return &sw->headers[index];
}

static
void remove_node(
    ecs_switch_header_t *hdr,
    ecs_switch_node_t *nodes,
    ecs_switch_node_t *node,
    int32_t element)
{
    /* The node is currently assigned to a value */
    if (hdr->element == element) {
        /* If this is the first node, update the header */
        hdr->element = node->next;
    } else {
        /* If this is not the first node, update the previous node */
        ecs_switch_node_t *prev_node = &nodes[node->prev];
        prev_node->next = node->next;
    }

    if (node->next != -1) {
        /* If this is not the last node, update the next node */
        ecs_switch_node_t *next_node = &nodes[node->next];
        next_node->prev = node->prev;
    }

    /* Decrease count of current header */
    hdr->count --;
}

ecs_switch_t* ecs_switch_new(
    int32_t min, 
    int32_t max,
    int32_t elements)
{
    ecs_assert(min != max, ECS_INVALID_PARAMETER, NULL);

    /* Min must be larger than 0, as 0 is an invalid entity id, and should
     * therefore never occur as case id */
    ecs_assert(min > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_switch_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_switch_t));
    result->min = min;
    result->max = max;

    int32_t count = max - min;
    result->headers = ecs_os_calloc(ECS_SIZEOF(ecs_switch_header_t) * count);
    result->nodes = ecs_vector_new(ecs_switch_node_t, elements);

    int32_t i;
    for (i = 0; i < count; i ++) {
        result->headers[i].element = -1;
        result->headers[i].count = 0;
    }

    ecs_switch_node_t *nodes = ecs_vector_first(
        result->nodes, ecs_switch_node_t);

    for (i = 0; i < elements; i ++) {
        nodes[i].prev = -1;
        nodes[i].next = -1;
        nodes[i].value = -1;
    }

    return result;
}

void ecs_switch_free(
    ecs_switch_t *sw)
{
    ecs_os_free(sw->headers);
    ecs_vector_free(sw->nodes);
    ecs_os_free(sw);
}

void ecs_switch_add(
    ecs_switch_t *sw)
{
    ecs_switch_node_t *node = ecs_vector_add(&sw->nodes, ecs_switch_node_t);
    node->prev = -1;
    node->next = -1;
    node->value = 0;
}

void ecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    ecs_vector_set_count(&sw->nodes, ecs_switch_node_t, count);

    ecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, ecs_switch_node_t);
    int32_t i;
    for (i = old_count; i < count; i ++) {
        ecs_switch_node_t *node = &nodes[i];
        node->prev = -1;
        node->next = -1;
        node->value = 0;        
    }
}

void ecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    ecs_switch_set_count(sw, old_count + count);
}

void ecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    int32_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);

    ecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, ecs_switch_node_t);

    ecs_switch_node_t *node = &nodes[element];

    /* If the node is already assigned to the value, nothing to be done */
    if (node->value == value) {
        return;
    }

    ecs_switch_header_t *cur_hdr = get_header(sw, node->value);
    ecs_switch_header_t *dst_hdr = get_header(sw, value);

    if (cur_hdr) {
        remove_node(cur_hdr, nodes, node, element);
    }

    /* Now update the node itself by adding it as the first node of dst */
    node->next = dst_hdr->element;
    node->prev = -1;
    node->value = value;

    /* Also update the dst header */
    dst_hdr->element = element;
    dst_hdr->count ++;
}

void ecs_switch_remove(
    ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);

    ecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, ecs_switch_node_t);

    ecs_switch_node_t *node = &nodes[element];

    /* If the node is not assigned to a value, nothing to be done */
    if (node->value == -1) {
        return;
    }

    ecs_switch_header_t *hdr = get_header(sw, node->value);
    ecs_assert(hdr != NULL, ECS_INTERNAL_ERROR, NULL);

    remove_node(hdr, nodes, node, element);

    node->value = 0;
    node->prev = -1;
    node->next = -1;
}

int32_t ecs_switch_get_case(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);

    ecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, ecs_switch_node_t);

    return nodes[element].value;
}

int32_t ecs_switch_first(
    const ecs_switch_t *sw,
    int32_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value <= sw->max, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value >= sw->min, ECS_INVALID_PARAMETER, NULL);

    ecs_switch_header_t *hdr = get_header(sw, value);
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);

    return hdr->element;
}

int32_t ecs_switch_next(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);

    ecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, ecs_switch_node_t);

    return nodes[element].next;
}
