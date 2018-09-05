#include <ecs/vector.h>

struct EcsVectorChunk {
    struct EcsVectorChunk *next;
    void *buffer;
};

struct EcsVector {
    uint32_t count;
    EcsVectorChunk *current;
    EcsVectorChunk first;
};

static
void ecs_vector_deinit(
    EcsVector *me)
{
    EcsVectorChunk *el = &me->first;
    EcsVectorChunk *next = el->next;

    do {
        next = el->next;
        if (el == &me->first) {
            free(el->buffer);
        } else {
            free (el->buffer);
        }
        if (el != &me->first) {
            free(el);
        }
    } while (next);
}

static
void ecs_vector_chunk_new(
    EcsVector *me,
    const EcsVectorParams *params)
{
    uint32_t buffer_size = params->element_size * params->chunk_count;
    EcsVectorChunk *el = malloc(sizeof(EcsVectorChunk) + buffer_size);
    el->buffer = ECS_OFFSET(el, sizeof(EcsVectorChunk));
    el->next = NULL;
    me->current->next= el;
    me->current = el;
    me->count = 0;
}

static
EcsVectorChunk* ecs_vector_find_chunk(
    EcsVector *me,
    const EcsVectorParams *params,
    void *el)
{
    EcsVectorChunk *result = NULL, *cur = me->current, *next;
    uint32_t count = params->chunk_count;

    do {
        next = cur->next;

        if (!next) {
            count = me->count % params->chunk_count;
        }

        if (el >= cur->buffer &&
            el < ECS_OFFSET(cur->buffer, count * params->element_size))
        {
            result = cur;
        } else {
            cur = next;
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

static
bool ecs_vector_hasnext(
    EcsIter *me)
{
    EcsVector *vec = me->data;
    EcsVectorIter *iter_data = me->ctx;

    if (iter_data->index < vec->count) {
        return true;
    } else {
        return false;
    }
}

static
void *ecs_vector_next(
    EcsIter *me)
{
    EcsVectorIter *iter_data = me->ctx;
    const EcsVectorParams *params = iter_data->params;
    EcsVectorChunk *current = iter_data->current;
    uint32_t index = iter_data->index;
    uint32_t chunk_count = params->chunk_count;
    uint32_t size = params->element_size;

    if (!index || index < chunk_count) {
        return ECS_OFFSET(current->buffer, size * index);
    } else {
        uint32_t cur_index = index % chunk_count;
        if (cur_index) {
            return ECS_OFFSET(current->buffer, size * cur_index);
        } else {
            iter_data->current = current->next;
            return iter_data->current->buffer;
        }
    }
}

EcsVector* ecs_vector_new(
    const EcsVectorParams *params)
{
    uint32_t buffer_size = params->element_size * params->chunk_count;
    EcsVector *buffer = malloc(sizeof(EcsVector) + buffer_size);
    buffer->current = &buffer->first;
    buffer->count = 0;
    buffer->first.buffer = ECS_OFFSET(buffer, buffer_size);
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
    EcsVector *me,
    const EcsVectorParams *params)
{
    void *result;
    EcsVectorChunk *chunk = me->current;
    uint32_t count = params->chunk_count;

    if (!chunk->next) {
        count = me->count % params->chunk_count;
    }

    if (count == params->chunk_count) {
        ecs_vector_chunk_new(me, params);
        count = 0;
        chunk = chunk->next;
    }

    result = ECS_OFFSET(chunk->buffer, params->element_size * count);
    me->count = count + 1;

    return result;
}

EcsResult ecs_vector_remove(
    EcsVector *me,
    const EcsVectorParams *params,
    void *element)
{
    EcsVectorChunk *chunk = ecs_vector_find_chunk(me, params, element);
    if (!chunk) {
        return EcsError;
    }

    uint32_t chunk_count = params->chunk_count;
    if (!chunk->next) {
        chunk_count = me->count % params->chunk_count;
    }

    EcsVectorChunk *current = me->current;
    uint32_t size = params->element_size;
    void *last_element =
        ECS_OFFSET(current->buffer, (chunk_count - 1) * size);

    if (last_element != element) {
        memcpy(element, last_element, size);
    }

    me->count --;

    if (!(chunk_count - 1)) {
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

EcsIter _ecs_vector_iter(
    EcsVector *me,
    const EcsVectorParams *params,
    EcsVectorIter *iter_data)
{
    EcsIter result = {
        .data = me,
        .ctx = iter_data,
        .hasnext = ecs_vector_hasnext,
        .next = ecs_vector_next,
        .release = NULL
    };

    iter_data->params = 0;
    iter_data->current = NULL;
    iter_data->index = 0;

    return result;
}

uint32_t ecs_vector_count(
    EcsVector *me)
{
    return me->count;
}

void ecs_vector_sort(
    EcsVector *me,
    const EcsVectorParams *params,
    EcsComparator compare)
{

}

bool ecs_vector_compare_ptr(
    void *a,
    void *b)
{
    return false;
}
