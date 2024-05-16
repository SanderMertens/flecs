/**
 * @file query/engine/eval_toggle.c
 * @brief Bitset toggle evaluation.
 */

#include "../../private_api.h"

typedef struct {
    ecs_flags64_t mask;
    bool has_bitset;
} flecs_query_row_mask_t;

static
flecs_query_row_mask_t flecs_query_get_row_mask(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t block_index,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields,
    ecs_query_toggle_ctx_t *op_ctx)
{
    ecs_flags64_t mask = UINT64_MAX;
    int32_t i, field_count = it->field_count;
    ecs_flags64_t fields = and_fields | not_fields;
    bool has_bitset = false;

    for (i = 0; i < field_count; i ++) {
        uint64_t field_bit = 1llu << i;
        if (!(fields & field_bit)) {
            continue;
        }

        if (not_fields & field_bit) {
            it->set_fields &= (ecs_termset_t)~field_bit;
        } else if (and_fields & field_bit) {
            ecs_assert(it->set_fields & field_bit, ECS_INTERNAL_ERROR, NULL);
        } else {
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }

        ecs_id_t id = it->ids[i];
        ecs_bitset_t *bs = flecs_table_get_toggle(table, id);
        if (!bs) {
            if (not_fields & field_bit) {
                if (op_ctx->prev_set_fields & field_bit) {
                    has_bitset = false;
                    break;
                }
            }
            continue;
        }

        ecs_assert((64 * block_index) < bs->size, ECS_INTERNAL_ERROR, NULL);
        ecs_flags64_t block = bs->data[block_index];

        if (not_fields & field_bit) {
            block = ~block;
        }
        mask &= block;
        has_bitset = true;
    }

    return (flecs_query_row_mask_t){ mask, has_bitset };
}

static
bool flecs_query_toggle_for_up(
    ecs_iter_t *it,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields)
{
    int32_t i, field_count = it->field_count;
    ecs_flags64_t fields = (and_fields | not_fields) & it->up_fields;

    for (i = 0; i < field_count; i ++) {
        uint64_t field_bit = 1llu << i;
        if (!(fields & field_bit)) {
            continue;
        }

        bool match = false;
        if ((it->set_fields & field_bit)) {
            ecs_entity_t src = it->sources[i];
            ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);
            match = ecs_is_enabled_id(it->world, src, it->ids[i]);
        }

        if (field_bit & not_fields) {
            match = !match;
        }

        if (!match) {
            return false;
        }
    }

    return true;
}

static
bool flecs_query_toggle_cmp(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    ecs_table_t *table = range.table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if ((and_fields & op_ctx->prev_set_fields) != and_fields) {
        /* If not all fields matching and toggles are set, table can't match */
        return false;
    }

    ecs_flags32_t up_fields = it->up_fields;
    if (!redo) {
        if (up_fields & (and_fields|not_fields)) {
            /* If there are toggle fields that were matched with query 
             * traversal, evaluate those separately. */
            if (!flecs_query_toggle_for_up(it, and_fields, not_fields)) {
                return false;
            }

            it->set_fields &= (ecs_termset_t)~(not_fields & up_fields);
        }
    }

    /* Shared fields are evaluated, can be ignored from now on */
    // and_fields &= ~up_fields;
    not_fields &= ~up_fields;

    if (!(table->flags & EcsTableHasToggle)) {
        if (not_fields) {
            /* If any of the toggle fields with a not operator are for fields
             * that are set, without a bitset those fields can't match. */
            return false;
        } else {
            /* If table doesn't have toggles but query matched toggleable 
             * components, all entities match. */
            if (!redo) {            
                return true;
            } else {
                return false;
            }
        }
    }

    if (table && !range.count) {
        range.count = ecs_table_count(table);
        if (!range.count) {
            return false;
        }
    }

    int32_t i, j;
    int32_t first, last, block_index, cur;
    uint64_t block = 0;
    if (!redo) {
        op_ctx->range = range;
        cur = op_ctx->cur = range.offset;
        block_index = op_ctx->block_index = -1;
        first = range.offset;
        last = range.offset + range.count;
    } else {
        if (!op_ctx->has_bitset) {
            goto done;
        }

        last = op_ctx->range.offset + op_ctx->range.count;
        cur = op_ctx->cur;
        ecs_assert(cur <= last, ECS_INTERNAL_ERROR, NULL);
        if (cur == last) {
            goto done;
        }

        first = cur;
        block_index = op_ctx->block_index;
        block = op_ctx->block;
    }

    /* If end of last iteration is start of new block, compute new block */    
    int32_t new_block_index = cur / 64, row = first;
    if (new_block_index != block_index) {
compute_block:
        block_index = op_ctx->block_index = new_block_index;

        flecs_query_row_mask_t row_mask = flecs_query_get_row_mask(
            it, table, block_index, and_fields, not_fields, op_ctx);

        /* If table doesn't have bitset columns, all columns match */
        if (!(op_ctx->has_bitset = row_mask.has_bitset)) {
            if (!not_fields) {
                return true;
            } else {
                goto done;
            }
        }

        /* No enabled bits */
        block = row_mask.mask;
        if (!block) {
next_block:
            new_block_index ++;
            cur = new_block_index * 64;
            if (cur >= last) {
                /* No more rows */
                goto done;
            }

            op_ctx->cur = cur;
            goto compute_block;
        }

        op_ctx->block = block;
    }

    /* Find first enabled bit (TODO: use faster bitmagic) */
    int32_t first_bit = cur - (block_index * 64);
    int32_t last_bit = ECS_MIN(64, last - (block_index * 64));
    ecs_assert(first_bit >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(first_bit < 64, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit <= 64, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit >= first_bit, ECS_INTERNAL_ERROR, NULL);

    for (i = first_bit; i < last_bit; i ++) {
        uint64_t bit = (1ull << i);
        bool cond = 0 != (block & bit);
        if (cond) {
            /* Find last enabled bit */
            for (j = i; j < last_bit; j ++) {
                bit = (1ull << j);
                cond = !(block & bit);
                if (cond) {
                    break;
                }
            }

            row = i + (block_index * 64);
            cur = j + (block_index * 64);
            break;
        }
    }

    if (i == last_bit) {
        goto next_block;
    }

    ecs_assert(row >= first, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur <= last, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur >= first, ECS_INTERNAL_ERROR, NULL);

    if (!(cur - row)) {
        goto done;
    }

    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        flecs_query_var_narrow_range(op->src.var, table, row, cur - row, ctx);
    }
    op_ctx->cur = cur;

    return true;

done:
    /* Restore range & set fields */
    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        flecs_query_var_narrow_range(op->src.var, 
            table, op_ctx->range.offset, op_ctx->range.count, ctx);
    }

    it->set_fields = op_ctx->prev_set_fields;
    return false;
}

bool flecs_query_toggle(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    if (!redo) {
        op_ctx->prev_set_fields = it->set_fields;
    }

    ecs_flags64_t and_fields = op->first.entity;
    ecs_flags64_t not_fields = op->second.entity & op_ctx->prev_set_fields;

    return flecs_query_toggle_cmp(
        op, redo, ctx, and_fields, not_fields);
}

bool flecs_query_toggle_option(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    if (!redo) {
        op_ctx->prev_set_fields = it->set_fields;
        op_ctx->optional_not = false;
        op_ctx->has_bitset = false;
    }

repeat: {}
    ecs_flags64_t and_fields = 0, not_fields = 0;
    if (op_ctx->optional_not) {
        not_fields = op->first.entity & op_ctx->prev_set_fields;
    } else {
        and_fields = op->first.entity;
    }

    bool result = flecs_query_toggle_cmp(
        op, redo, ctx, and_fields, not_fields);
    if (!result) {
        if (!op_ctx->optional_not) {
            /* Run the not-branch of optional fields */
            op_ctx->optional_not = true;
            it->set_fields = op_ctx->prev_set_fields;
            redo = false;
            goto repeat;
        }
    }

    return result;
}

