#include "private_api.h"

static
void* get_owned_column_ptr(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t table_column,
    int32_t row)
{
    ecs_assert(it->table_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_column_t *column = &((ecs_column_t*)it->table_columns)[table_column - 1];
    ecs_assert(column->size != 0, ECS_COLUMN_HAS_NO_DATA, NULL);
    ecs_assert(!size || column->size == size, ECS_COLUMN_TYPE_MISMATCH, NULL);
    void *buffer = ecs_vector_first_t(column->data, column->size, column->alignment);
    return ECS_OFFSET(buffer, column->size * (it->offset + row));
}

static
const void* get_shared_column(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t table_column)
{
    ecs_ref_t *refs = it->table->references;
    ecs_assert(refs != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_ref_t *ref = &refs[-table_column - 1];
    if (!ref->component) {
        return NULL;
    }

#ifndef NDEBUG
    if (size) {
        ecs_entity_t component_id = ecs_get_typeid(
            it->world, refs[-table_column - 1].component);

        const EcsComponent *cdata = ecs_get(
            it->world, component_id, EcsComponent);

        ecs_assert(cdata != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdata->size == size, ECS_COLUMN_TYPE_MISMATCH, 
            it->system ? ecs_get_name(it->world, it->system) : NULL);
    }
#endif

    return (void*)ecs_get_ref_w_id(
        it->world, ref, ref->entity, ref->component);
}

static
bool get_table_column(
    const ecs_iter_t *it,
    int32_t column,
    int32_t *table_column_out)
{
    ecs_assert(column <= it->column_count, ECS_INVALID_PARAMETER, NULL);

    int32_t table_column = 0;

    if (column != 0) {
        ecs_assert(it->table->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table_column = it->table->columns[column - 1];
        if (!table_column) {
            /* column is not set */
            return false;
        }
    }

    *table_column_out = table_column;

    return true;
}

static
void* get_term(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t column,
    int32_t row)
{
    int32_t table_column;

    if (!column) {
        return it->entities;
    }

    if (!get_table_column(it, column, &table_column)) {
        return NULL;
    }

    if (table_column < 0) {
        return (void*)get_shared_column(it, size, table_column);
    } else {
        return get_owned_column_ptr(it, size, table_column, row);
    }
}


/* --- Public API --- */

void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t term)
{
    return get_term(it, ecs_from_size_t(size), term, 0);
}

bool ecs_term_is_owned(
    const ecs_iter_t *it,
    int32_t term)
{
    int32_t table_column;

    if (!get_table_column(it, term, &table_column)) {
        return true;
    }

    return table_column >= 0;
}

bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t term_index)
{
    ecs_query_t *query = it->query;

    /* If this is not a query iterator, readonly is meaningless */
    ecs_assert(query != NULL, ECS_INVALID_OPERATION, NULL);
    (void)query;

    ecs_term_t *term = &it->query->filter.terms[term_index - 1];
    
    if (term->inout == EcsIn) {
        return true;
    } else {
        ecs_term_id_t *subj = &term->args[0];

        if (term->inout == EcsInOutDefault) {
            if (subj->entity != EcsThis) {
                return true;
            }

            if ((subj->set != EcsSelf) && (subj->set != EcsDefaultSet)) {
                return true;
            }
        }
    }
    
    return false;
}

ecs_entity_t ecs_term_source(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->table->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_iter_table_t *table = it->table;
    int32_t table_column = table->columns[index - 1];
    if (table_column >= 0) {
        return 0;
    }

    ecs_assert(table->references != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_ref_t *ref = &table->references[-table_column - 1];
    return ref->entity;
}

ecs_id_t ecs_term_id(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->table->components != NULL, ECS_INTERNAL_ERROR, NULL);
    return it->table->components[index - 1];
}

size_t ecs_term_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(it->table->columns != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t table_column = it->table->columns[index - 1];
    return ecs_iter_column_size(it, table_column - 1);
}

ecs_type_t ecs_iter_type(
    const ecs_iter_t *it)
{
    /* If no table is set it means that the iterator isn't pointing to anything
     * yet. The most likely cause for this is that the operation is invoked on
     * a new iterator for which "next" hasn't been invoked yet, or on an
     * iterator that is out of elements. */
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_table_t *table = it->table->table;
    return table->type;
}

int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    /* See ecs_iter_type */    
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table->table != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_type_index_of(it->table->table->type, component);
}

void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column_index)
{
    /* See ecs_iter_type */ 
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table->table != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;
    
    ecs_table_t *table = it->table->table;
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
    /* See ecs_iter_type */
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table->table != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *table = it->table->table;
    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);

    if (table->column_count <= column_index) {
        return 0;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    
    return ecs_to_size_t(column->size);
}

// DEPRECATED
void* ecs_element_w_size(
    const ecs_iter_t *it, 
    size_t size,
    int32_t column,
    int32_t row)
{
    return get_term(it, ecs_from_size_t(size), column, row);
}
