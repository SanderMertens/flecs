#include "private_api.h"

#define INIT_CACHE(it, f, term_count)\
    if (!it->f && term_count) {\
        if (term_count < ECS_TERM_CACHE_SIZE) {\
            it->f = it->cache.f;\
            it->cache.f##_alloc = false;\
        } else {\
            it->f = ecs_os_malloc(ECS_SIZEOF(*(it->f)) * term_count);\
            it->cache.f##_alloc = true;\
        }\
    }

#define FINI_CACHE(it, f)\
    if (it->f) {\
        if (it->cache.f##_alloc) {\
            ecs_os_free((void*)it->f);\
        }\
    }   

void ecs_iter_init(
    ecs_iter_t *it)
{
    INIT_CACHE(it, ids, it->column_count);
    INIT_CACHE(it, columns, it->column_count);
    INIT_CACHE(it, subjects, it->column_count);
    INIT_CACHE(it, sizes, it->column_count);
    INIT_CACHE(it, ptrs, it->column_count);

    it->is_valid = true;
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    ecs_assert(it->is_valid == true, ECS_INVALID_PARAMETER, NULL);
    it->is_valid = false;

    FINI_CACHE(it, ids);
    FINI_CACHE(it, columns);
    FINI_CACHE(it, subjects);
    FINI_CACHE(it, sizes);
    FINI_CACHE(it, ptrs);
}

/* --- Public API --- */

void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || ecs_term_size(it, term) == size, 
        ECS_INVALID_PARAMETER, NULL);

    (void)size;

    if (!term) {
        return it->entities;
    }
    
    if (!it->ptrs) {
        return NULL;
    }

    return it->ptrs[term - 1];
}

bool ecs_term_is_owned(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term > 0, ECS_INVALID_PARAMETER, NULL);
    return it->subjects == NULL || it->subjects[term - 1] == 0;
}

bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t term_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_query_t *query = it->query;

    /* If this is not a query iterator, readonly is meaningless */
    ecs_assert(query != NULL, ECS_INVALID_OPERATION, NULL);
    (void)query;

    ecs_term_t *term = &it->query->filter.terms[term_index - 1];
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (term->inout == EcsIn) {
        return true;
    } else {
        ecs_term_id_t *subj = &term->args[0];

        if (term->inout == EcsInOutDefault) {
            if (subj->entity != EcsThis) {
                return true;
            }

            if ((subj->set.mask != EcsSelf) && 
                (subj->set.mask != EcsDefaultSet)) 
            {
                return true;
            }
        }
    }

    return false;
}

bool ecs_term_is_set(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(term > 0, ECS_INVALID_PARAMETER, NULL);

    return it->columns[term - 1] != 0;
}

ecs_entity_t ecs_term_source(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term > 0, ECS_INVALID_PARAMETER, NULL);
    
    if (!it->subjects) {
        return 0;
    } else {
        return it->subjects[term - 1];
    }
}

ecs_id_t ecs_term_id(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->ids != NULL, ECS_INTERNAL_ERROR, NULL);
    return it->ids[index - 1];
}

size_t ecs_term_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index >= 0, ECS_INVALID_PARAMETER, NULL);

    if (index == 0) {
        return sizeof(ecs_entity_t);
    }

    return flecs_to_size_t(it->sizes[index - 1]);
}

int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_type_index_of(it->table->type, 0, component);
}

void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    ecs_table_t *table = it->table;
    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);
    
    if (table->column_count <= column_index) {
        return NULL;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    ecs_assert(!size || (ecs_size_t)size == column->size, ECS_INVALID_PARAMETER, NULL);

    return ecs_vector_first_t(column->data, column->size, column->alignment);
}

size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t column_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_t *table = it->table;
    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);

    if (table->column_count <= column_index) {
        return 0;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    
    return flecs_to_size_t(column->size);
}
