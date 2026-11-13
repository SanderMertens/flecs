/**
 * @file addons/script/expr/visit_eval.c
 * @brief Script expression evaluation visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"
#include <inttypes.h>

typedef struct ecs_script_eval_ctx_t {
    const ecs_script_t *script;
    ecs_world_t *world;
    const ecs_expr_eval_desc_t *desc;
    ecs_expr_stack_t *stack;
} ecs_script_eval_ctx_t;

static int flecs_expr_visit_eval_priv(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_expr_value_t *out);

static inline ecs_expr_value_t* flecs_expr_eval_result(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_expr_value_t *literal)
{
    if (node->kind == EcsExprValue) {
        literal->value = (ecs_value_t){
            .type = node->type,
            .ptr = ((ecs_expr_value_node_t*)node)->ptr
        };
        literal->type_info = node->type_info;
        literal->owned = false;
        return literal;
    }

    ecs_expr_value_t *result = flecs_expr_stack_result(ctx->stack, node);
    return flecs_expr_visit_eval_priv(ctx, node, result) ? NULL : result;
}

static int flecs_expr_value_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_value_node_t *node,
    ecs_expr_value_t *out)
{
    (void)ctx;
    out->value.type = node->node.type;
    out->value.ptr = node->ptr;
    out->owned = false;
    return 0;
}

static int flecs_expr_interpolated_fragment_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *expr,
    ecs_expr_format_t *format,
    ecs_strbuf_t *buf)
{
    ecs_expr_value_t *val = flecs_expr_eval_result(
        ctx, expr, &(ecs_expr_value_t){0});
    if (!val) {
        return -1;
    }

    if (format->is_present) {
        int32_t width = 0;
        int32_t precision = -1;
        ecs_expr_node_t *format_exprs[2] = {
            format->width, format->precision
        };
        int32_t *format_values[2] = { &width, &precision };
        int32_t f;
        for (f = 0; f < 2; f ++) {
            if (!format_exprs[f]) {
                continue;
            }

            ecs_expr_value_t *format_val = flecs_expr_eval_result(
                ctx, format_exprs[f], &(ecs_expr_value_t){0});
            if (!format_val) {
                return -1;
            }

            ecs_assert(format_val->value.type == ecs_id(ecs_i32_t),
                ECS_INTERNAL_ERROR, NULL);
            format_values[f][0] = *(int32_t*)format_val->value.ptr;
        }

        return flecs_expr_format_value(ctx->script, expr, &val->value,
            format, width, precision, buf);
    }

    if (val->value.type != ecs_id(ecs_string_t)) {
        return ecs_ptr_to_expr_buf(
            ctx->world, val->value.type, val->value.ptr, buf);
    }

    const char *str = *(char**)val->value.ptr;
    if (str) {
        ecs_strbuf_appendstr(buf, str);
    }

    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_interpolated_string_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_interpolated_string_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(node->node.type == ecs_id(ecs_string_t), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    int32_t i, count = ecs_vec_count(&node->fragments);
    ecs_expr_fragment_t *fragments = ecs_vec_first(&node->fragments);
    for (i = 0; i < count; i ++) {
        char *fragment = fragments[i].text;
        if (fragment) {
            ecs_strbuf_appendstr(&buf, fragment);
        }

        if (fragments[i].expr) {
            flecs_expr_stack_push(ctx->stack);

            int result = flecs_expr_interpolated_fragment_eval(
                ctx, fragments[i].expr, &fragments[i].format, &buf);

            flecs_expr_stack_pop(ctx->stack);

            if (result) {
                ecs_strbuf_reset(&buf);
                return -1;
            }
        }
    }

    *(char**)out->value.ptr = ecs_strbuf_get(&buf);
    out->owned = true;
    return 0;
}

static int flecs_expr_initializer_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value,
    ecs_meta_cursor_t *cur,
    ecs_size_t value_size);

static void flecs_expr_value_move(
    void *dst,
    void *src,
    const ecs_type_info_t *ti)
{
    if (ti->hooks.dtor) {
        flecs_type_info_move_dtor(dst, src, 1, ti);
        flecs_type_info_ctor(src, 1, ti);
    } else {
        flecs_type_info_move(dst, src, 1, ti);
    }
}

static int flecs_expr_initializer_eval_static(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value,
    ecs_size_t value_size)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_assert(elem->value != NULL, ECS_INTERNAL_ERROR, NULL);

        if (i) {
            flecs_expr_stack_pop(ctx->stack);
            flecs_expr_stack_push(ctx->stack);
        }

        if (elem->value->kind == EcsExprInitializer) {
            ecs_expr_initializer_t *inner =
                (ecs_expr_initializer_t*)elem->value;
            void *inner_value = value;
            ecs_size_t inner_size = value_size;
            if (inner->is_dynamic) {
                inner_value = ECS_OFFSET(value, elem->offset);
                inner_size = inner->node.type_info->size;
            }

            if (flecs_expr_initializer_eval(ctx,
                inner, inner_value, NULL, inner_size))
            {
                goto error;
            }

            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(elem->value);
        if (swizzle) {
            ecs_expr_value_t *left = flecs_expr_eval_result(
                ctx, swizzle->left, &(ecs_expr_value_t){0});
            if (!left) {
                goto error;
            }

            ecs_size_t size = swizzle->elem_size;
            int32_t s;
            for (s = 0; s < swizzle->count; s ++) {
                uintptr_t offset = elem->offset + swizzle->dst[s];
                if (flecs_expr_initializer_check(ctx->script, node,
                    swizzle->node.type, offset, size,
                    flecs_uto(uintptr_t, value_size)))
                {
                    goto error;
                }

                ecs_os_memcpy(ECS_OFFSET(value, offset),
                    ECS_OFFSET(left->value.ptr, swizzle->src[s]), size);
            }

            continue;
        }

        ecs_expr_value_t *expr = flecs_expr_eval_result(
            ctx, elem->value, &(ecs_expr_value_t){0});
        if (!expr) {
            goto error;
        }

        /* Type is guaranteed to be correct, since type visitor will insert
         * a cast to the type of the initializer element. */
        ecs_entity_t type = elem->value->type;
        if (flecs_expr_initializer_check(ctx->script, node, type,
            elem->offset, elem->value->type_info->size,
            flecs_uto(uintptr_t, value_size)))
        {
            goto error;
        }

        if (expr->owned) {
            flecs_expr_value_move(ECS_OFFSET(value, elem->offset),
                expr->value.ptr, elem->value->type_info);
        } else {
            if (ecs_ptr_copy(ctx->world, type, 
                ECS_OFFSET(value, elem->offset), expr->value.ptr))
            {
                goto error;
            }
        }
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static int flecs_expr_initializer_eval_dynamic(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value,
    ecs_meta_cursor_t *cur,
    ecs_size_t value_size)
{
    (void)value_size;

    flecs_expr_stack_push(ctx->stack);

    ecs_meta_cursor_t local_cur;
    if (!cur) {
        local_cur = ecs_meta_cursor(
            ctx->world, node->node.type, value);
        cur = &local_cur;
    }
    
    if (ecs_meta_push(cur)) {
        goto error;
    }

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_assert(elem->value != NULL, ECS_INTERNAL_ERROR, NULL);

        if (i) {
            flecs_expr_stack_pop(ctx->stack);
            flecs_expr_stack_push(ctx->stack);
            if (ecs_meta_next(cur)) {
                goto error;
            }
        }

        if (elem->key) {
            ecs_expr_value_t *key = flecs_expr_eval_result(
                ctx, elem->key, &(ecs_expr_value_t){0});
            if (!key) {
                goto error;
            }

            if (ecs_meta_key(cur, &key->value)) {
                goto error;
            }
        } else if (elem->member) {
            if (ecs_meta_member(cur, elem->member)) {
                goto error;
            }
        }

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_eval_dynamic(ctx,
                (ecs_expr_initializer_t*)elem->value, value, cur, value_size))
            {
                goto error;
            }

            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(elem->value);
        if (swizzle) {
            ecs_expr_value_t *left = flecs_expr_eval_result(
                ctx, swizzle->left, &(ecs_expr_value_t){0});
            if (!left) {
                goto error;
            }

            if (flecs_expr_swizzle_set(cur, swizzle, left->value.ptr)) {
                goto error;
            }

            continue;
        }

        ecs_expr_value_t *expr = flecs_expr_eval_result(
            ctx, elem->value, &(ecs_expr_value_t){0});
        if (!expr) {
            goto error;
        }

        ecs_value_t v_elem_value = {
            .ptr = expr->value.ptr,
            .type = expr->value.type
        };

        if (ecs_meta_set_value(cur, &v_elem_value)) {
            goto error;
        }
    }

    if (ecs_meta_pop(cur)) {
        goto error;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static int flecs_expr_initializer_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value,
    ecs_meta_cursor_t *cur,
    ecs_size_t value_size)
{
    if (node->is_dynamic) {
        return flecs_expr_initializer_eval_dynamic(
            ctx, node, value, cur, value_size);
    } else {
        return flecs_expr_initializer_eval_static(ctx, node, value, value_size);
    }
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_initializer_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    ecs_expr_value_t *out)
{
    out->owned = true;

    const ecs_type_info_t *ti = ecs_get_type_info(
        ctx->world, node->node.type);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_expr_initializer_eval(
        ctx, node, out->value.ptr, NULL, ti->size);
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_empty_initializer_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    ecs_expr_value_t *out)
{
    if (node->is_dynamic) {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            ctx->world, node->node.type, out->value.ptr);
        
        if (ecs_meta_push(&cur)) {
            return -1;
        }

        if (ecs_meta_pop(&cur)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_expr_unary_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_unary_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->expr, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    return flecs_value_unary(
        ctx->script, &expr->value, &out->value, node->operator);
}

static int flecs_expr_binary_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_binary_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *left = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!left) {
        return -1;
    }

    if (!node->vector_count && left->value.type == ecs_id(ecs_bool_t)) {
        if (node->operator == EcsTokAnd || node->operator == EcsTokOr) {
            bool lval = *(bool*)left->value.ptr;
            if ((node->operator == EcsTokAnd) != lval) {
                *(bool*)out->value.ptr = lval;
                return 0;
            }
        }
    }

    ecs_expr_value_t *right = flecs_expr_eval_result(
        ctx, node->right, &(ecs_expr_value_t){0});
    if (!right) {
        return -1;
    }

    return flecs_expr_binary_eval(ctx->script, node, &left->value,
        &right->value, out->value.ptr, node->operator);
}

static int flecs_expr_identifier_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_identifier_t *node,
    ecs_expr_value_t *out)
{
    if (node->expr) {
        return flecs_expr_visit_eval_priv(ctx, node->expr, out);
    }

    ecs_entity_t entity = 0;
    if (node->symbol != -1) {
        ecs_script_eval_visitor_t *v = ctx->desc
            ? ctx->desc->script_visitor : NULL;
        ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);
        entity = flecs_script_symbol_entity(v, node->symbol);
    } else {
        flecs_script_symbol_t symbol;
        if (!flecs_script_symbol_lookup(ctx->script, ctx->desc, 0,
            node->value, FlecsScriptLookupEntity | FlecsScriptLookupDynamic,
            &symbol))
        {
            entity = symbol.entity;
        }
    }

    return flecs_expr_identifier_set(ctx->script, node, entity, &out->value);
}

static int flecs_expr_variable_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_variable_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(ctx->desc != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");
    ecs_assert(ctx->desc->vars != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");
    ecs_assert(!ctx->desc->script_visitor ||
        ctx->desc->disable_dynamic_variable_binding,
        ECS_INTERNAL_ERROR, NULL);

    const ecs_script_var_t *var;
    if (ctx->desc->disable_dynamic_variable_binding) {
        var = ecs_script_vars_from_sp(ctx->desc->vars, node->sp);
    } else {
        flecs_script_symbol_t symbol;
        if (flecs_script_symbol_lookup(ctx->script, ctx->desc, 0,
            node->name, FlecsScriptLookupVariable | FlecsScriptLookupDynamic,
            &symbol))
        {
            symbol.sp = -1;
        }

        var = symbol.sp == -1 ? NULL : ecs_script_vars_from_sp(
            ctx->desc->vars, symbol.sp);
    }

    var = flecs_expr_var_check(ctx->script, node, var);
    if (!var) {
        return -1;
    }

    out->value = var->value;
    out->owned = false;
    return 0;
}

static int flecs_expr_global_variable_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_variable_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(ctx->desc != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");
    ecs_value_t value = flecs_script_global_var_get(
        ctx->world, node->global, NULL);
    ecs_assert(value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    out->value = value;
    out->owned = false;

    return 0;
}

static int flecs_expr_cast_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_cast_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->expr, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    int32_t src_count, dst_count;
    if (flecs_value_array_count_mismatch(ctx->world, out->value.type,
        &expr->value, &src_count, &dst_count))
    {
        flecs_expr_visit_error(ctx->script, node,
            "cannot assign %d elements to array of %d elements",
            src_count, dst_count);
        return -1;
    }

    if (flecs_value_copy_to(ctx->world, &out->value, expr)) {
        flecs_expr_visit_error(ctx->script, node, "failed to cast value");
        return -1;
    }

    out->owned = true;
    return 0;
}

static int flecs_expr_cast_number_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_cast_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->expr, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    node->cast(out->value.ptr, expr->value.ptr);
    return 0;
}

static int flecs_expr_function_args_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    ecs_value_t *args)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_expr_value_t *expr = flecs_expr_eval_result(
            ctx, elem->value, &(ecs_expr_value_t){0});
        if (!expr) {
            return -1;
        }

        args[i] = expr->value;
    }

    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_function_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_function_t *node,
    ecs_expr_value_t *out)
{
    bool method = node->node.kind == EcsExprMethod;
    if (method && !node->left) {
        return 0;
    }

    ecs_value_t receiver = {0};
    if (method) {
        ecs_expr_value_t *expr = flecs_expr_eval_result(
            ctx, node->left, &(ecs_expr_value_t){0});
        if (!expr) {
            return -1;
        }

        receiver = expr->value;
    }

    ecs_assert(out->value.ptr != NULL || !node->node.type,
        ECS_INTERNAL_ERROR, NULL);

    ecs_value_t *argv = NULL;
    int32_t argc = ecs_vec_count(&node->args->elements);
    if (argc || method) {
        argv = ecs_os_alloca_n(ecs_value_t, argc + method);
        if (method) {
            argv[0] = receiver;
        }

        if (flecs_expr_function_args_visit_eval(ctx, node->args, argv + method)) {
            return -1;
        }
    }

    if (flecs_expr_function_call(ctx->script, node, argc, argv, &out->value)) {
        return -1;
    }

    out->owned = true;
    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_template_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_function_t *node,
    ecs_expr_value_t *out)
{
    ecs_world_t *world = ctx->world;
    ecs_entity_t tmpl = node->calldata.function;
    bool is_ref = flecs_type_is_value(world, node->node.type);
    bool has_args = node->args && ecs_vec_count(&node->args->elements) != 0;
    bool has_type = node->args && node->args->node.type_info != NULL;
    void *value = NULL;

    ecs_assert(out->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_expr_template_ref_var(world, node->left)) {
        ecs_expr_value_t *ref_value = flecs_expr_eval_result(
            ctx, node->left, &(ecs_expr_value_t){0});
        if (!ref_value) {
            return -1;
        }

        const ecs_value_t *ref = ref_value->value.ptr;
        value = flecs_expr_template_ref_value(ctx->script, node, ref);
        if (!value) {
            return -1;
        }

        tmpl = ref->type;
    } else if (has_args || has_type || !is_ref) {
        value = ecs_ptr_new(world, tmpl);
        if (!value) {
            flecs_expr_visit_error(ctx->script, node,
                "failed to create value for template '%s'",
                node->function_name);
            return -1;
        }
    }

    if (has_args) {
        ecs_expr_value_t args_out = {
            .value = { .type = node->args->node.type, .ptr = value },
            .type_info = node->args->node.type_info,
            .owned = false
        };

        if (flecs_expr_visit_eval_priv(
            ctx, (ecs_expr_node_t*)node->args, &args_out))
        {
            ecs_ptr_free(world, tmpl, value);
            return -1;
        }
    }

    if (is_ref) {
        flecs_script_template_ref_set(world, out->value.ptr, tmpl, value);
    } else {
        ecs_ptr_move(world, tmpl, out->value.ptr, value);
        ecs_ptr_free(world, tmpl, value);
    }

    out->owned = true;
    return 0;
}

static int flecs_expr_member_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_member_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    out->value.ptr = ECS_OFFSET(expr->value.ptr, node->offset);
    out->value.type = node->node.type;
    out->owned = false;

    return 0;
}

static int flecs_expr_swizzle_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_swizzle_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(!node->expand, ECS_INTERNAL_ERROR, NULL);

    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    ecs_size_t size = node->elem_size;
    int32_t i;
    for (i = 0; i < node->count; i ++) {
        ecs_os_memcpy(ECS_OFFSET(out->value.ptr, i * size),
            ECS_OFFSET(expr->value.ptr, node->src[i]), size);
    }

    out->value.type = node->node.type;
    out->owned = false;
    return 0;
}

static int flecs_expr_element_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_element_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    ecs_expr_value_t *index = flecs_expr_eval_result(
        ctx, node->index, &(ecs_expr_value_t){0});
    if (!index) {
        return -1;
    }

    const EcsType *type = ecs_get(ctx->world, expr->value.type, EcsType);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_expr_element_get(ctx->script, node, type->kind,
        &expr->value, &index->value, &out->value.ptr))
    {
        return -1;
    }

    out->value.type = node->node.type;
    out->owned = false;
    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_range_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_range_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *from = flecs_expr_eval_result(
        ctx, node->from, &(ecs_expr_value_t){0});
    if (!from) {
        return -1;
    }

    ecs_expr_value_t *to = flecs_expr_eval_result(
        ctx, node->to, &(ecs_expr_value_t){0});
    if (!to) {
        return -1;
    }

    if (flecs_expr_range_eval(ctx->world, node->node.type,
        *(int32_t*)from->value.ptr, *(int32_t*)to->value.ptr,
        out->value.ptr))
    {
        return -1;
    }

    out->owned = true;
    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_match_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_match_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_eval_result(
        ctx, node->expr, &(ecs_expr_value_t){0});
    if (!expr) {
        return -1;
    }

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];

        flecs_expr_stack_push(ctx->stack);
        ecs_expr_value_t *compare = flecs_expr_eval_result(
            ctx, elem->compare, &(ecs_expr_value_t){0});

        bool value = false;
        ecs_value_t result = { .type = ecs_id(ecs_bool_t), .ptr = &value };

        int r = !compare || flecs_value_binary(
            ctx->script, &node->node, &expr->value, &compare->value, &result,
            EcsTokEq);

        flecs_expr_stack_pop(ctx->stack);

        if (r) {
            return -1;
        }

        if (value) {
            break;
        }
    }

    while (i < count && !elems[i].expr) {
        i ++;
    }

    ecs_expr_node_t *branch = i < count ? elems[i].expr : node->any.expr;
    if (!branch) {
        char *str = ecs_ptr_to_str(
            ctx->world, expr->value.type, expr->value.ptr);
        flecs_expr_visit_error(ctx->script, node,
            "match value '%s' not handled by case", str);
        ecs_os_free(str);
        return -1;
    }

    ecs_expr_value_t *result = flecs_expr_eval_result(
        ctx, branch, &(ecs_expr_value_t){0});
    if (!result) {
        return -1;
    }

    ecs_assert(out->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    if (result->owned) {
        flecs_expr_value_move(out->value.ptr, result->value.ptr,
            node->node.type_info);
    } else {
        flecs_type_info_copy(out->value.ptr, result->value.ptr, 1,
            node->node.type_info);
    }

    return 0;
}

static int flecs_expr_new_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_new_t *node,
    ecs_expr_value_t *out)
{
    ecs_script_eval_visitor_t *v = ctx->desc ? ctx->desc->script_visitor : NULL;

    if (v && v->template) {
        *(ecs_entity_t*)out->value.ptr = 0;
        out->value.type = ecs_id(ecs_entity_t);
        return 0;
    }

    ecs_entity_t result = 0;
    if (flecs_script_eval_entity(v, ctx->script,
        ctx->desc ? ctx->desc->vars : NULL, node->entity, &result))
    {
        return -1;
    }

    *(ecs_entity_t*)out->value.ptr = result;
    out->value.type = ecs_id(ecs_entity_t);

    return 0;
}

static int flecs_expr_script_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_script_t *node,
    ecs_expr_value_t *out)
{
    ecs_script_eval_visitor_t *v = ctx->desc ? ctx->desc->script_visitor : NULL;
    if (v && v->template) {
        *(ecs_entity_t*)out->value.ptr = 0;
        out->value.type = ecs_id(ecs_entity_t);
        return 0;
    }

    *(ecs_entity_t*)out->value.ptr = flecs_expr_script_new(
        ctx->world, node->script);
    out->value.type = ecs_id(ecs_entity_t);
    return 0;
}

static void flecs_expr_iterate_assign(
    void *dst,
    ecs_expr_value_t *src,
    const ecs_type_info_t *ti)
{
    if (src->owned) {
        flecs_expr_value_move(dst, src->value.ptr, ti);
    } else {
        flecs_type_info_copy(dst, src->value.ptr, 1, ti);
    }
}

static void* flecs_expr_iterate_append(
    bool is_map,
    void *result,
    ecs_map_key_t key,
    const ecs_type_info_t *ti)
{
    if (!is_map) {
        ecs_vec_t *vec = result;
        ecs_vec_init_if(vec, ti->size);
        int32_t count = ecs_vec_count(vec);
        ecs_vec_set_count_w_type_info(NULL, vec, ti->size, count + 1, ti);
        return ecs_vec_get(vec, ti->size, count);
    }

    ecs_map_t *map = result;
    ecs_map_init_if(map, NULL);
    int32_t count = ecs_map_count(map);
    ecs_map_val_t *val = ecs_map_ensure(map, key);
    void *ptr;
    if (ti->size <= ECS_SIZEOF(ecs_map_val_t)) {
        ptr = val;
    } else {
        if (!*val) {
            *val = (ecs_map_val_t)(uintptr_t)ecs_os_calloc(ti->size);
        }

        ptr = (void*)(uintptr_t)*val;
    }

    if (count != ecs_map_count(map) && ti->hooks.ctor) {
        ti->hooks.ctor(ptr, 1, ti);
    }

    return ptr;
}

static int flecs_expr_iterate_step(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_iterate_t *node,
    ecs_expr_value_t *val,
    void *elem,
    const ecs_type_info_t *elem_ti,
    ecs_map_key_t key,
    void *result,
    bool to_map,
    bool *has_value)
{
    const ecs_type_info_t *result_ti = node->node.type_info;
    ecs_value_t acc = { .type = node->node.type, .ptr = result };

    switch(node->kind) {
    case EcsExprIterateMap:
    case EcsExprIterateMapValues:
        flecs_expr_iterate_assign(flecs_expr_iterate_append(
            to_map, result, key, val->type_info), val, val->type_info);
        return 0;
    case EcsExprIterateFilter:
        if (*(bool*)val->value.ptr) {
            flecs_type_info_copy(flecs_expr_iterate_append(
                to_map, result, key, elem_ti), elem, 1, elem_ti);
        }
        return 0;
    case EcsExprIterateReduce:
        if (val->value.ptr != result) {
            flecs_expr_iterate_assign(result, val, result_ti);
        }
        return 0;
    case EcsExprIterateSum:
        return flecs_value_binary(ctx->script, &node->node, &acc,
            &val->value, &acc, EcsTokAdd);
    case EcsExprIterateMin:
    case EcsExprIterateMax: {
        bool replace = !*has_value;
        if (!replace) {
            ecs_value_t cmp = { .type = ecs_id(ecs_bool_t), .ptr = &replace };
            if (flecs_value_binary(ctx->script, &node->node, &val->value,
                &acc, &cmp, node->kind == EcsExprIterateMin
                    ? EcsTokLt : EcsTokGt))
            {
                return -1;
            }
        }

        if (replace) {
            flecs_type_info_copy(result, val->value.ptr, 1, result_ti);
            *has_value = true;
        }

        return 0;
    }
    case EcsExprIterateAny:
        if (*(bool*)val->value.ptr) {
            *(bool*)result = true;
            return 1;
        }
        return 0;
    case EcsExprIterateAll:
        if (!*(bool*)val->value.ptr) {
            *(bool*)result = false;
            return 1;
        }
        return 0;
    case EcsExprIterateCount:
        if (*(bool*)val->value.ptr) {
            (*(int32_t*)result) ++;
        }
        return 0;
    case EcsExprIterateContains:
    case EcsExprIterateIndexOf:
    case EcsExprIterateFirst:
    case EcsExprIterateLast:
    case EcsExprIterateReverse:
    case EcsExprIterateSort:
        return 0;
    }

    return 0;
}

static void flecs_expr_sort_indices(
    int32_t *indices,
    int32_t count,
    const void *keys,
    const ecs_type_info_t *key_ti)
{
    if (count < 2) {
        return;
    }

    ecs_size_t size = key_ti->size;
    int32_t *tmp = ecs_os_malloc_n(int32_t, count);
    int32_t width, i;
    for (width = 1; width < count; width *= 2) {
        for (i = 0; i < count; i += 2 * width) {
            int32_t l = i, l_end = i + width, r = i + width;
            int32_t r_end = i + 2 * width, out = i;
            if (l_end > count) {
                l_end = count;
            }
            if (r > count) {
                r = count;
            }
            if (r_end > count) {
                r_end = count;
            }

            while (l < l_end && r < r_end) {
                if (flecs_type_info_cmp(ECS_ELEM(keys, size, indices[r]),
                    ECS_ELEM(keys, size, indices[l]), key_ti) < 0)
                {
                    tmp[out ++] = indices[r ++];
                } else {
                    tmp[out ++] = indices[l ++];
                }
            }

            while (l < l_end) {
                tmp[out ++] = indices[l ++];
            }

            while (r < r_end) {
                tmp[out ++] = indices[r ++];
            }
        }

        ecs_os_memcpy_n(indices, tmp, int32_t, count);
    }

    ecs_os_free(tmp);
}

static void flecs_expr_sort_collection(
    ecs_vec_t *result,
    const void *elems,
    int32_t count,
    const ecs_type_info_t *elem_ti,
    const void *keys,
    const ecs_type_info_t *key_ti)
{
    int32_t *indices = ecs_os_malloc_n(int32_t, count ? count : 1);
    int32_t i;
    for (i = 0; i < count; i ++) {
        indices[i] = i;
    }

    flecs_expr_sort_indices(indices, count, keys, key_ti);

    ecs_vec_init_if(result, elem_ti->size);
    flecs_value_vec_splice(elem_ti, result, 0, ecs_vec_count(result), NULL, 0);
    for (i = 0; i < count; i ++) {
        flecs_value_vec_splice(elem_ti, result, i, i,
            ECS_ELEM(elems, elem_ti->size, indices[i]), 1);
    }

    ecs_os_free(indices);
}

static int flecs_expr_iterate_collection_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_iterate_t *node,
    ecs_expr_value_t *left,
    ecs_expr_value_t *out)
{
    ecs_world_t *world = ctx->world;
    void *result = out->value.ptr;
    out->owned = true;

    ecs_expr_value_t init_val = {0};
    ecs_expr_value_t *init = NULL;
    if (node->init) {
        init = flecs_expr_eval_result(ctx, node->init, &init_val);
        if (!init) {
            return -1;
        }
    }

    if (node->collection_kind == EcsMapType) {
        ecs_assert(node->kind == EcsExprIterateContains,
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(init != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_map_key_t key = 0;
        if (flecs_value_blit_u64(world, &init->value, &key)) {
            return -1;
        }

        const ecs_map_t *map = left->value.ptr;
        *(bool*)result = ecs_map_is_init(map) && ecs_map_get(map, key) != NULL;
        return 0;
    }

    ecs_entity_t elem_type;
    const void *elems;
    int32_t i, count;
    if (!flecs_value_collection_get(world, left->value.type, left->value.ptr,
        &elem_type, &elems, &count))
    {
        return -1;
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, elem_type);
    ecs_assert(elem_ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_size_t size = elem_ti->size;

    switch(node->kind) {
    case EcsExprIterateContains:
    case EcsExprIterateIndexOf: {
        ecs_assert(init != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t found = -1;
        for (i = 0; i < count; i ++) {
            if (flecs_type_info_equals(
                ECS_ELEM(elems, size, i), init->value.ptr, elem_ti))
            {
                found = i;
                break;
            }
        }

        if (node->kind == EcsExprIterateContains) {
            *(bool*)result = found != -1;
        } else {
            *(int32_t*)result = found;
        }

        return 0;
    }
    case EcsExprIterateFirst:
    case EcsExprIterateLast:
        if (!count) {
            flecs_expr_visit_error(ctx->script, node,
                "cannot get '%s' of empty collection", node->function_name);
            return -1;
        }

        flecs_type_info_copy(result, ECS_ELEM(elems, size,
            node->kind == EcsExprIterateFirst ? 0 : count - 1), 1, elem_ti);
        return 0;
    case EcsExprIterateReverse: {
        ecs_vec_t *vec = result;
        ecs_vec_init_if(vec, size);
        flecs_value_vec_splice(elem_ti, vec, 0, ecs_vec_count(vec), NULL, 0);
        for (i = 0; i < count; i ++) {
            flecs_value_vec_splice(elem_ti, vec, i, i,
                ECS_ELEM(elems, size, count - 1 - i), 1);
        }

        return 0;
    }
    case EcsExprIterateSort:
        flecs_expr_sort_collection(
            result, elems, count, elem_ti, elems, elem_ti);
        return 0;
    case EcsExprIterateMap:
    case EcsExprIterateMapValues:
    case EcsExprIterateFilter:
    case EcsExprIterateReduce:
    case EcsExprIterateSum:
    case EcsExprIterateMin:
    case EcsExprIterateMax:
    case EcsExprIterateAny:
    case EcsExprIterateAll:
    case EcsExprIterateCount:
        break;
    }

    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_iterate_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_iterate_t *node,
    ecs_expr_value_t *out)
{
    ecs_world_t *world = ctx->world;
    ecs_expr_value_t *left = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!left) {
        return -1;
    }

    void *result = out->value.ptr;
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    if (node->kind >= EcsExprIterateContains &&
        (node->kind != EcsExprIterateSort || !node->lambda))
    {
        return flecs_expr_iterate_collection_eval(ctx, node, left, out);
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, node->elem_type);
    ecs_assert(elem_ti != NULL, ECS_INTERNAL_ERROR, NULL);
    bool is_map = node->collection_kind == EcsMapType;
    bool to_map = node->kind == EcsExprIterateMapValues ||
        (node->kind == EcsExprIterateFilter && is_map);

    if (node->init) {
        ecs_expr_value_t *init = flecs_expr_eval_result(
            ctx, node->init, &(ecs_expr_value_t){0});
        if (!init) {
            return -1;
        }

        flecs_expr_iterate_assign(result, init, node->node.type_info);
    }

    if (node->kind == EcsExprIterateAll) {
        *(bool*)result = true;
    }

    int32_t index = 0, count = 0;
    ecs_map_key_t key = 0;

    const ecs_expr_eval_desc_t *desc = ctx->desc;
    ecs_expr_eval_desc_t lambda_desc = {0};
    ecs_script_vars_t *vars = NULL;
    ecs_script_var_t *params = NULL;
    ecs_expr_lambda_t *lambda = node->lambda;

    if (lambda) {
        vars = flecs_expr_iterate_vars_push(
            world, node, desc ? desc->vars : NULL);
        ecs_assert(vars != NULL, ECS_INTERNAL_ERROR, NULL);
        params = ecs_vec_first(&vars->vars);
        ecs_assert(params != NULL, ECS_INTERNAL_ERROR, NULL);

        if (desc) {
            lambda_desc = *desc;
        }

        lambda_desc.vars = vars;
        ctx->desc = &lambda_desc;

        if (node->acc_var != -1) {
            params[node->acc_var].value.ptr = result;
        }

        if (node->key_var != -1) {
            params[node->key_var].value.ptr = is_map
                ? (void*)&key : (void*)&index;
        }
    }

    ecs_map_iter_t it = {0};
    void *elems = NULL;

    if (node->collection_kind == EcsArrayType) {
        const EcsArray *array = ecs_get(world, left->value.type, EcsArray);
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);
        elems = left->value.ptr;
        count = array->count;
    } else if (node->collection_kind == EcsVectorType) {
        ecs_vec_t *vec = left->value.ptr;
        if (vec) {
            elems = ecs_vec_first(vec);
            count = ecs_vec_count(vec);
        }
    } else if (left->value.ptr) {
        it = ecs_map_iter(left->value.ptr);
    }

    bool is_sort = node->kind == EcsExprIterateSort;
    ecs_assert(!is_sort || lambda != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *key_ti = is_sort ? lambda->node.type_info : NULL;
    ecs_vec_t keys = {0};
    if (is_sort) {
        ecs_vec_init(NULL, &keys, key_ti->size, count);
    }

    bool has_value = false;
    int r = 0;
    for (index = 0; ; index ++) {
        void *elem;
        if (is_map) {
            if (!left->value.ptr || !ecs_map_next(&it)) {
                break;
            }

            key = ecs_map_key(&it);
            elem = elem_ti->size > ECS_SIZEOF(ecs_map_val_t)
                ? ecs_map_ptr(&it)
                : (void*)&it.res[1];
        } else {
            if (index >= count) {
                break;
            }

            elem = ECS_ELEM(elems, elem_ti->size, index);
        }

        flecs_expr_stack_push(ctx->stack);

        ecs_expr_value_t elem_val = {
            .value = { .type = node->elem_type, .ptr = elem },
            .type_info = elem_ti,
            .owned = false
        };

        ecs_expr_value_t lambda_val = {0};
        ecs_expr_value_t *val = &elem_val;
        if (lambda) {
            params[node->elem_var].value.ptr = elem;
            val = flecs_expr_eval_result(
                ctx, lambda->body, &lambda_val);
        }

        if (val && is_sort) {
            flecs_value_vec_splice(key_ti, &keys, index, index,
                val->value.ptr, 1);
        }

        r = val ? flecs_expr_iterate_step(ctx, node, val, elem, elem_ti,
            key, result, to_map, &has_value) : -1;

        flecs_expr_stack_pop(ctx->stack);

        if (r) {
            break;
        }
    }

    ctx->desc = desc;
    if (vars) {
        ecs_script_vars_pop(vars);
    }

    if (r == 1) {
        r = 0;
    }

    if (is_sort) {
        if (!r) {
            flecs_expr_sort_collection(result, elems, count, elem_ti,
                ecs_vec_first(&keys), key_ti);
        }

        flecs_value_vec_splice(key_ti, &keys, 0, ecs_vec_count(&keys), NULL, 0);
        ecs_vec_fini(NULL, &keys, key_ti->size);
    }

    if (!r && !has_value && (node->kind == EcsExprIterateMin ||
        node->kind == EcsExprIterateMax))
    {
        flecs_expr_visit_error(ctx->script, node,
            "cannot compute '%s' of empty collection", node->function_name);
        r = -1;
    }

    out->owned = true;
    return r;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_slice_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_slice_t *node,
    ecs_expr_value_t *out)
{
    ecs_world_t *world = ctx->world;
    ecs_expr_value_t *left = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!left) {
        return -1;
    }

    ecs_entity_t elem_type;
    const void *elems;
    int32_t count;
    if (!flecs_value_collection_get(world, left->value.type, left->value.ptr,
        &elem_type, &elems, &count))
    {
        flecs_expr_visit_error(ctx->script, node, "cannot slice value");
        return -1;
    }

    int64_t from = 0, to = count;
    if (node->from) {
        ecs_expr_value_t *v = flecs_expr_eval_result(
            ctx, node->from, &(ecs_expr_value_t){0});
        if (!v) {
            return -1;
        }

        from = *(int64_t*)v->value.ptr;
    }

    if (node->to) {
        ecs_expr_value_t *v = flecs_expr_eval_result(
            ctx, node->to, &(ecs_expr_value_t){0});
        if (!v) {
            return -1;
        }

        to = *(int64_t*)v->value.ptr;
    }

    if (from < 0 || to < from || to > count) {
        flecs_expr_visit_error(ctx->script, node,
            "slice [%" PRId64 "..%" PRId64 "] is out of range for "
            "collection (count = %d)", from, to, count);
        return -1;
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, elem_type);
    ecs_assert(elem_ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_vec_t *vec = out->value.ptr;
    ecs_vec_init_if(vec, elem_ti->size);
    flecs_value_vec_splice(elem_ti, vec, 0, ecs_vec_count(vec),
        ECS_ELEM(elems, elem_ti->size, (int32_t)from), (int32_t)(to - from));

    out->owned = true;
    return 0;
}

typedef struct flecs_expr_mutate_ctx_t {
    const ecs_script_t *script;
    const ecs_expr_mutate_t *node;
    const ecs_value_t *args;
    int32_t arg_count;
    void *out;
} flecs_expr_mutate_ctx_t;

static int flecs_expr_mutate_action(
    ecs_script_eval_visitor_t *v,
    ecs_script_assign_t *assign,
    void *dst,
    void *ptr)
{
    (void)assign;
    flecs_expr_mutate_ctx_t *c = ptr;
    const ecs_expr_mutate_t *node = c->node;
    ecs_world_t *world = v->world;
    const ecs_type_info_t *elem_ti = ecs_get_type_info(world, node->elem_type);
    ecs_assert(elem_ti != NULL, ECS_INTERNAL_ERROR, NULL);

    if (node->is_map) {
        ecs_map_t *map = dst;
        if (node->kind == EcsExprMutateClear) {
            flecs_meta_map_clear(map, elem_ti);
            return 0;
        }

        ecs_map_key_t key = 0;
        if (flecs_value_blit_u64(world, &c->args[0], &key)) {
            return -1;
        }

        flecs_value_map_remove(elem_ti, map, key);
        return 0;
    }

    ecs_size_t size = elem_ti->size;
    ecs_vec_t *vec = dst;
    ecs_vec_init_if(vec, size);
    int32_t i, count = ecs_vec_count(vec);

    switch(node->kind) {
    case EcsExprMutatePush:
        for (i = 0; i < c->arg_count; i ++) {
            flecs_value_vec_splice(elem_ti, vec, count + i, count + i,
                c->args[i].ptr, 1);
        }
        break;
    case EcsExprMutateInsert: {
        int64_t index = *(int64_t*)c->args[0].ptr;
        if (index < 0 || index > count) {
            flecs_expr_visit_error(c->script, node,
                "cannot insert at index %" PRId64 " in vector '%s' with "
                "%d elements", index, node->name, count);
            return -1;
        }

        for (i = 1; i < c->arg_count; i ++) {
            int32_t at = (int32_t)index + i - 1;
            flecs_value_vec_splice(elem_ti, vec, at, at, c->args[i].ptr, 1);
        }
        break;
    }
    case EcsExprMutatePop:
        if (!count) {
            flecs_expr_visit_error(c->script, node,
                "cannot pop from empty vector '%s'", node->name);
            return -1;
        }

        flecs_type_info_copy(c->out, ecs_vec_get(vec, size, count - 1), 1,
            elem_ti);
        flecs_value_vec_splice(elem_ti, vec, count - 1, count, NULL, 0);
        break;
    case EcsExprMutateRemove: {
        int64_t index = *(int64_t*)c->args[0].ptr;
        if (index < 0 || index >= count) {
            flecs_expr_visit_error(c->script, node,
                "cannot remove index %" PRId64 " from vector '%s' with "
                "%d elements", index, node->name, count);
            return -1;
        }

        flecs_value_vec_splice(elem_ti, vec,
            (int32_t)index, (int32_t)index + 1, NULL, 0);
        break;
    }
    case EcsExprMutateClear:
        flecs_value_vec_splice(elem_ti, vec, 0, count, NULL, 0);
        break;
    }

    return 0;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_mutate_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_mutate_t *node,
    ecs_expr_value_t *out)
{
    ecs_script_eval_visitor_t *v = ctx->desc ? ctx->desc->script_visitor : NULL;
    if (!v) {
        flecs_expr_visit_error(ctx->script, node,
            "'%s' can only be called from an async block or a function",
            node->function_name);
        return -1;
    }

    ecs_script_assign_t *assign = node->assign;
    ecs_entity_t entity = 0;
    if (assign->entity) {
        ecs_expr_value_t *e = flecs_expr_eval_result(
            ctx, assign->entity, &(ecs_expr_value_t){0});
        if (!e) {
            return -1;
        }

        entity = *(ecs_entity_t*)e->value.ptr;
    }

    int64_t indices[FLECS_SCRIPT_ASSIGN_INDEX_MAX + 2];
    ecs_script_assign_step_t *steps = ecs_vec_first(&assign->steps);
    int32_t i, count = ecs_vec_count(&assign->steps), index_count = 0;
    for (i = assign->entity != NULL; i < count; i ++) {
        if (steps[i].member) {
            continue;
        }

        ecs_expr_value_t *index = flecs_expr_eval_result(
            ctx, steps[i].index, &(ecs_expr_value_t){0});
        if (!index) {
            return -1;
        }

        indices[index_count ++] = *(int64_t*)index->value.ptr;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &node->args->elements);
    int32_t arg_count = ecs_vec_count(&node->args->elements);
    ecs_value_t *args = NULL;
    if (arg_count) {
        args = ecs_os_alloca_n(ecs_value_t, arg_count);
        for (i = 0; i < arg_count; i ++) {
            ecs_expr_value_t *arg = flecs_expr_eval_result(
                ctx, elems[i].value, &(ecs_expr_value_t){0});
            if (!arg) {
                return -1;
            }

            args[i] = arg->value;
        }
    }

    flecs_expr_mutate_ctx_t mutate_ctx = {
        .script = ctx->script,
        .node = node,
        .args = args,
        .arg_count = arg_count,
        .out = out->value.ptr
    };

    if (node->kind != EcsExprMutatePop) {
        *(bool*)out->value.ptr = false;
    }

    out->owned = true;
    return flecs_script_assign_apply(v, assign, entity, indices,
        flecs_expr_mutate_action, &mutate_ctx);
}

static int flecs_expr_component_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_element_t *node,
    ecs_expr_value_t *out)
{
    ecs_script_eval_visitor_t *v = ctx->desc ? ctx->desc->script_visitor : NULL;
    if (v && v->template) {
        ecs_assert(out->value.type == node->node.type,
            ECS_INTERNAL_ERROR, NULL);
        flecs_expr_stack_storage(ctx->stack, out);
        out->owned = true;
        return 0;
    }

    ecs_expr_value_t *left = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!left) {
        return -1;
    }

    /* Left side of expression must be of entity type */
    ecs_assert(left->value.type == ecs_id(ecs_entity_t), 
        ECS_INTERNAL_ERROR, NULL);

    /* Component must be resolvable at parse time */
    ecs_expr_node_t *index = node->index;
    if (index->kind == EcsExprIdentifier) {
        index = ((ecs_expr_identifier_t*)index)->expr;
    }

    ecs_entity_t entity = *(ecs_entity_t*)left->value.ptr;
    ecs_entity_t component;
    if (index) {
        ecs_assert(index->kind == EcsExprValue, ECS_INTERNAL_ERROR, NULL);
        component = ((ecs_expr_value_node_t*)index)->storage.entity;
    } else {
        component = node->node.type;
    }

    ecs_assert(out->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);

    out->value.ptr = ECS_CONST_CAST(void*, flecs_expr_component_get(
        ctx->script, v, node, entity, component));
    out->owned = false;
    return out->value.ptr ? 0 : -1;
}

static int flecs_expr_has_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_has_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(out->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_eval_visitor_t *v = ctx->desc ? ctx->desc->script_visitor : NULL;
    if (v && v->template) {
        *(bool*)out->value.ptr = false;
        out->owned = true;
        return 0;
    }

    ecs_expr_value_t *left = flecs_expr_eval_result(
        ctx, node->left, &(ecs_expr_value_t){0});
    if (!left) {
        return -1;
    }

    ecs_assert(left->value.type == ecs_id(ecs_entity_t) ||
        left->value.type == ecs_id(ecs_id_t), ECS_INTERNAL_ERROR, NULL);

    *(bool*)out->value.ptr = flecs_expr_has_eval(
        ctx->script, v, node, *(ecs_entity_t*)left->value.ptr);
    out->owned = true;
    return 0;
}

static const bool flecs_expr_eval_has_frame[] = {
    [EcsExprUnary] = true,
    [EcsExprBinary] = true,
    [EcsExprFunction] = true,
    [EcsExprMethod] = true,
    [EcsExprTemplate] = true,
    [EcsExprSwizzle] = true,
    [EcsExprMatch] = true,
    [EcsExprRange] = true,
    [EcsExprCast] = true,
    [EcsExprCastNumber] = true,
    [EcsExprScript] = false,
    [EcsExprLambda] = false,
    [EcsExprIterate] = true,
    [EcsExprSlice] = true,
    [EcsExprMutate] = true
};

static int flecs_expr_visit_eval_priv(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    bool frame = flecs_expr_eval_has_frame[node->kind];
    if (frame) {
        flecs_expr_stack_push(ctx->stack);
    }

    int result = 0;
    switch(node->kind) {
    case EcsExprValue:
        result = flecs_expr_value_visit_eval(
            ctx, (ecs_expr_value_node_t*)node, out);
        break;
    case EcsExprInterpolatedString:
        result = flecs_expr_interpolated_string_visit_eval(
            ctx, (ecs_expr_interpolated_string_t*)node, out);
        break;
    case EcsExprEmptyInitializer:
        result = flecs_expr_empty_initializer_visit_eval(
            ctx, (ecs_expr_initializer_t*)node, out);
        break;
    case EcsExprInitializer:
        result = flecs_expr_initializer_visit_eval(
            ctx, (ecs_expr_initializer_t*)node, out);
        break;
    case EcsExprUnary:
        result = flecs_expr_unary_visit_eval(
            ctx, (ecs_expr_unary_t*)node, out);
        break;
    case EcsExprBinary:
        result = flecs_expr_binary_visit_eval(
            ctx, (ecs_expr_binary_t*)node, out);
        break;
    case EcsExprIdentifier:
        result = flecs_expr_identifier_visit_eval(
            ctx, (ecs_expr_identifier_t*)node, out);
        break;
    case EcsExprVariable:
        result = flecs_expr_variable_visit_eval(
            ctx, (ecs_expr_variable_t*)node, out);
        break;
    case EcsExprGlobalVariable:
        result = flecs_expr_global_variable_visit_eval(
            ctx, (ecs_expr_variable_t*)node, out);
        break;
    case EcsExprFunction:
    case EcsExprMethod:
        result = flecs_expr_function_visit_eval(
            ctx, (ecs_expr_function_t*)node, out);
        break;
    case EcsExprTemplate:
        result = flecs_expr_template_visit_eval(
            ctx, (ecs_expr_function_t*)node, out);
        break;
    case EcsExprMember:
        result = flecs_expr_member_visit_eval(
            ctx, (ecs_expr_member_t*)node, out);
        break;
    case EcsExprSwizzle:
        result = flecs_expr_swizzle_visit_eval(
            ctx, (ecs_expr_swizzle_t*)node, out);
        break;
    case EcsExprElement:
        result = flecs_expr_element_visit_eval(
            ctx, (ecs_expr_element_t*)node, out);
        break;
    case EcsExprMatch:
        result = flecs_expr_match_visit_eval(
            ctx, (ecs_expr_match_t*)node, out);
        break;
    case EcsExprRange:
        result = flecs_expr_range_visit_eval(
            ctx, (ecs_expr_range_t*)node, out);
        break;
    case EcsExprNew:
        result = flecs_expr_new_visit_eval(
            ctx, (ecs_expr_new_t*)node, out);
        break;
    case EcsExprScript:
        result = flecs_expr_script_visit_eval(
            ctx, (ecs_expr_script_t*)node, out);
        break;
    case EcsExprIterate:
        result = flecs_expr_iterate_visit_eval(
            ctx, (ecs_expr_iterate_t*)node, out);
        break;
    case EcsExprSlice:
        result = flecs_expr_slice_visit_eval(
            ctx, (ecs_expr_slice_t*)node, out);
        break;
    case EcsExprMutate:
        result = flecs_expr_mutate_visit_eval(
            ctx, (ecs_expr_mutate_t*)node, out);
        break;
    case EcsExprLambda:
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected lambda expression");
    case EcsExprComponent:
        result = flecs_expr_component_visit_eval(
            ctx, (ecs_expr_element_t*)node, out);
        break;
    case EcsExprHas:
        result = flecs_expr_has_visit_eval(
            ctx, (ecs_expr_has_t*)node, out);
        break;
    case EcsExprCast:
        result = flecs_expr_cast_visit_eval(
            ctx, (ecs_expr_cast_t*)node, out);
        break;
    case EcsExprCastNumber:
        result = flecs_expr_cast_number_visit_eval(
            ctx, (ecs_expr_cast_t*)node, out);
        break;
    }

    if (frame) {
        flecs_expr_stack_pop(ctx->stack);
    }

    return result;
}

static int flecs_expr_visit_eval_w_stack(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc,
    ecs_value_t *out,
    ecs_expr_stack_t *stack)
{
    flecs_expr_stack_push(stack);

    ecs_expr_value_t val_tmp;
    /* Initializers assign members of the existing value, so they are evaluated
     * straight into it. Other nodes produce a new value that is move assigned
     * to the output, so that the old value is destructed. An initializer typed
     * by a base struct can be evaluated straight into a value of a derived
     * struct: the base members are a prefix of the derived layout, so member
     * offsets are the same. */
    bool is_initializer = node->kind == EcsExprInitializer ||
        node->kind == EcsExprEmptyInitializer;
    bool direct = is_initializer && out->type && out->ptr &&
        (out->type == node->type || flecs_struct_is_derived_from(
            script->world, out->type, node->type));

    ecs_script_eval_ctx_t ctx = {
        .script = script,
        .world = script->world,
        .stack = stack,
        .desc = desc
    };

    int result = -1;
    ecs_expr_value_t *val = &val_tmp;
    if (!node->type) {
        val_tmp = (ecs_expr_value_t){0};
        result = flecs_expr_function_visit_eval(
            &ctx, (ecs_expr_function_t*)node, val);
        goto done;
    }

    if (direct) {
        val_tmp = (ecs_expr_value_t){
            .value = *out,
            .owned = false,
            .type_info = ecs_get_type_info(script->world, out->type)
        };

        if (flecs_expr_visit_eval_priv(&ctx, node, val)) {
            goto done;
        }
    } else {
        val = flecs_expr_eval_result(&ctx, node, &val_tmp);
        if (!val) {
            goto done;
        }
    }

    if (desc && !out->type) {
        out->type = desc->type;
    }

    if (!out->type) {
        out->type = node->type;
    }

    ecs_world_t *world = ECS_CONST_CAST(ecs_world_t*,
        ecs_get_world(ctx.world));
    if (out->type && !out->ptr) {
        out->ptr = ecs_ptr_new(world, out->type);
    }

    if (val != &val_tmp || out->ptr != val->value.ptr) {
        if (val->owned && out->type == val->value.type) {
            flecs_expr_value_move(out->ptr, val->value.ptr, val->type_info);
        } else if (flecs_value_copy_to(world, out, val)) {
            flecs_expr_visit_error(script, node, "failed to write to output");
            goto done;
        }
    }

    result = 0;
done:
    flecs_expr_stack_pop(stack);
    return result;
}

static FLECS_SCRIPT_NOINLINE int flecs_expr_visit_eval_w_local_stack(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc,
    ecs_value_t *out)
{
    ecs_expr_stack_t stack;
    flecs_expr_stack_init(&stack);
    int result = flecs_expr_visit_eval_w_stack(
        script, node, desc, out, &stack);
    flecs_expr_stack_fini(&stack);
    return result;
}

int flecs_expr_visit_eval(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc,
    ecs_value_t *out)
{
    if (desc && desc->runtime) {
        ecs_expr_stack_t *stack = flecs_script_runtime_expr_stack(
            desc->runtime);
        if (stack) {
            return flecs_expr_visit_eval_w_stack(
                script, node, desc, out, stack);
        }
    }

    return flecs_expr_visit_eval_w_local_stack(script, node, desc, out);
}

#endif
