
#include "flecs_private.h"

struct ecs_ringbuf_t {
    ecs_vector_t *data;
    int32_t index;
#ifndef NDEBUG
    size_t elem_size;
#endif
};

ecs_ringbuf_t* _ecs_ringbuf_new(
    size_t elem_size,
    int32_t elem_count)
{
    ecs_ringbuf_t *result = ecs_os_malloc(sizeof(ecs_ringbuf_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = ecs_vector_new(elem_size, elem_count);
    result->index = 0;
    return result;
}

void* _ecs_ringbuf_push(
    ecs_ringbuf_t *buffer,
    size_t elem_size)
{
    ecs_assert(elem_size == buffer->elem_size, ECS_INVALID_PARAMETER, NULL);

    int32_t size = ecs_vector_size(buffer->data);
    int32_t count = ecs_vector_count(buffer->data);
    void *result;

    if (count == buffer->index) {
        result = _ecs_vector_add(&buffer->data, elem_size);
    } else {
        result = _ecs_vector_get(buffer->data, elem_size, buffer->index);
    }

    buffer->index = (buffer->index + 1) % size;

    return result;
}

void ecs_ringbuf_free(
    ecs_ringbuf_t *buffer)
{
    ecs_vector_free(buffer->data);
    ecs_os_free(buffer);
}

void* _ecs_ringbuf_get(
    ecs_ringbuf_t *buffer,
    size_t elem_size,
    int32_t index)
{
    int32_t count = ecs_vector_count(buffer->data);
    int32_t size = ecs_vector_size(buffer->data);
    index = ((buffer->index - count + size) + (int32_t)index) % size;
    return _ecs_vector_get(buffer->data, elem_size, index);
}

void* _ecs_ringbuf_last(
    ecs_ringbuf_t *buffer,
    size_t elem_size)
{
    int32_t index = buffer->index;
    if (!index) {
        index = ecs_vector_size(buffer->data);
    }

    return _ecs_vector_get(buffer->data, elem_size, index - 1);
}

int32_t ecs_ringbuf_index(
    ecs_ringbuf_t *buffer)
{
    return buffer->index;
}

int32_t ecs_ringbuf_count(
    ecs_ringbuf_t *buffer)
{
    return ecs_vector_count(buffer->data);
}
