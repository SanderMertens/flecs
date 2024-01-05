/**
 * @file entity_filter.c
 * @brief Filters that are applied to entities in a table.
 * 
 * After a table has been matched by a query, additional filters may have to
 * be applied before returning entities to the application. The two scenarios
 * under which this happens are queries for union relationship pairs (entities
 * for multiple targets are stored in the same table) and toggles (components 
 * that are enabled/disabled with a bitset).
 */

#include "private_api.h"

static
int flecs_entity_filter_find_smallest_term(
    ecs_table_t *table,
    ecs_entity_filter_iter_t *iter)
{
    ecs_assert(table->_ != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_switch_term_t *sw_terms = ecs_vec_first(&iter->entity_filter->sw_terms);
    int32_t i, count = ecs_vec_count(&iter->entity_filter->sw_terms);
    int32_t min = INT_MAX, index = 0;

    for (i = 0; i < count; i ++) {
        /* The array with sparse queries for the matched table */
        flecs_switch_term_t *sparse_column = &sw_terms[i];

        /* Pointer to the switch column struct of the table */
        ecs_switch_t *sw = sparse_column->sw_column;

        /* If the sparse column pointer hadn't been retrieved yet, do it now */
        if (!sw) {
            /* Get the table column index from the signature column index */
            int32_t table_column_index = iter->columns[
                sparse_column->signature_column_index];

            /* Translate the table column index to switch column index */
            table_column_index -= table->_->sw_offset;
            ecs_assert(table_column_index >= 1, ECS_INTERNAL_ERROR, NULL);

            /* Get the sparse column */
            sw = sparse_column->sw_column = 
                &table->_->sw_columns[table_column_index - 1];
        }

        /* Find the smallest column */
        int32_t case_count = flecs_switch_case_count(sw, sparse_column->sw_case);
        if (case_count < min) {
            min = case_count;
            index = i + 1;
        }
    }

    return index;
}

static
int flecs_entity_filter_switch_next(
    ecs_table_t *table,
    ecs_entity_filter_iter_t *iter,
    bool filter)
{
    bool first_iteration = false;
    int32_t switch_smallest;

    if (!(switch_smallest = iter->sw_smallest)) {
        switch_smallest = iter->sw_smallest = 
            flecs_entity_filter_find_smallest_term(table, iter);
        first_iteration = true;
    }

    switch_smallest -= 1;

    flecs_switch_term_t *columns = ecs_vec_first(&iter->entity_filter->sw_terms);
    flecs_switch_term_t *column = &columns[switch_smallest];
    ecs_switch_t *sw, *sw_smallest = column->sw_column;
    ecs_entity_t case_smallest = column->sw_case;

    /* Find next entity to iterate in sparse column */
    int32_t first, sparse_first = iter->sw_offset;

    if (!filter) {
        if (first_iteration) {
            first = flecs_switch_first(sw_smallest, case_smallest);
        } else {
            first = flecs_switch_next(sw_smallest, sparse_first);
        }
    } else {
        int32_t cur_first = iter->range.offset, cur_count = iter->range.count;
        first = cur_first;
        while (flecs_switch_get(sw_smallest, first) != case_smallest) {
            first ++;
            if (first >= (cur_first + cur_count)) {
                first = -1;
                break;
            }
        }
    }

    if (first == -1) {
        goto done;
    }

    /* Check if entity matches with other sparse columns, if any */
    int32_t i, count = ecs_vec_count(&iter->entity_filter->sw_terms);
    do {
        for (i = 0; i < count; i ++) {
            if (i == switch_smallest) {
                /* Already validated this one */
                continue;
            }

            column = &columns[i];
            sw = column->sw_column;

            if (flecs_switch_get(sw, first) != column->sw_case) {
                first = flecs_switch_next(sw_smallest, first);
                if (first == -1) {
                    goto done;
                }
            }
        }
    } while (i != count);

    iter->range.offset = iter->sw_offset = first;
    iter->range.count = 1;

    return 0;
done:
    /* Iterated all elements in the sparse list, we should move to the
     * next matched table. */
    iter->sw_smallest = 0;
    iter->sw_offset = 0;

    return -1;
}

#define BS_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

static
int flecs_entity_filter_bitset_next(
    ecs_table_t *table,
    ecs_entity_filter_iter_t *iter)
{
    /* Precomputed single-bit test */
    static const uint64_t bitmask[64] = {
    (uint64_t)1 << 0, (uint64_t)1 << 1, (uint64_t)1 << 2, (uint64_t)1 << 3,
    (uint64_t)1 << 4, (uint64_t)1 << 5, (uint64_t)1 << 6, (uint64_t)1 << 7,
    (uint64_t)1 << 8, (uint64_t)1 << 9, (uint64_t)1 << 10, (uint64_t)1 << 11,
    (uint64_t)1 << 12, (uint64_t)1 << 13, (uint64_t)1 << 14, (uint64_t)1 << 15,
    (uint64_t)1 << 16, (uint64_t)1 << 17, (uint64_t)1 << 18, (uint64_t)1 << 19,
    (uint64_t)1 << 20, (uint64_t)1 << 21, (uint64_t)1 << 22, (uint64_t)1 << 23,
    (uint64_t)1 << 24, (uint64_t)1 << 25, (uint64_t)1 << 26, (uint64_t)1 << 27,  
    (uint64_t)1 << 28, (uint64_t)1 << 29, (uint64_t)1 << 30, (uint64_t)1 << 31,
    (uint64_t)1 << 32, (uint64_t)1 << 33, (uint64_t)1 << 34, (uint64_t)1 << 35,  
    (uint64_t)1 << 36, (uint64_t)1 << 37, (uint64_t)1 << 38, (uint64_t)1 << 39,
    (uint64_t)1 << 40, (uint64_t)1 << 41, (uint64_t)1 << 42, (uint64_t)1 << 43,
    (uint64_t)1 << 44, (uint64_t)1 << 45, (uint64_t)1 << 46, (uint64_t)1 << 47,  
    (uint64_t)1 << 48, (uint64_t)1 << 49, (uint64_t)1 << 50, (uint64_t)1 << 51,
    (uint64_t)1 << 52, (uint64_t)1 << 53, (uint64_t)1 << 54, (uint64_t)1 << 55,  
    (uint64_t)1 << 56, (uint64_t)1 << 57, (uint64_t)1 << 58, (uint64_t)1 << 59,
    (uint64_t)1 << 60, (uint64_t)1 << 61, (uint64_t)1 << 62, (uint64_t)1 << 63
    };

    /* Precomputed test to verify if remainder of block is set (or not) */
    static const uint64_t bitmask_remain[64] = {
    BS_MAX, BS_MAX - (BS_MAX >> 63), BS_MAX - (BS_MAX >> 62),
    BS_MAX - (BS_MAX >> 61), BS_MAX - (BS_MAX >> 60), BS_MAX - (BS_MAX >> 59),
    BS_MAX - (BS_MAX >> 58), BS_MAX - (BS_MAX >> 57), BS_MAX - (BS_MAX >> 56), 
    BS_MAX - (BS_MAX >> 55), BS_MAX - (BS_MAX >> 54), BS_MAX - (BS_MAX >> 53), 
    BS_MAX - (BS_MAX >> 52), BS_MAX - (BS_MAX >> 51), BS_MAX - (BS_MAX >> 50), 
    BS_MAX - (BS_MAX >> 49), BS_MAX - (BS_MAX >> 48), BS_MAX - (BS_MAX >> 47), 
    BS_MAX - (BS_MAX >> 46), BS_MAX - (BS_MAX >> 45), BS_MAX - (BS_MAX >> 44), 
    BS_MAX - (BS_MAX >> 43), BS_MAX - (BS_MAX >> 42), BS_MAX - (BS_MAX >> 41), 
    BS_MAX - (BS_MAX >> 40), BS_MAX - (BS_MAX >> 39), BS_MAX - (BS_MAX >> 38), 
    BS_MAX - (BS_MAX >> 37), BS_MAX - (BS_MAX >> 36), BS_MAX - (BS_MAX >> 35), 
    BS_MAX - (BS_MAX >> 34), BS_MAX - (BS_MAX >> 33), BS_MAX - (BS_MAX >> 32), 
    BS_MAX - (BS_MAX >> 31), BS_MAX - (BS_MAX >> 30), BS_MAX - (BS_MAX >> 29), 
    BS_MAX - (BS_MAX >> 28), BS_MAX - (BS_MAX >> 27), BS_MAX - (BS_MAX >> 26), 
    BS_MAX - (BS_MAX >> 25), BS_MAX - (BS_MAX >> 24), BS_MAX - (BS_MAX >> 23), 
    BS_MAX - (BS_MAX >> 22), BS_MAX - (BS_MAX >> 21), BS_MAX - (BS_MAX >> 20), 
    BS_MAX - (BS_MAX >> 19), BS_MAX - (BS_MAX >> 18), BS_MAX - (BS_MAX >> 17), 
    BS_MAX - (BS_MAX >> 16), BS_MAX - (BS_MAX >> 15), BS_MAX - (BS_MAX >> 14), 
    BS_MAX - (BS_MAX >> 13), BS_MAX - (BS_MAX >> 12), BS_MAX - (BS_MAX >> 11), 
    BS_MAX - (BS_MAX >> 10), BS_MAX - (BS_MAX >> 9), BS_MAX - (BS_MAX >> 8), 
    BS_MAX - (BS_MAX >> 7), BS_MAX - (BS_MAX >> 6), BS_MAX - (BS_MAX >> 5), 
    BS_MAX - (BS_MAX >> 4), BS_MAX - (BS_MAX >> 3), BS_MAX - (BS_MAX >> 2),
    BS_MAX - (BS_MAX >> 1)
    };

    int32_t i, count = ecs_vec_count(&iter->entity_filter->bs_terms);
    flecs_bitset_term_t *terms = ecs_vec_first(&iter->entity_filter->bs_terms);
    int32_t bs_offset = table->_->bs_offset;
    int32_t first = iter->bs_offset;
    int32_t last = 0;

    for (i = 0; i < count; i ++) {
        flecs_bitset_term_t *column = &terms[i];
        ecs_bitset_t *bs = terms[i].bs_column;

        if (!bs) {
            int32_t index = column->column_index;
            ecs_assert((index - bs_offset >= 0), ECS_INTERNAL_ERROR, NULL);
            bs = &table->_->bs_columns[index - bs_offset];
            terms[i].bs_column = bs;
        }
        
        int32_t bs_elem_count = bs->count;
        int32_t bs_block = first >> 6;
        int32_t bs_block_count = ((bs_elem_count - 1) >> 6) + 1;

        if (bs_block >= bs_block_count) {
            goto done;
        }

        uint64_t *data = bs->data;
        int32_t bs_start = first & 0x3F;

        /* Step 1: find the first non-empty block */
        uint64_t v = data[bs_block];
        uint64_t remain = bitmask_remain[bs_start];
        while (!(v & remain)) {
            /* If no elements are remaining, move to next block */
            if ((++bs_block) >= bs_block_count) {
                /* No non-empty blocks left */
                goto done;
            }

            bs_start = 0;
            remain = BS_MAX; /* Test the full block */
            v = data[bs_block];
        }

        /* Step 2: find the first non-empty element in the block */
        while (!(v & bitmask[bs_start])) {
            bs_start ++;

            /* Block was not empty, so bs_start must be smaller than 64 */
            ecs_assert(bs_start < 64, ECS_INTERNAL_ERROR, NULL);
        }

        /* Step 3: Find number of contiguous enabled elements after start */
        int32_t bs_end = bs_start, bs_block_end = bs_block;

        remain = bitmask_remain[bs_end];
        while ((v & remain) == remain) {
            bs_end = 0;
            bs_block_end ++;

            if (bs_block_end == bs_block_count) {
                break;
            }

            v = data[bs_block_end];
            remain = BS_MAX; /* Test the full block */
        }

        /* Step 4: find remainder of enabled elements in current block */
        if (bs_block_end != bs_block_count) {
            while ((v & bitmask[bs_end])) {
                bs_end ++;
            }
        }

        /* Block was not 100% occupied, so bs_start must be smaller than 64 */
        ecs_assert(bs_end < 64, ECS_INTERNAL_ERROR, NULL);

        /* Step 5: translate to element start/end and make sure that each column
         * range is a subset of the previous one. */
        first = bs_block * 64 + bs_start;
        int32_t cur_last = bs_block_end * 64 + bs_end;
        
        /* No enabled elements found in table */
        if (first == cur_last) {
            goto done;
        }

        /* If multiple bitsets are evaluated, make sure each subsequent range
         * is equal or a subset of the previous range */
        if (i) {
            /* If the first element of a subsequent bitset is larger than the
             * previous last value, start over. */
            if (first >= last) {
                i = -1;
                continue;
            }

            /* Make sure the last element of the range doesn't exceed the last
             * element of the previous range. */
            if (cur_last > last) {
                cur_last = last;
            }
        }

        last = cur_last;
        int32_t elem_count = last - first;

        /* Make sure last element doesn't exceed total number of elements in 
         * the table */
        if (elem_count > (bs_elem_count - first)) {
            elem_count = (bs_elem_count - first);
            if (!elem_count) {
                iter->bs_offset = 0;
                goto done;
            }
        }
        
        iter->range.offset = first;
        iter->range.count = elem_count;
        iter->bs_offset = first;
    }

    /* Keep track of last processed element for iteration */ 
    iter->bs_offset = last;

    return 0;
done:
    iter->sw_smallest = 0;
    iter->sw_offset = 0;
    return -1;
}

#undef BS_MAX

static
int32_t flecs_get_flattened_target(
    ecs_world_t *world,
    EcsTarget *cur,
    ecs_entity_t rel,
    ecs_id_t id,
    ecs_entity_t *src_out,
    ecs_table_record_t **tr_out)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return -1;
    }

    ecs_record_t *r = cur->target;
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = r->table;
    if (!table) {
        return -1;
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (tr) {
        *src_out = ecs_record_get_entity(r);
        *tr_out = tr;
        return tr->index;
    }

    if (table->flags & EcsTableHasTarget) {
        int32_t col = table->column_map[table->_->ft_offset];
        ecs_assert(col != -1, ECS_INTERNAL_ERROR, NULL);
        EcsTarget *next = table->data.columns[col].data.array;
        next = ECS_ELEM_T(next, EcsTarget, ECS_RECORD_TO_ROW(r->row));
        return flecs_get_flattened_target(
            world, next, rel, id, src_out, tr_out);
    }

    return ecs_search_relation(
        world, table, 0, id, rel, EcsSelf|EcsUp, src_out, NULL, tr_out);
}

void flecs_entity_filter_init(
    ecs_world_t *world,
    ecs_entity_filter_t **entity_filter,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_id_t *ids,
    int32_t *columns)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(entity_filter != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(filter != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &world->allocator;
    ecs_entity_filter_t ef;
    ecs_os_zeromem(&ef);
    ecs_vec_t *sw_terms = &ef.sw_terms;
    ecs_vec_t *bs_terms = &ef.bs_terms;
    ecs_vec_t *ft_terms = &ef.ft_terms;
    if (*entity_filter) {
        ef.sw_terms = (*entity_filter)->sw_terms;
        ef.bs_terms = (*entity_filter)->bs_terms;
        ef.ft_terms = (*entity_filter)->ft_terms;
    }
    ecs_vec_reset_t(a, sw_terms, flecs_switch_term_t);
    ecs_vec_reset_t(a, bs_terms, flecs_bitset_term_t);
    ecs_vec_reset_t(a, ft_terms, flecs_flat_table_term_t);
    ecs_term_t *terms = filter->terms;
    int32_t i, term_count = filter->term_count;
    bool has_filter = false;
    ef.flat_tree_column = -1;

    /* Look for union fields */
    if (table->flags & EcsTableHasUnion) {
        for (i = 0; i < term_count; i ++) {
            if (ecs_term_match_0(&terms[i])) {
                continue;
            }

            ecs_id_t id = terms[i].id;
            if (ECS_HAS_ID_FLAG(id, PAIR) && ECS_PAIR_SECOND(id) == EcsWildcard) {
                continue;
            }
            
            int32_t field = terms[i].field_index;
            int32_t column = columns[field];
            if (column <= 0) {
                continue;
            }

            ecs_id_t table_id = table->type.array[column - 1];
            if (ECS_PAIR_FIRST(table_id) != EcsUnion) {
                continue;
            }

            flecs_switch_term_t *el = ecs_vec_append_t(a, sw_terms, 
                flecs_switch_term_t);
            el->signature_column_index = field;
            el->sw_case = ecs_pair_second(world, id);
            el->sw_column = NULL;
            ids[field] = id;
            has_filter = true;
        }
    }

    /* Look for disabled fields */
    if (table->flags & EcsTableHasToggle) {
        for (i = 0; i < term_count; i ++) {
            if (ecs_term_match_0(&terms[i])) {
                continue;
            }

            int32_t field = terms[i].field_index;
            ecs_id_t id = ids[field];
            ecs_id_t bs_id = ECS_TOGGLE | id;
            int32_t bs_index = ecs_table_get_type_index(world, table, bs_id);

            if (bs_index != -1) {
                flecs_bitset_term_t *bc = ecs_vec_append_t(a, bs_terms, 
                    flecs_bitset_term_t);
                bc->column_index = bs_index;
                bc->bs_column = NULL;
                has_filter = true;
            }
        }
    }

    /* Look for flattened fields */
    if (table->flags & EcsTableHasTarget) {
        const ecs_table_record_t *tr = flecs_table_record_get(world, table, 
            ecs_pair_t(EcsTarget, EcsWildcard));
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t column = tr->index;
        ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t rel = ecs_pair_second(world, table->type.array[column]);

        for (i = 0; i < term_count; i ++) {
            if (ecs_term_match_0(&terms[i])) {
                continue;
            }

            if (terms[i].src.trav == rel) {
                ef.flat_tree_column = table->column_map[column];
                ecs_assert(ef.flat_tree_column != -1, 
                    ECS_INTERNAL_ERROR, NULL);
                has_filter = true;
                
                flecs_flat_table_term_t *term = ecs_vec_append_t(
                    a, ft_terms, flecs_flat_table_term_t);
                term->field_index = terms[i].field_index;
                term->term = &terms[i];
                ecs_os_zeromem(&term->monitor);
            }
        }
    }

    if (has_filter) {
        if (!*entity_filter) {
            *entity_filter = ecs_os_malloc_t(ecs_entity_filter_t);
        }
        ecs_assert(*entity_filter != NULL, ECS_OUT_OF_MEMORY, NULL);
        **entity_filter = ef;
    }
}

void flecs_entity_filter_fini(
    ecs_world_t *world,
    ecs_entity_filter_t *ef)
{
    if (!ef) {
        return;
    }

    ecs_allocator_t *a = &world->allocator;

    flecs_flat_table_term_t *fields = ecs_vec_first(&ef->ft_terms);
    int32_t i, term_count = ecs_vec_count(&ef->ft_terms);
    for (i = 0; i < term_count; i ++) {
        ecs_vec_fini_t(NULL, &fields[i].monitor, flecs_flat_monitor_t);
    }

    ecs_vec_fini_t(a, &ef->sw_terms, flecs_switch_term_t);
    ecs_vec_fini_t(a, &ef->bs_terms, flecs_bitset_term_t);
    ecs_vec_fini_t(a, &ef->ft_terms, flecs_flat_table_term_t);
    ecs_os_free(ef);
}

int flecs_entity_filter_next(
    ecs_entity_filter_iter_t *it)
{
    ecs_table_t *table = it->range.table;
    flecs_switch_term_t *sw_terms = ecs_vec_first(&it->entity_filter->sw_terms);
    flecs_bitset_term_t *bs_terms = ecs_vec_first(&it->entity_filter->bs_terms);
    ecs_entity_filter_t *ef = it->entity_filter;
    int32_t flat_tree_column = ef->flat_tree_column;
    ecs_table_range_t *range = &it->range;
    int32_t range_end = range->offset + range->count;
    int result = EcsIterNext;
    bool found = false;

    do {
        found = false;

        if (bs_terms) {
            if (flecs_entity_filter_bitset_next(table, it) == -1) {
                /* No more enabled components for table */
                it->bs_offset = 0;
                break;
            } else {
                result = EcsIterYield;
                found = true;
            }
        }

        if (sw_terms) {
            if (flecs_entity_filter_switch_next(table, it, found) == -1) {
                /* No more elements in sparse column */
                if (found) {
                    /* Try again */
                    result = EcsIterNext;
                    found = false;
                } else {
                    /* Nothing found */
                    it->bs_offset = 0;
                    break;
                }
            } else {
                result = EcsIterYield;
                found = true;
                it->bs_offset = range->offset + range->count;
            }
        }

        if (flat_tree_column != -1) {
            bool first_for_table = it->prev != table;
            ecs_iter_t *iter = it->it;
            ecs_world_t *world = iter->real_world;
            EcsTarget *ft = table->data.columns[flat_tree_column].data.array;
            int32_t ft_offset;
            int32_t ft_count;

            if (first_for_table) {
                ft_offset = it->flat_tree_offset = range->offset;
                it->target_count = 1;
            } else {
                it->flat_tree_offset += ft[it->flat_tree_offset].count;
                ft_offset = it->flat_tree_offset;
                it->target_count ++;
            }

            ecs_assert(ft_offset < ecs_table_count(table), 
                ECS_INTERNAL_ERROR, NULL);

            EcsTarget *cur = &ft[ft_offset];
            ft_count = cur->count;
            bool is_last = (ft_offset + ft_count) >= range_end;

            int32_t i, field_count = ecs_vec_count(&ef->ft_terms);
            flecs_flat_table_term_t *fields = ecs_vec_first(&ef->ft_terms);
            for (i = 0; i < field_count; i ++) {
                flecs_flat_table_term_t *field = &fields[i];
                ecs_vec_init_if_t(&field->monitor, flecs_flat_monitor_t);
                int32_t field_index = field->field_index;
                ecs_id_t id = it->it->ids[field_index];
                ecs_id_t flat_pair = table->type.array[flat_tree_column];
                ecs_entity_t rel = ECS_PAIR_FIRST(flat_pair);
                ecs_entity_t tgt;
                ecs_table_record_t *tr;
                int32_t tgt_col = flecs_get_flattened_target(
                    world, cur, rel, id, &tgt, &tr);
                if (tgt_col != -1) {
                    iter->sources[field_index] = tgt;
                    iter->columns[field_index] = /* encode flattened field */
                        -(iter->field_count + tgt_col + 1);
                    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);

                    /* Keep track of maximum value encountered in target table
                     * dirty state so this doesn't have to be recomputed when
                     * synchronizing the query monitor. */
                    ecs_vec_set_min_count_zeromem_t(NULL, &field->monitor, 
                        flecs_flat_monitor_t, it->target_count);
                    ecs_table_t *tgt_table = tr->hdr.table;
                    int32_t *ds = flecs_table_get_dirty_state(world, tgt_table);
                    ecs_assert(ds != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_vec_get_t(&field->monitor, flecs_flat_monitor_t, 
                        it->target_count - 1)->table_state = ds[tgt_col + 1];
                } else {
                    if (field->term->oper == EcsOptional) {
                        iter->columns[field_index] = 0;
                        iter->ptrs[field_index] = NULL;
                    } else {
                        it->prev = NULL;
                        break;
                    }
                }
            }
            if (i != field_count) {
                if (is_last) {
                    break;
                }
            } else {
                found = true;
                if ((ft_offset + ft_count) == range_end) {
                    result = EcsIterNextYield;
                } else {
                    result = EcsIterYield;
                }
            }

            range->offset = ft_offset;
            range->count = ft_count;
            it->prev = table;
        }
    } while (!found);

    it->prev = table;

    if (!found) {
        return EcsIterNext;
    } else {
        return result;
    }
}
