#include <ecs/vector.h>

static
void ecs_vector_deinit(
    EcsVector *me)
{
    EcsVectorChunk *el = &me->first;
    EcsVectorChunk *next = el->next;

    do {
        next = el->next;
        if (el == &me->first) {
            if (!me->initial_buffer_set) {
                free(el->buffer);
            }
        } else {
            free (el->buffer);
        }
        if (el != &me->first) {
            free(el);
        }
    } while (next);
}

static
uint32_t ecs_vector_buffer_size(
    EcsVector *me)
{
    return me->size * me->buffer_count;
}

static
void ecs_vector_chunk_new(
    EcsVector *me)
{
    uint32_t buffer_size = ecs_vector_buffer_size(me);
    EcsVectorChunk *el = malloc(sizeof(EcsVectorChunk) + buffer_size);
    el->buffer = ECS_OFFSET(el, sizeof(EcsVectorChunk));
    el->count = 0;
    el->next = NULL;
    me->current->next= el;
    me->current = el;
}

static
EcsVectorChunk* ecs_vector_find_chunk(
    EcsVector *me,
    void *el)
{
    EcsVectorChunk *result = NULL, *cur = me->current;

    do {
        if (el >= cur->buffer &&
            el < ECS_OFFSET(cur->buffer, cur->count * me->size))
        {
            result = cur;
        } else {
            cur = cur->next;
        }
    } while (!result);

    return result;
}

static
EcsVectorChunk* ecs_vector_prev_chunk(
    EcsVector *me,
    EcsVectorChunk *chunk)
{
    EcsVectorChunk *cur = &me->first;

    if (chunk == cur) {
        return NULL;
    }

    while (cur && cur->next != chunk) {
        cur = cur->next;
    }

    return cur;
}

EcsVector* ecs_vector_new(
    uint32_t size,
    uint32_t buffer_count,
    void *initial_buffer)
{
    EcsVector *buffer;

    if (initial_buffer) {
        buffer = malloc(sizeof(EcsVector));
        buffer->first.buffer = initial_buffer;
        buffer->initial_buffer_set = true;
    } else {
        uint32_t buffer_size = size * buffer_count;
        buffer = malloc(sizeof(EcsVector) + buffer_size);
        buffer->first.buffer = ECS_OFFSET(buffer, buffer_size);
        buffer->initial_buffer_set = false;
    }

    buffer->size = size;
    buffer->buffer_count = buffer_count;
    buffer->current = &buffer->first;
    buffer->first.count = 0;
    buffer->first.next = NULL;

    return buffer;
}

void ecs_vector_free(
    EcsVector *me)
{
    ecs_vector_deinit(me);
    free(me);
}

void* ecs_vector_add(
    EcsVector *me)
{
    void *result;
    EcsVectorChunk *chunk = me->current;
    uint32_t count = chunk->count;

    if (count == me->buffer_count) {
        ecs_vector_chunk_new(me);
        count = 0;
        chunk = chunk->next;
    }

    result = ECS_OFFSET(chunk->buffer, me->size * count);
    chunk->count = count + 1;

    return result;
}

EcsResult ecs_vector_remove(
    EcsVector *me,
    void *el)
{
    EcsVectorChunk *chunk = ecs_vector_find_chunk(me, el);
    if (!chunk) {
        return EcsError;
    }

    EcsVectorChunk *current = me->current;
    uint32_t size = me->size;
    void *last_element =
        ECS_OFFSET(current->buffer, (current->count - 1) * size);

    if (last_element != el) {
        memcpy(el, last_element, size);
    }

    current->count --;

    if (!current->count) {
        EcsVectorChunk *prev_chunk = ecs_vector_prev_chunk(me, current);
        if (prev_chunk) {
            free(current);
            prev_chunk->next = NULL;
            me->current = prev_chunk;
        } else {
            /* chunk is the first chunk, don't delete */
        }
    }

    return EcsOk;
}
