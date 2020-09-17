#include "flecs.h"

#ifdef FLECS_QUEUE

struct ecs_queue_t {
    ecs_vector_t *data;
    int32_t index;
};

ecs_queue_t* _ecs_queue_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_new(elem_size, offset, elem_count);
    result->index = 0;
    return result;
}

ecs_queue_t* _ecs_queue_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_from_array(elem_size, offset, elem_count, array);
    result->index = 0;
    return result;    
}

void* _ecs_queue_push(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t size = ecs_vector_size(buffer->data);
    int32_t count = ecs_vector_count(buffer->data);
    void *result;

    if (count == buffer->index) {
        result = _ecs_vector_add(&buffer->data, elem_size, offset);
    } else {
        result = _ecs_vector_get(buffer->data, elem_size, offset, buffer->index);
    }

    buffer->index = (buffer->index + 1) % size;

    return result;
}

void ecs_queue_free(
    ecs_queue_t *buffer)
{
    ecs_vector_free(buffer->data);
    ecs_os_free(buffer);
}

void* _ecs_queue_get(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    int32_t count = ecs_vector_count(buffer->data);
    int32_t size = ecs_vector_size(buffer->data);
    index = ((buffer->index - count + size) + (int32_t)index) % size;
    return _ecs_vector_get(buffer->data, elem_size, offset, index);
}

void* _ecs_queue_last(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t index = buffer->index;
    if (!index) {
        index = ecs_vector_size(buffer->data);
    }

    return _ecs_vector_get(buffer->data, elem_size, offset, index - 1);
}

int32_t ecs_queue_index(
    ecs_queue_t *buffer)
{
    return buffer->index;
}

int32_t ecs_queue_count(
    ecs_queue_t *buffer)
{
    return ecs_vector_count(buffer->data);
}

#endif
