#include "flecs_private.h"

#define ELEM(ptr, size, index) ECS_OFFSET(ptr, size * index)

static
int32_t qsort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,    
    int32_t elem_size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{
repeat:
    {
        void *pivot = ELEM(ptr, elem_size, (hi - lo) / 2);
        int32_t i = lo - 1, j = hi;
        void *el;

        do {
            i ++;
            el = ELEM(ptr, elem_size, i);
        } while ( compare(entities[i], el, entities[hi], pivot) < 0);

        do {
            j --;
            el = ELEM(ptr, elem_size, j);
        } while ( compare(entities[j], el, entities[hi], pivot) > 0);

        if (i >= j) {
            return j;
        }

        ecs_table_swap(world, &world->stage, table, data, i, j);
        goto repeat;
    }
}

static
void sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t sort_on_component,
    int32_t component_index,
    ecs_compare_action_t compare)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data || !data->columns) {
        /* Nothing to sort */
        return;
    }

    int32_t count = ecs_table_data_count(data);
    if (!count) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities);
    ecs_column_t *column = &data->columns[component_index];
    void *ptr = ecs_vector_first(column->data);

    int32_t p = qsort_table(
        world, table, data, entities, ptr, column->size, 0, count, compare);

    qsort_table(
        world, table, data, entities, ptr, column->size, 0, p, compare);        

    qsort_table(
        world, table, data, entities, ptr, column->size, p + 1, count, compare);
}

static
void sort_query_tables(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_on_component,
    ecs_compare_action_t compare)
{
    /* Iterate over active tables. Don't bother with inactive tables, since
     * they're empty */
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        /* Get index of sorted component. We only care if the component we're
         * sorting on has changed or if entities have been added / re(moved) */
        int32_t column_count = ecs_vector_count(table->type);
        int32_t index = ecs_type_index_of(table->type, sort_on_component);
        ecs_assert(index != -1, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(index < column_count, ECS_INTERNAL_ERROR, NULL);

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);

        is_dirty |= dirty_state[0] != m_table->monitor[0] ||
            dirty_state[index + 1] != m_table->monitor[index + 1];
        
        /* Check both if entities have moved (element 0) or if the component
         * we're sorting on has changed (index + 1) */
        if (is_dirty) {
            /* Sort the tables */
            sort_table(world, table, sort_on_component, index, compare);

            /* Sorting the table will make it dirty again, so update our monitor
             * after the sort */
            m_table->monitor[0] = dirty_state[0];
            m_table->monitor[index + 1] = dirty_state[index + 1];
        }
    }
}

/* Public API */

ecs_sorted_query_t* ecs_sorted_query_new(
    ecs_world_t *world,
    const char *sig,
    ecs_entity_t sort_on_component,
    ecs_compare_action_t compare)
{
    ecs_assert(compare != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_sorted_query_t *result = ecs_sparse_add(
        world->sorted_queries, ecs_sorted_query_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    result->query = ecs_query_new(world, sig);
    ecs_assert(result->query != NULL, ECS_INTERNAL_ERROR, NULL);

    result->sort_on_component = sort_on_component;
    result->compare = compare;

    sort_query_tables(world, result->query, sort_on_component, compare);

    return result;
}
