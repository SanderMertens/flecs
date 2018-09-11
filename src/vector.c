#include <reflecs/vector.h>

struct EcsVectorChunk {
    struct EcsVectorChunk *next;
};

struct EcsVector {
    EcsVectorChunk *current;
    uint32_t count;
    EcsVectorChunk first;
};

#define CHUNK_BUFFER(chunk) ECS_OFFSET(chunk, sizeof(EcsVectorChunk))

static
void ecs_vector_deinit(
    EcsVector *me)
{
    EcsVectorChunk *el = &me->first;
    EcsVectorChunk *next = el->next;

    do {
        next = el->next;
        if (el != &me->first) {
            free(el);
        }
    } while ((el = next));
}

static
void ecs_vector_chunk_new(
    EcsVector *me,
    const EcsVectorParams *params)
{
    uint32_t buffer_size = params->element_size * params->chunk_count;
    EcsVectorChunk *el = malloc(sizeof(EcsVectorChunk) + buffer_size);
    EcsVectorChunk *current = me->current;
    el->next = NULL;
    current->next= el;
    me->current = el;
    me->count = 0;
}

static
EcsVectorChunk* ecs_vector_find_chunk(
    EcsVector *me,
    const EcsVectorParams *params,
    void *el)
{
    EcsVectorChunk *result = NULL, *cur = &me->first, *next;
    uint32_t count = params->chunk_count;
    uint32_t vector_count = me->count;

    do {
        next = cur->next;

        if (!next) {
            count = (vector_count - 1) % params->chunk_count + 1;
        }

        void *buffer = CHUNK_BUFFER(cur);
        if (el >= buffer &&
            el < ECS_OFFSET(buffer, count * params->element_size))
        {
            result = cur;
        } else {
            cur = next;
        }
    } while (!result && cur);

    return result;
}

static
EcsVectorChunk* ecs_vector_prev_chunk(
    EcsVector *me,
    EcsVectorChunk *chunk)
{
    EcsVectorChunk *next, *ptr = &me->first;

    if (ptr == chunk) {
        return NULL;
    }

    while ((next = ptr->next) != chunk) {
        ptr = next;
    }

    return ptr;
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
    void *result;
    void *buffer = CHUNK_BUFFER(current);

    if (!index || index < chunk_count) {
        result = ECS_OFFSET(buffer, size * index);
    } else {
        uint32_t cur_index = index % chunk_count;
        if (cur_index) {
            result = ECS_OFFSET(buffer, size * cur_index);
        } else {
            iter_data->current = current->next;
            result = CHUNK_BUFFER(iter_data->current);
        }
    }

    iter_data->index ++;
    return result;
}

EcsVector* ecs_vector_new(
    const EcsVectorParams *params)
{
    uint32_t buffer_size = params->element_size * params->chunk_count;
    EcsVector *result = malloc(sizeof(EcsVector) + buffer_size);
    result->current = &result->first;
    result->count = 0;
    result->first.next = NULL;
    return result;
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
    uint32_t v_count = me->count;

    if (!chunk->next) {
        count = me->count % params->chunk_count;
    }

    if (!count && v_count) {
        ecs_vector_chunk_new(me, params);
        count = 0;
        chunk = chunk->next;
    }

    result = ECS_OFFSET(CHUNK_BUFFER(chunk), params->element_size * count);
    me->count = v_count + 1;

    return result;
}

EcsResult ecs_vector_remove(
    EcsVector *me,
    const EcsVectorParams *params,
    void *element)
{
    if (!element) {
        return EcsError;
    }

    EcsVectorChunk *chunk = ecs_vector_find_chunk(me, params, element);
    if (!chunk) {
        return EcsError;
    }

    uint32_t chunk_count = params->chunk_count;
    if (!chunk->next) {
        chunk_count = (me->count - 1) % params->chunk_count + 1;
    }

    EcsVectorChunk *current = me->current;
    uint32_t size = params->element_size;
    void *last_element =
        ECS_OFFSET(CHUNK_BUFFER(current), (chunk_count - 1) * size);

    if (last_element != element) {
        memcpy(element, last_element, size);

        if (params->move_action) {
            params->move_action(element, last_element, params->ctx);
        }
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

void* ecs_vector_get(
    EcsVector *me,
    const EcsVectorParams *params,
    uint32_t index)
{
    if (!index) {
        return CHUNK_BUFFER(&me->first);
    }

    if (index >= me->count) {
        return NULL;
    }

    uint32_t chunk_count = params->chunk_count;
    uint32_t element_size = params->element_size;
    uint32_t chunk_index = index / chunk_count;
    uint32_t i;
    EcsVectorChunk *chunk_ptr = &me->first;

    for (i = 0; i < chunk_index; i ++) {
        chunk_ptr = chunk_ptr->next;
    }

    return ECS_OFFSET(CHUNK_BUFFER(chunk_ptr), element_size * (index % chunk_count));
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

    iter_data->params = params;
    iter_data->current = &me->first;
    iter_data->index = 0;

    return result;
}

uint32_t ecs_vector_count(
    EcsVector *me)
{
    return me->count;
}

typedef struct cursor {
    const EcsVectorParams *params;
    EcsVectorChunk *chunk;
    uint32_t index;
} cursor;

static
void* cursor_value(
    cursor *c)
{
    return ECS_OFFSET(CHUNK_BUFFER(c->chunk), c->index * c->params->element_size);
}

static
void cursor_next(
    cursor *cur)
{
    cur->index ++;
    if (cur->index >= cur->params->chunk_count) {
        cur->chunk = cur->chunk->next;
        cur->index = 0;
    }
}

static
void cursor_swap(
    cursor *c1,
    cursor *c2)
{
    int i, size = c1->params->element_size;
    char *v1 = cursor_value(c1);
    char *v2 = cursor_value(c2);

    if (v1 == v2)
        return;

    for (i = 0; i < size; i ++) {
        char t = v1[i];
        v1[i] = v2[i];
        v2[i] = t;
    }
}

static
void ecs_vector_qsort(
    cursor start,
    cursor end,
    int32_t total_count)
{
repeat:
    {
        const EcsVectorParams *params = start.params;
        void *pivot = cursor_value(&end);
        cursor c_l = start, c_r = start, c_prev = c_l;
        uint32_t r_count = 0, l_count = 0;

        do {
            if (params->compare_action(cursor_value(&c_r), pivot) < 0) {
                cursor_swap(&c_l, &c_r);
                c_prev = c_l;
                cursor_next(&c_l);
                l_count ++;
            }
            r_count ++;
            if (r_count < total_count) {
                cursor_next(&c_r);
            } else {
                break;
            }
        } while (true);
        r_count --;

        cursor_swap(&c_l, &end);

        int r_size = r_count - l_count;
        if (r_size > l_count) {
            if (l_count) {
                ecs_vector_qsort(start, c_prev, l_count);
            }
            cursor_next(&c_l);
            start = c_l;
            total_count = r_size;
        } else {
            if (r_size) {
                cursor next = c_l;
                cursor_next(&next);
                ecs_vector_qsort(next, end, r_size);
            }
            end = c_prev;
            total_count = l_count;
        }

        if (total_count < 2) {
            return;
        } else {
            goto repeat;
        }
    }
}

void ecs_vector_sort(
    EcsVector *me,
    const EcsVectorParams *params)
{
    if (me->count < 2) {
        return;
    }

    cursor start = {
        .params = params,
        .chunk = &me->first,
        .index = 0
    };

    cursor stop = {
        .params = params,
        .chunk = me->current,
        .index = (me->count - 1) % params->chunk_count
    };

    ecs_vector_qsort(start, stop, me->count);
}

bool ecs_vector_compare_ptr(
    void *a,
    void *b)
{
    return false;
}
