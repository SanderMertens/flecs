#include "private_api.h"

static
int flecs_entity_filter_find_smallest_term(
    ecs_table_t *table,
    ecs_entity_filter_iter_t *iter)
{
    flecs_switch_term_t *sw_terms = ecs_vec_first(iter->sw_terms);
    int32_t i, count = ecs_vec_count(iter->sw_terms);
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
            table_column_index -= table->sw_offset;
            ecs_assert(table_column_index >= 1, ECS_INTERNAL_ERROR, NULL);

            /* Get the sparse column */
            ecs_data_t *data = &table->data;
            sw = sparse_column->sw_column = 
                &data->sw_columns[table_column_index - 1];
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

    flecs_switch_term_t *columns = ecs_vec_first(iter->sw_terms);
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
    int32_t i, count = ecs_vec_count(iter->sw_terms);
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

    int32_t i, count = ecs_vec_count(iter->bs_terms);
    flecs_bitset_term_t *terms = ecs_vec_first(iter->bs_terms);
    int32_t bs_offset = table->bs_offset;
    int32_t first = iter->bs_offset;
    int32_t last = 0;

    for (i = 0; i < count; i ++) {
        flecs_bitset_term_t *column = &terms[i];
        ecs_bitset_t *bs = terms[i].bs_column;

        if (!bs) {
            int32_t index = column->column_index;
            ecs_assert((index - bs_offset >= 0), ECS_INTERNAL_ERROR, NULL);
            bs = &table->data.bs_columns[index - bs_offset];
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

int ecs_entity_filter_next(
    ecs_entity_filter_iter_t *it)
{
    ecs_table_t *table = it->range.table;
    flecs_switch_term_t *sw_terms = ecs_vec_first(it->sw_terms);
    flecs_bitset_term_t *bs_terms = ecs_vec_first(it->bs_terms);
    ecs_table_range_t *range = &it->range;
    bool found = false, next_table = true;

    do {
        found = false;

        if (bs_terms) {
            if (flecs_entity_filter_bitset_next(table, it) == -1) {
                /* No more enabled components for table */
                it->bs_offset = 0;
                break;
            } else {
                found = true;
                next_table = false;
            }
        }

        if (sw_terms) {
            if (flecs_entity_filter_switch_next(table, it, found) == -1) {
                /* No more elements in sparse column */
                if (found) {
                    /* Try again */
                    next_table = true;
                    found = false;
                } else {
                    /* Nothing found */
                    it->bs_offset = 0;
                    break;
                }
            } else {
                found = true;
                next_table = false;
                it->bs_offset = range->offset + range->count;
            }
        }
    } while (!found);

    if (!found) {
        return 1;
    } else if (next_table) {
        return 0;
    } else {
        return -1;
    }
}
