/**
 * @file addons/script/ir/compile.c
 * @brief Compile script AST to IR.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"

typedef struct ecs_script_ir_compiler_t {
    ecs_script_impl_t *script;
    ecs_world_t *world;
    ecs_script_ir_t *ir;
    ecs_script_scope_t *scope;
    int32_t entry;
    int32_t reg_count;
    int32_t reg_floor;
    int32_t reg_max;
    int32_t force_depth;
} ecs_script_ir_compiler_t;

static int flecs_irc_compile_expr(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *node,
    int32_t dst,
    bool in_place);

static int flecs_irc_compile_scope(
    ecs_script_ir_compiler_t *c,
    ecs_script_scope_t *scope,
    uint16_t flags);

static int32_t flecs_irc_emit(
    ecs_script_ir_compiler_t *c,
    ecs_script_ir_op_kind_t kind,
    int32_t a,
    int32_t b,
    int32_t cc,
    const void *node)
{
    ecs_script_ir_op_t *op = ecs_vec_append_t(
        NULL, &c->ir->ops, ecs_script_ir_op_t);
    op->kind = (uint16_t)kind;
    op->flags = 0;
    op->a = a;
    op->b = b;
    op->c = cc;
    op->imm.u64 = 0;
    op->node = node;
    return ecs_vec_count(&c->ir->ops) - 1;
}

static ecs_script_ir_op_t* flecs_irc_op(
    ecs_script_ir_compiler_t *c,
    int32_t index)
{
    return ecs_vec_get_t(&c->ir->ops, ecs_script_ir_op_t, index);
}

static int32_t flecs_irc_pc(
    ecs_script_ir_compiler_t *c)
{
    return ecs_vec_count(&c->ir->ops);
}

static int32_t flecs_irc_reg(
    ecs_script_ir_compiler_t *c)
{
    int32_t reg = c->reg_count ++;
    if (c->reg_count > c->reg_max) {
        c->reg_max = c->reg_count;
    }
    return reg;
}

static int32_t flecs_irc_entry_add(
    ecs_script_ir_compiler_t *c,
    const void *node,
    ecs_script_ir_entry_kind_t kind)
{
    const uint64_t *existing = ecs_map_get(
        &c->ir->entry_index, (uintptr_t)node);
    if (existing) {
        return (int32_t)*existing;
    }
    int32_t count = ecs_vec_count(&c->ir->entries);
    ecs_script_ir_entry_t *entry = ecs_vec_append_t(
        NULL, &c->ir->entries, ecs_script_ir_entry_t);
    entry->node = node;
    entry->kind = kind;
    entry->pc = -1;
    entry->reg_count = 0;
    ecs_map_insert(&c->ir->entry_index, (uintptr_t)node, (uint64_t)count);
    return count;
}

static ecs_script_ir_num_class_t flecs_irc_num_class(
    ecs_entity_t type,
    int32_t *size)
{
    if (type == ecs_id(ecs_i8_t))  { *size = 1; return EcsIrNumSigned; }
    if (type == ecs_id(ecs_i16_t)) { *size = 2; return EcsIrNumSigned; }
    if (type == ecs_id(ecs_i32_t)) { *size = 4; return EcsIrNumSigned; }
    if (type == ecs_id(ecs_i64_t)) { *size = 8; return EcsIrNumSigned; }
    if (type == ecs_id(ecs_iptr_t)) {
        *size = ECS_SIZEOF(ecs_iptr_t); return EcsIrNumSigned;
    }
    if (type == ecs_id(ecs_u8_t))  { *size = 1; return EcsIrNumUnsigned; }
    if (type == ecs_id(ecs_u16_t)) { *size = 2; return EcsIrNumUnsigned; }
    if (type == ecs_id(ecs_u32_t)) { *size = 4; return EcsIrNumUnsigned; }
    if (type == ecs_id(ecs_u64_t)) { *size = 8; return EcsIrNumUnsigned; }
    if (type == ecs_id(ecs_uptr_t)) {
        *size = ECS_SIZEOF(ecs_uptr_t); return EcsIrNumUnsigned;
    }
    if (type == ecs_id(ecs_f32_t)) { *size = 4; return EcsIrNumFloat; }
    if (type == ecs_id(ecs_f64_t)) { *size = 8; return EcsIrNumFloat; }
    *size = 0;
    return EcsIrNumNone;
}

static uint64_t flecs_irc_num_pack(
    ecs_entity_t left,
    ecs_entity_t result)
{
    int32_t lsize = 0, rsize = 0;
    ecs_script_ir_num_class_t lclass = flecs_irc_num_class(left, &lsize);
    ecs_script_ir_num_class_t rclass = flecs_irc_num_class(result, &rsize);
    return (uint64_t)lclass | ((uint64_t)lsize << 8) |
        ((uint64_t)rclass << 16) | ((uint64_t)rsize << 24);
}

static int32_t flecs_irc_id(
    ecs_script_ir_compiler_t *c,
    ecs_script_id_t *id,
    void *node,
    bool is_component,
    int32_t component_slot)
{
    int32_t first_reg = -1, second_reg = -1;
    if (id->first_expr) {
        first_reg = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, id->first_expr, first_reg, false)) {
            return -1;
        }
    }
    if (id->second_expr) {
        second_reg = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, id->second_expr, second_reg, false)) {
            return -1;
        }
    }

    ecs_script_ir_id_t *desc = ecs_vec_append_t(
        NULL, &c->ir->ids, ecs_script_ir_id_t);
    ecs_os_zeromem(desc);
    desc->eval = id->eval;
    desc->flag = id->flag;
    desc->first_eval = id->first_eval;
    desc->second_eval = id->second_eval;
    desc->interface = id->interface;
    desc->first = id->first;
    desc->second = id->second;
    desc->first_symbol = id->first_symbol;
    desc->second_symbol = id->second_symbol;
    desc->first_sp = id->first_sp;
    desc->second_sp = id->second_sp;
    desc->first_reg = first_reg;
    desc->second_reg = second_reg;
    desc->value_sp = id->value_sp;
    desc->component_slot = component_slot;
    desc->resolved = id->eval != 0;
    desc->has_second = id->second != NULL;
    desc->dynamic = id->first_expr || id->second_expr ||
        id->first_symbol != -1 || id->second_symbol != -1 ||
        id->first_sp != -1 || id->second_sp != -1 || !id->eval;

    if (!desc->dynamic && id->eval && is_component) {
        ecs_component_record_t *cr = flecs_components_ensure(
            c->world, id->eval);
        if (cr && cr->type_info) {
            desc->ti = cr->type_info;
            desc->needs_set = desc->ti->hooks.on_replace != NULL;
        }
        if (!desc->has_second) {
            desc->visitor = ecs_has(c->world, id->eval, EcsScriptVisitor);
        }
    }

    (void)node;
    return ecs_vec_count(&c->ir->ids) - 1;
}

static bool flecs_irc_id_needs_expr(
    const ecs_script_id_t *id)
{
    return id->first_expr != NULL || id->second_expr != NULL;
}

static int flecs_irc_compile_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst);

static int flecs_irc_compile_dynamic_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);

    flecs_irc_emit(c, EcsIrDynPush, dst, 0, 0, node);

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (i) {
            flecs_irc_emit(c, EcsIrDynNext, dst, 0, 0, node);
        }

        if (elem->key) {
            int32_t key = flecs_irc_reg(c);
            if (flecs_irc_compile_expr(c, elem->key, key, false)) {
                return -1;
            }
            flecs_irc_emit(c, EcsIrDynKey, key, 0, 0, node);
        } else if (elem->member) {
            int32_t op = flecs_irc_emit(c, EcsIrDynMember, 0, 0, 0, node);
            flecs_irc_op(c, op)->imm.str = elem->member;
        }

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_irc_compile_dynamic_initializer(
                c, (ecs_expr_initializer_t*)elem->value, dst))
            {
                return -1;
            }
            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(
            elem->value);
        if (swizzle) {
            int32_t left = flecs_irc_reg(c);
            if (flecs_irc_compile_expr(c, swizzle->left, left, false)) {
                return -1;
            }
            int32_t op = flecs_irc_emit(c, EcsIrDynSwizzle, left, 0, 0, node);
            flecs_irc_op(c, op)->imm.ptr = swizzle;
            continue;
        }

        int32_t value = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, elem->value, value, false)) {
            return -1;
        }
        flecs_irc_emit(c, EcsIrDynSet, value, 0, 0, node);
    }

    flecs_irc_emit(c, EcsIrDynPop, dst, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_static_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_assert(elem->value != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t offset = flecs_uto(int32_t, elem->offset);

        if (elem->value->kind == EcsExprInitializer) {
            ecs_expr_initializer_t *inner =
                (ecs_expr_initializer_t*)elem->value;
            if (inner->is_dynamic) {
                int32_t member = flecs_irc_reg(c);
                flecs_irc_emit(c, EcsIrMember, member, dst, offset, inner);
                flecs_irc_emit(c, EcsIrDynBegin, member, 0, 0, inner);
                if (flecs_irc_compile_dynamic_initializer(c, inner, member)) {
                    return -1;
                }
                flecs_irc_emit(c, EcsIrDynEnd, member, 0, 0, inner);
            } else {
                if (flecs_irc_compile_static_initializer(c, inner, dst)) {
                    return -1;
                }
            }
            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(
            elem->value);
        if (swizzle) {
            int32_t left = flecs_irc_reg(c);
            if (flecs_irc_compile_expr(c, swizzle->left, left, false)) {
                return -1;
            }
            int32_t op = flecs_irc_emit(
                c, EcsIrInitSwizzle, dst, offset, left, node);
            flecs_irc_op(c, op)->imm.ptr = swizzle;
            continue;
        }

        if (!elem->operator && elem->value->kind == EcsExprValue) {
            ecs_expr_value_node_t *value = (ecs_expr_value_node_t*)elem->value;
            int32_t op = flecs_irc_emit(
                c, EcsIrInitConst, dst, offset, 0, elem->value);
            flecs_irc_op(c, op)->imm.ptr = value->ptr;
            continue;
        }

        int32_t value = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, elem->value, value, false)) {
            return -1;
        }

        if (!elem->operator) {
            int32_t op = flecs_irc_emit(
                c, EcsIrInitAssign, dst, offset, value, elem->value);
            flecs_irc_op(c, op)->imm.ptr = node;
        } else {
            int32_t op = flecs_irc_emit(
                c, EcsIrInitOp, dst, offset, value, elem->value);
            flecs_irc_op(c, op)->flags = (uint16_t)elem->operator;
            flecs_irc_op(c, op)->imm.ptr = node;
        }
    }

    return 0;
}

static int flecs_irc_compile_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst)
{
    if (node->is_dynamic) {
        flecs_irc_emit(c, EcsIrDynBegin, dst, 0, 0, node);
        if (flecs_irc_compile_dynamic_initializer(c, node, dst)) {
            return -1;
        }
        flecs_irc_emit(c, EcsIrDynEnd, dst, 0, 0, node);
        return 0;
    }
    return flecs_irc_compile_static_initializer(c, node, dst);
}

static int flecs_irc_compile_interpolated_string(
    ecs_script_ir_compiler_t *c,
    ecs_expr_interpolated_string_t *node,
    int32_t dst)
{
    flecs_irc_emit(c, EcsIrStrBegin, dst, 0, 0, node);

    int32_t i, e = 0, count = ecs_vec_count(&node->fragments);
    char **fragments = ecs_vec_first(&node->fragments);
    ecs_expr_format_t *formats = ecs_vec_first(&node->formats);
    for (i = 0; i < count; i ++) {
        char *fragment = fragments[i];
        if (fragment) {
            int32_t op = flecs_irc_emit(c, EcsIrStrFrag, 0, 0, 0, node);
            flecs_irc_op(c, op)->imm.str = fragment;
            continue;
        }

        ecs_expr_node_t *expr = ecs_vec_get_t(
            &node->expressions, ecs_expr_node_t*, e)[0];
        ecs_expr_format_t *format = &formats[e ++];

        if (!format->is_present && expr->kind == EcsExprCast &&
            expr->type == ecs_id(ecs_string_t))
        {
            ecs_expr_node_t *inner = ((ecs_expr_cast_t*)expr)->expr;
            ecs_entity_t it = inner->type;
            uint16_t cls = 0;
            int32_t sz = 0;
            if (it == ecs_id(ecs_char_t)) {
                cls = 4;
                sz = 1;
            } else if (it != ecs_id(ecs_bool_t) && it != ecs_id(ecs_byte_t)) {
                ecs_script_ir_num_class_t nc = flecs_irc_num_class(it, &sz);
                if (nc == EcsIrNumSigned) {
                    cls = 1;
                } else if (nc == EcsIrNumUnsigned) {
                    cls = 2;
                } else if (nc == EcsIrNumFloat) {
                    cls = 3;
                }
            }
            if (cls) {
                int32_t src = flecs_irc_reg(c);
                if (flecs_irc_compile_expr(c, inner, src, false)) {
                    return -1;
                }
                int32_t op = flecs_irc_emit(c, EcsIrStrCast, src, sz, 0, inner);
                flecs_irc_op(c, op)->flags = cls;
                continue;
            }
        }

        int32_t value = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, expr, value, false)) {
            return -1;
        }

        if (format->is_present) {
            int32_t width = -1, precision = -1;
            if (format->width) {
                width = flecs_irc_reg(c);
                if (flecs_irc_compile_expr(c, format->width, width, false)) {
                    return -1;
                }
            }
            if (format->precision) {
                precision = flecs_irc_reg(c);
                if (flecs_irc_compile_expr(
                    c, format->precision, precision, false))
                {
                    return -1;
                }
            }
            int32_t op = flecs_irc_emit(
                c, EcsIrStrFormat, value, width, precision, expr);
            flecs_irc_op(c, op)->imm.ptr = format;
        } else {
            flecs_irc_emit(c, EcsIrStrValue, value, 0, 0, expr);
        }
    }

    flecs_irc_emit(c, EcsIrStrEnd, dst, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_call(
    ecs_script_ir_compiler_t *c,
    ecs_expr_function_t *node,
    int32_t dst,
    bool in_place,
    bool method)
{
    int32_t argc = ecs_vec_count(&node->args->elements);
    int32_t total = argc + (method ? 1 : 0);
    int32_t first = c->reg_count;
    int32_t i;
    for (i = 0; i < total; i ++) {
        flecs_irc_reg(c);
    }

    int32_t arg_reg = first;
    if (method) {
        if (!node->left) {
            return 0;
        }
        if (flecs_irc_compile_expr(c, node->left, first, false)) {
            return -1;
        }
        arg_reg ++;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &node->args->elements);
    for (i = 0; i < argc; i ++) {
        if (flecs_irc_compile_expr(c, elems[i].value, arg_reg + i, false)) {
            return -1;
        }
    }

    int32_t op = flecs_irc_emit(c, EcsIrCall, dst, first, total, node);
    flecs_irc_op(c, op)->flags = (uint16_t)(
        (in_place ? EcsIrInPlace : 0) | (method ? EcsIrCallMethod : 0));
    return 0;
}

static int flecs_irc_compile_match(
    ecs_script_ir_compiler_t *c,
    ecs_expr_match_t *node,
    int32_t dst,
    bool in_place)
{
    int32_t value = flecs_irc_reg(c);
    if (flecs_irc_compile_expr(c, node->expr, value, false)) {
        return -1;
    }

    ecs_vec_t jumps;
    ecs_vec_init_t(NULL, &jumps, int32_t, 0);

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);
    for (i = 0; i < count; i ++) {
        int32_t compare = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, elems[i].compare, compare, false)) {
            goto error;
        }
        int32_t cmp = flecs_irc_emit(c, EcsIrMatchCmp, value, compare, 0, node);
        if (flecs_irc_compile_expr(c, elems[i].expr, dst, in_place)) {
            goto error;
        }
        ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
            flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
        flecs_irc_op(c, cmp)->c = flecs_irc_pc(c);
    }

    if (node->any.expr) {
        if (flecs_irc_compile_expr(c, node->any.expr, dst, in_place)) {
            goto error;
        }
    } else {
        flecs_irc_emit(c, EcsIrMatchFail, value, 0, 0, node);
    }

    int32_t end = flecs_irc_pc(c);
    int32_t *jump_ops = ecs_vec_first(&jumps);
    for (i = 0; i < ecs_vec_count(&jumps); i ++) {
        flecs_irc_op(c, jump_ops[i])->a = end;
    }

    ecs_vec_fini_t(NULL, &jumps, int32_t);
    return 0;
error:
    ecs_vec_fini_t(NULL, &jumps, int32_t);
    return -1;
}

static int flecs_irc_compile_binary(
    ecs_script_ir_compiler_t *c,
    ecs_expr_binary_t *node,
    int32_t dst,
    bool in_place)
{
    int32_t left = flecs_irc_reg(c);
    if (flecs_irc_compile_expr(c, node->left, left, false)) {
        return -1;
    }

    int32_t skip = -1;
    if (!node->vector_count && node->left->type == ecs_id(ecs_bool_t) &&
        (node->operator == EcsTokAnd || node->operator == EcsTokOr))
    {
        skip = flecs_irc_emit(c,
            node->operator == EcsTokAnd ? EcsIrJumpIfFalse : EcsIrJumpIfTrue,
            left, 0, dst, node);
        flecs_irc_op(c, skip)->flags = in_place ? EcsIrInPlace : 0;
    }

    int32_t right = flecs_irc_reg(c);
    if (flecs_irc_compile_expr(c, node->right, right, false)) {
        return -1;
    }

    ecs_script_ir_op_kind_t kind = EcsIrBinary;
    if (!node->vector_count) {
        ecs_entity_t lt = node->left->type, rt = node->node.type;
        bool cmp = rt == ecs_id(ecs_bool_t);
        if (lt == ecs_id(ecs_i64_t) && (cmp || rt == lt)) {
            kind = EcsIrBinaryI64;
        } else if (lt == ecs_id(ecs_i32_t) && (cmp || rt == lt)) {
            kind = EcsIrBinaryI32;
        } else if (lt == ecs_id(ecs_f64_t) && (cmp || rt == lt)) {
            kind = EcsIrBinaryF64;
        } else if (lt == ecs_id(ecs_f32_t) && (cmp || rt == lt)) {
            kind = EcsIrBinaryF32;
        }
        if (kind != EcsIrBinary && (node->operator == EcsTokAddAssign ||
            node->operator == EcsTokMulAssign || node->operator == EcsTokAnd ||
            node->operator == EcsTokOr))
        {
            kind = EcsIrBinary;
        }
        if ((kind == EcsIrBinaryF64 || kind == EcsIrBinaryF32) &&
            (node->operator == EcsTokMod || node->operator == EcsTokBitwiseAnd ||
             node->operator == EcsTokBitwiseOr ||
             node->operator == EcsTokShiftLeft ||
             node->operator == EcsTokShiftRight))
        {
            kind = EcsIrBinary;
        }
    }
    int32_t op = flecs_irc_emit(c, kind, dst, left, right, node);
    ecs_script_ir_op_t *ptr = flecs_irc_op(c, op);
    ptr->flags = (uint16_t)(node->operator | (in_place ? 0x8000 : 0));
    if (!node->vector_count) {
        ptr->imm.u64 = flecs_irc_num_pack(node->left->type, node->node.type);
    }

    if (skip != -1) {
        flecs_irc_op(c, skip)->b = flecs_irc_pc(c);
    }
    return 0;
}

static int flecs_irc_compile_expr(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *node,
    int32_t dst,
    bool in_place)
{
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
    uint16_t place = in_place ? EcsIrInPlace : 0;

    switch(node->kind) {
    case EcsExprValue: {
        ecs_expr_value_node_t *n = (ecs_expr_value_node_t*)node;
        int32_t op = flecs_irc_emit(c, EcsIrLoadConst, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.ptr = n->ptr;
        return 0;
    }
    case EcsExprInterpolatedString:
        return flecs_irc_compile_interpolated_string(
            c, (ecs_expr_interpolated_string_t*)node, dst);
    case EcsExprInitializer: {
        if (!in_place) {
            flecs_irc_emit(c, EcsIrInitAlloc, dst, 0, 0, node);
        }
        return flecs_irc_compile_initializer(
            c, (ecs_expr_initializer_t*)node, dst);
    }
    case EcsExprEmptyInitializer: {
        ecs_expr_initializer_t *n = (ecs_expr_initializer_t*)node;
        if (!in_place) {
            flecs_irc_emit(c, EcsIrInitAlloc, dst, 0, 0, node);
        }
        if (n->is_dynamic) {
            flecs_irc_emit(c, EcsIrDynBegin, dst, 0, 0, node);
            flecs_irc_emit(c, EcsIrDynPush, dst, 0, 0, node);
            flecs_irc_emit(c, EcsIrDynPop, dst, 0, 0, node);
            flecs_irc_emit(c, EcsIrDynEnd, dst, 0, 0, node);
        }
        return 0;
    }
    case EcsExprUnary: {
        ecs_expr_unary_t *n = (ecs_expr_unary_t*)node;
        int32_t src = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->expr, src, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrUnary, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprBinary:
        return flecs_irc_compile_binary(
            c, (ecs_expr_binary_t*)node, dst, in_place);
    case EcsExprIdentifier: {
        ecs_expr_identifier_t *n = (ecs_expr_identifier_t*)node;
        if (n->expr) {
            return flecs_irc_compile_expr(c, n->expr, dst, in_place);
        } else if (n->symbol != -1) {
            int32_t op = flecs_irc_emit(
                c, EcsIrLoadSymbol, dst, n->symbol, 0, node);
            flecs_irc_op(c, op)->flags = (uint16_t)(place |
                (node->type == ecs_id(ecs_entity_t) ? EcsIrIdDirect : 0));
        } else {
            int32_t op = flecs_irc_emit(c, EcsIrLookup, dst, 0, 0, node);
            flecs_irc_op(c, op)->imm.str = n->value;
            flecs_irc_op(c, op)->flags = (uint16_t)(place |
                (node->type == ecs_id(ecs_entity_t) ? EcsIrIdDirect : 0));
        }
        return 0;
    }
    case EcsExprVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        flecs_irc_emit(c, EcsIrLoadVar, dst, n->sp, 0, node);
        return 0;
    }
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        int32_t op = flecs_irc_emit(c, EcsIrLoadGlobal, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.entity = n->global;
        flecs_irc_op(c, op)->flags =
            n->global_component == ecs_id(EcsScriptMutVar) ? 1 : 0;
        return 0;
    }
    case EcsExprFunction:
        return flecs_irc_compile_call(
            c, (ecs_expr_function_t*)node, dst, in_place, false);
    case EcsExprMethod:
        return flecs_irc_compile_call(
            c, (ecs_expr_function_t*)node, dst, in_place, true);
    case EcsExprMember: {
        ecs_expr_member_t *n = (ecs_expr_member_t*)node;
        int32_t src = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->left, src, false)) {
            return -1;
        }
        flecs_irc_emit(c, EcsIrMember, dst, src,
            flecs_uto(int32_t, n->offset), node);
        return 0;
    }
    case EcsExprSwizzle: {
        ecs_expr_swizzle_t *n = (ecs_expr_swizzle_t*)node;
        int32_t src = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->left, src, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrSwizzle, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        int32_t left = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->left, left, false)) {
            return -1;
        }
        int32_t index = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->index, index, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrElement, dst, left, index, node);
        const EcsType *type = ecs_get(c->world, n->left->type, EcsType);
        if (type && type->kind == EcsMapType) {
            flecs_irc_op(c, op)->flags = EcsIrElementMap;
        } else if (type && type->kind == EcsVectorType) {
            flecs_irc_op(c, op)->flags = EcsIrElementVector;
        }
        return 0;
    }
    case EcsExprComponent: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        int32_t left = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->left, left, false)) {
            return -1;
        }
        ecs_expr_node_t *index = n->index;
        if (index && index->kind == EcsExprIdentifier) {
            index = ((ecs_expr_identifier_t*)index)->expr;
        }
        ecs_entity_t component;
        if (index) {
            ecs_assert(index->kind == EcsExprValue, ECS_INTERNAL_ERROR, NULL);
            component = ((ecs_expr_value_node_t*)index)->storage.entity;
        } else {
            component = node->type;
        }
        int32_t op = flecs_irc_emit(c, EcsIrComponentGet, dst, left, 0, node);
        flecs_irc_op(c, op)->imm.entity = component;
        return 0;
    }
    case EcsExprHas: {
        ecs_expr_has_t *n = (ecs_expr_has_t*)node;
        int32_t left = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->left, left, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrHas, dst, left, 0, node);
        flecs_irc_op(c, op)->imm.id = n->id;
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprCast: {
        ecs_expr_cast_t *n = (ecs_expr_cast_t*)node;
        int32_t src = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->expr, src, false)) {
            return -1;
        }
        int32_t rsize = 0;
        ecs_script_ir_num_class_t rclass = flecs_irc_num_class(
            node->type, &rsize);
        if (n->expr->type == ecs_id(ecs_bool_t) && rclass != EcsIrNumNone) {
            int32_t op = flecs_irc_emit(c, EcsIrCastNumber, dst, src, 0, node);
            flecs_irc_op(c, op)->flags = place;
            flecs_irc_op(c, op)->imm.u64 = (uint64_t)EcsIrNumUnsigned |
                ((uint64_t)1 << 8) | ((uint64_t)rclass << 16) |
                ((uint64_t)rsize << 24);
            return 0;
        }
        int32_t op = flecs_irc_emit(c, EcsIrCast, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprCastNumber: {
        ecs_expr_cast_t *n = (ecs_expr_cast_t*)node;
        int32_t src = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->expr, src, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrCastNumber, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        flecs_irc_op(c, op)->imm.u64 = flecs_irc_num_pack(
            n->expr->type, node->type);
        return 0;
    }
    case EcsExprMatch:
        return flecs_irc_compile_match(
            c, (ecs_expr_match_t*)node, dst, in_place);
    case EcsExprRange: {
        ecs_expr_range_t *n = (ecs_expr_range_t*)node;
        int32_t from = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->from, from, false)) {
            return -1;
        }
        int32_t to = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, n->to, to, false)) {
            return -1;
        }
        int32_t op = flecs_irc_emit(c, EcsIrRange, dst, from, to, node);
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprNew: {
        ecs_expr_new_t *n = (ecs_expr_new_t*)node;
        int32_t entry = flecs_irc_entry_add(c, n->entity, EcsIrEntryEntity);
        int32_t op = flecs_irc_emit(c, EcsIrNew, dst, entry, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    case EcsExprScript: {
        ecs_expr_script_t *n = (ecs_expr_script_t*)node;
        int32_t op = flecs_irc_emit(c, EcsIrScript, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.ptr = n->script;
        flecs_irc_op(c, op)->flags = place;
        return 0;
    }
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt expression node kind");
    return -1;
}

static bool flecs_irc_expr_borrows(
    ecs_expr_node_t *expr)
{
    switch(expr->kind) {
    case EcsExprValue:
    case EcsExprVariable:
    case EcsExprGlobalVariable:
    case EcsExprMember:
    case EcsExprElement:
    case EcsExprComponent:
        return true;
    case EcsExprIdentifier: {
        ecs_expr_identifier_t *id = (ecs_expr_identifier_t*)expr;
        return id->expr ? flecs_irc_expr_borrows(id->expr) : false;
    }
    case EcsExprMatch: {
        ecs_expr_match_t *m = (ecs_expr_match_t*)expr;
        int32_t i, count = ecs_vec_count(&m->elements);
        ecs_expr_match_element_t *elems = ecs_vec_first(&m->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_irc_expr_borrows(elems[i].expr)) {
                return true;
            }
        }
        return m->any.expr ? flecs_irc_expr_borrows(m->any.expr) : false;
    }
    default:
        return false;
    }
}

static bool flecs_irc_type_matches(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *expr,
    ecs_entity_t type)
{
    if (!type) {
        return false;
    }
    if (flecs_irc_expr_borrows(expr)) {
        return false;
    }
    if (expr->type == type) {
        return true;
    }
    if (expr->kind == EcsExprInitializer ||
        expr->kind == EcsExprEmptyInitializer)
    {
        return flecs_struct_is_derived_from(c->world, type, expr->type);
    }
    return false;
}

static int32_t flecs_irc_expr_begin(
    ecs_script_ir_compiler_t *c,
    const void *node)
{
    return flecs_irc_emit(c, EcsIrExprBegin, c->reg_count, 0, 0, node);
}

static void flecs_irc_expr_end(
    ecs_script_ir_compiler_t *c,
    int32_t begin,
    bool emit)
{
    ecs_script_ir_op_t *op = flecs_irc_op(c, begin);
    op->b = c->reg_count - op->a;
    if (emit) {
        flecs_irc_emit(c, EcsIrExprEnd, 0, 0, 0, NULL);
    }
}

static int flecs_irc_compile_tag(
    ecs_script_ir_compiler_t *c,
    ecs_script_tag_t *node,
    bool with)
{
    bool expr = flecs_irc_id_needs_expr(&node->id);
    int32_t begin = -1;
    if (expr) {
        begin = flecs_irc_expr_begin(c, node);
    }
    int32_t id = flecs_irc_id(c, &node->id, node, false, node->component_slot);
    if (id == -1) {
        return -1;
    }
    flecs_irc_emit(c, with ? EcsIrWithTag : EcsIrTag, id, 0, 0, node);
    if (expr) {
        flecs_irc_expr_end(c, begin, true);
    }
    return 0;
}

static int flecs_irc_compile_component(
    ecs_script_ir_compiler_t *c,
    ecs_script_component_t *node,
    bool with)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t id = flecs_irc_id(c, &node->id, node, true, node->component_slot);
    if (id == -1) {
        return -1;
    }

    ecs_script_ir_id_t desc_copy = *ecs_vec_get_t(
        &c->ir->ids, ecs_script_ir_id_t, id);
    const ecs_script_ir_id_t *desc = &desc_copy;

    bool partial = false;
    if (node->expr && node->expr->kind == EcsExprInitializer) {
        partial = ((ecs_expr_initializer_t*)node->expr)->is_partial;
    }

    int32_t visitor = -1;
    if (!with && !desc->has_second && (desc->visitor || desc->dynamic)) {
        visitor = flecs_irc_emit(c, EcsIrComponentVisitor, id, 0, 0, node);
        if (desc->visitor) {
            flecs_irc_op(c, visitor)->flags = 1;
        }
    }

    if (node->expr) {
        int32_t tmp = flecs_irc_reg(c);
        int32_t op = flecs_irc_emit(c,
            with ? EcsIrWithComponentBegin : EcsIrComponentBegin,
            id, tmp, 0, node);
        flecs_irc_op(c, op)->flags = (uint16_t)(
            (partial ? EcsIrComponentPartial : 0) |
            (node->id.interface ? EcsIrComponentInterface : 0));

        bool in_place;
        if (desc->ti) {
            in_place = flecs_irc_type_matches(
                c, node->expr, desc->ti->component);
        } else {
            in_place = node->expr->kind == EcsExprInitializer ||
                node->expr->kind == EcsExprEmptyInitializer;
        }
        int32_t value = tmp;
        if (!in_place) {
            value = flecs_irc_reg(c);
        }
        if (flecs_irc_compile_expr(c, node->expr, value, in_place)) {
            return -1;
        }
        op = flecs_irc_emit(c,
            with ? EcsIrWithComponentEnd : EcsIrComponentEnd,
            id, value, tmp, node);
        flecs_irc_op(c, op)->flags = (uint16_t)(
            node->id.interface ? EcsIrComponentInterface : 0);
    } else {
        if (with) {
            int32_t op = flecs_irc_emit(
                c, EcsIrWithComponentBegin, id, -1, 0, node);
            flecs_irc_op(c, op)->flags = (uint16_t)(
                node->id.interface ? EcsIrComponentInterface : 0);
        } else {
            int32_t op = flecs_irc_emit(c, EcsIrComponentEnd, id, -1, -1, node);
            flecs_irc_op(c, op)->flags = (uint16_t)(
                node->id.interface ? EcsIrComponentInterface : 0);
        }
    }

    if (visitor != -1) {
        flecs_irc_op(c, visitor)->b = flecs_irc_pc(c);
    }

    flecs_irc_expr_end(c, begin, true);
    return 0;
}

static int flecs_irc_compile_const(
    ecs_script_ir_compiler_t *c,
    ecs_script_var_node_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t value = flecs_irc_reg(c);
    if (flecs_irc_compile_expr(c, node->expr, value, false)) {
        return -1;
    }
    int32_t op = flecs_irc_emit(c, EcsIrConstEnd, value,
        node->computed - 1, 0, node);
    flecs_irc_op(c, op)->imm.ptr = node->eval_type
        ? ECS_CONST_CAST(void*, ecs_get_type_info(c->world, node->eval_type))
        : NULL;
    flecs_irc_expr_end(c, begin, true);
    return 0;
}

static int flecs_irc_compile_await(
    ecs_script_ir_compiler_t *c,
    ecs_script_node_t *stmt)
{
    ecs_expr_node_t *expr;
    uint16_t flags = 0;
    if (stmt->kind == EcsAstAwait) {
        expr = ((ecs_script_await_t*)stmt)->expr;
    } else {
        expr = ((ecs_script_var_node_t*)stmt)->expr;
        flags |= EcsIrAwaitVar;
    }

    int32_t check = flecs_irc_emit(c, EcsIrAwaitStart, 0, 0, 0, stmt);
    flecs_irc_op(c, check)->flags = flags;

    if (!expr || (expr->kind != EcsExprFunction &&
        expr->kind != EcsExprMethod))
    {
        int32_t op = flecs_irc_emit(c, EcsIrAwaitLaunch, -1, 0, 0, stmt);
        flecs_irc_op(c, op)->flags = flags;
    } else {
        ecs_expr_function_t *call = (ecs_expr_function_t*)expr;
        bool method = expr->kind == EcsExprMethod;
        int32_t argc = ecs_vec_count(&call->args->elements);
        int32_t total = argc + (method ? 1 : 0);

        int32_t begin = flecs_irc_expr_begin(c, stmt);
        int32_t first = c->reg_count;
        int32_t i;
        for (i = 0; i < total; i ++) {
            flecs_irc_reg(c);
        }

        int32_t arg_reg = first;
        if (method) {
            if (flecs_irc_compile_expr(c, call->left, first, false)) {
                return -1;
            }
            arg_reg ++;
        }
        ecs_expr_initializer_element_t *elems = ecs_vec_first(
            &call->args->elements);
        for (i = 0; i < argc; i ++) {
            if (flecs_irc_compile_expr(
                c, elems[i].value, arg_reg + i, false))
            {
                return -1;
            }
        }

        int32_t launch = flecs_irc_emit(
            c, EcsIrAwaitLaunch, first, total, 0, stmt);
        flecs_irc_op(c, launch)->flags = (uint16_t)(
            flags | (method ? EcsIrAwaitMethod : 0));
        flecs_irc_op(c, launch)->imm.ptr = call;
        flecs_irc_expr_end(c, begin, true);
    }

    flecs_irc_op(c, check)->b = flecs_irc_pc(c);
    int32_t poll = flecs_irc_emit(c, EcsIrAwaitPoll, 0, 0, 0, stmt);
    flecs_irc_op(c, poll)->flags = flags;
    return 0;
}

static int flecs_irc_compile_entity(
    ecs_script_ir_compiler_t *c,
    ecs_script_entity_t *node)
{
    int32_t name = -1, begin = -1;
    if (node->name_expr) {
        begin = flecs_irc_expr_begin(c, node);
        name = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, node->name_expr, name, false)) {
            return -1;
        }
    }

    int32_t op = flecs_irc_emit(c, EcsIrEntityEnter, name, 0, 0, node);
    if (name != -1) {
        flecs_irc_expr_end(c, begin, false);
        flecs_irc_op(c, op)->flags = 1;
    }

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    int result = flecs_irc_compile_scope(c, node->scope, EcsIrScopeEntity);
    c->reg_floor = floor;
    if (result) {
        return -1;
    }

    flecs_irc_emit(c, EcsIrEntityLeave, 0, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_pair_scope(
    ecs_script_ir_compiler_t *c,
    ecs_script_pair_scope_t *node)
{
    bool expr = flecs_irc_id_needs_expr(&node->id);
    int32_t begin = -1;
    if (expr) {
        begin = flecs_irc_expr_begin(c, node);
    }
    int32_t id = flecs_irc_id(c, &node->id, node, false, -1);
    if (id == -1) {
        return -1;
    }
    int32_t op = flecs_irc_emit(c, EcsIrPairScopeEnter, id, 0, 0, node);
    if (expr) {
        flecs_irc_expr_end(c, begin, false);
        flecs_irc_op(c, op)->flags = 1;
    }

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    int result = flecs_irc_compile_scope(c, node->scope, 0);
    c->reg_floor = floor;
    if (result) {
        return -1;
    }

    flecs_irc_emit(c, EcsIrPairScopeLeave, 0, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_with(
    ecs_script_ir_compiler_t *c,
    ecs_script_with_t *node)
{
    flecs_irc_emit(c, EcsIrWithEnter, 0, 0, 0, node);

    ecs_script_node_t **stmts = ecs_vec_first(&node->expressions->stmts);
    int32_t i, count = ecs_vec_count(&node->expressions->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        c->reg_count = c->reg_floor;
        if (stmt->skip) {
            continue;
        }
        if (stmt->kind == EcsAstWithTag) {
            if (flecs_irc_compile_tag(c, (ecs_script_tag_t*)stmt, true)) {
                return -1;
            }
        } else if (stmt->kind == EcsAstWithComponent) {
            if (flecs_irc_compile_component(
                c, (ecs_script_component_t*)stmt, true))
            {
                return -1;
            }
        } else {
            ecs_err("invalid statement in with expression");
            return -1;
        }
    }

    flecs_irc_emit(c, EcsIrWithBody, 0, 0, 0, node);

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    int result = flecs_irc_compile_scope(c, node->scope, 0);
    c->reg_floor = floor;
    if (result) {
        return -1;
    }

    flecs_irc_emit(c, EcsIrWithLeave, 0, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_if(
    ecs_script_ir_compiler_t *c,
    ecs_script_if_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t cond = flecs_irc_reg(c);
    if (flecs_irc_compile_expr(c, node->expr, cond, false)) {
        return -1;
    }
    flecs_irc_emit(c, EcsIrToBool, cond, 0, 0, node);
    flecs_irc_expr_end(c, begin, true);

    int32_t enter = flecs_irc_emit(c, EcsIrIfEnter, 0, 0, 0, node);

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    if (flecs_irc_compile_scope(c, node->if_true, 0)) {
        return -1;
    }
    int32_t jump = flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
    flecs_irc_op(c, enter)->b = flecs_irc_pc(c);
    if (flecs_irc_compile_scope(c, node->if_false, 0)) {
        return -1;
    }
    c->reg_floor = floor;
    flecs_irc_op(c, jump)->a = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrIfLeave, 0, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_for(
    ecs_script_ir_compiler_t *c,
    ecs_script_for_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t from = -1, to = -1;
    if (!node->expr) {
        from = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, node->from, from, false)) {
            return -1;
        }
        to = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, node->to, to, false)) {
            return -1;
        }
    } else {
        from = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, node->expr, from, false)) {
            return -1;
        }
    }
    flecs_irc_expr_end(c, begin, false);

    flecs_irc_emit(c, EcsIrForEnter, from, to, 0, node);
    int32_t next = flecs_irc_emit(c, EcsIrForNext, 0, 0, 0, node);

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    c->force_depth ++;
    int result = flecs_irc_compile_scope(c, node->scope, 0);
    c->force_depth --;
    c->reg_floor = floor;
    if (result) {
        return -1;
    }

    flecs_irc_emit(c, EcsIrJump, next, 0, 0, node);
    flecs_irc_op(c, next)->a = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrForLeave, 0, 0, 0, node);
    flecs_irc_emit(c, EcsIrExprEnd, 0, 0, 0, node);
    return 0;
}

static int flecs_irc_compile_try(
    ecs_script_ir_compiler_t *c,
    ecs_script_try_t *node)
{
    int32_t catch_count = ecs_vec_count(&node->catches);
    int32_t catch_first = ecs_vec_count(&c->ir->catches);
    int32_t i;
    ecs_vec_set_count_t(NULL, &c->ir->catches,
        int32_t, catch_first + catch_count);

    int32_t enter = flecs_irc_emit(
        c, EcsIrTryEnter, catch_first, catch_count, 0, node);

    ecs_vec_t jumps;
    ecs_vec_init_t(NULL, &jumps, int32_t, 0);

    int32_t floor = c->reg_floor;
    c->reg_floor = c->reg_count;
    if (flecs_irc_compile_scope(c, node->try_scope, 0)) {
        goto error;
    }
    ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
        flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);

    for (i = 0; i < catch_count; i ++) {
        ecs_script_catch_t *catch_ = ecs_vec_get_t(
            &node->catches, ecs_script_catch_t, i);
        ecs_vec_get_t(&c->ir->catches, int32_t,
            catch_first + i)[0] = flecs_irc_pc(c);
        if (flecs_irc_compile_scope(c, catch_->scope, 0)) {
            goto error;
        }
        ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
            flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
    }
    c->reg_floor = floor;

    int32_t end = flecs_irc_pc(c);
    int32_t *jump_ops = ecs_vec_first(&jumps);
    for (i = 0; i < ecs_vec_count(&jumps); i ++) {
        flecs_irc_op(c, jump_ops[i])->a = end;
    }
    flecs_irc_op(c, enter)->c = end;
    flecs_irc_emit(c, EcsIrTryLeave, 0, 0, 0, node);
    ecs_vec_fini_t(NULL, &jumps, int32_t);
    return 0;
error:
    c->reg_floor = floor;
    ecs_vec_fini_t(NULL, &jumps, int32_t);
    return -1;
}

static bool flecs_irc_stmt_always(
    ecs_script_node_t *node)
{
    return (node->kind == EcsAstConst &&
            !((ecs_script_var_node_t*)node)->computed) ||
        node->kind == EcsAstUsing ||
        node->kind == EcsAstModule;
}

static int flecs_irc_compile_stmt(
    ecs_script_ir_compiler_t *c,
    ecs_script_scope_t *scope,
    int32_t index)
{
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t *node = stmts[index];

    c->reg_count = c->reg_floor;

    int32_t marks = node->region - 1;
    int32_t stmt = -1;
    if (!c->force_depth || node->skip) {
        stmt = flecs_irc_emit(c, EcsIrStmt, 0, 0, marks, node);
        ecs_script_ir_op_t *op = flecs_irc_op(c, stmt);
        op->imm.u64 = node->input;
        op->flags = (uint16_t)(
            (flecs_irc_stmt_always(node) ? EcsIrStmtAlways : 0) |
            (node->skip ? EcsIrStmtSkip : 0));
    }

    if (node->skip) {
        flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
        return 0;
    }

    int result = 0;
    switch(node->kind) {
    case EcsAstScope:
        result = flecs_irc_compile_scope(c, (ecs_script_scope_t*)node, 0);
        break;
    case EcsAstTag:
        result = flecs_irc_compile_tag(c, (ecs_script_tag_t*)node, false);
        break;
    case EcsAstComponent:
        result = flecs_irc_compile_component(
            c, (ecs_script_component_t*)node, false);
        break;
    case EcsAstWithTag:
    case EcsAstWithComponent:
        ecs_err("invalid context for with expression");
        result = -1;
        break;
    case EcsAstWith:
        result = flecs_irc_compile_with(c, (ecs_script_with_t*)node);
        break;
    case EcsAstUsing:
        flecs_irc_emit(c, EcsIrUsing, 0, 0, 0, node);
        break;
    case EcsAstModule:
        flecs_irc_emit(c, EcsIrModule, 0, 0, 0, node);
        break;
    case EcsAstAnnotation: {
        int32_t op = flecs_irc_emit(c, EcsIrAnnot, 0, 0, 0, node);
        ecs_script_node_t *next = (index + 1) < count
            ? stmts[index + 1] : NULL;
        if (!next) {
            flecs_irc_op(c, op)->flags = EcsIrAnnotNoTarget;
        } else if (next->kind != EcsAstEntity &&
            next->kind != EcsAstTemplate && next->kind != EcsAstAnnotation)
        {
            flecs_irc_op(c, op)->flags = EcsIrAnnotBadTarget;
        }
        break;
    }
    case EcsAstTemplate: {
        ecs_script_template_node_t *n = (ecs_script_template_node_t*)node;
        flecs_irc_entry_add(c, n, EcsIrEntryTemplate);
        flecs_irc_emit(c, EcsIrTemplate, 0, 0, 0, node);
        break;
    }
    case EcsAstProp:
        break;
    case EcsAstMut:
        flecs_irc_emit(c, EcsIrMutCheck, 0, 0, 0, node);
        break;
    case EcsAstConst: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (n->is_await) {
            result = flecs_irc_compile_await(c, node);
        } else {
            result = flecs_irc_compile_const(c, n);
            if (!result && n->computed && stmt != -1) {
                int32_t jump = flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
                flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
                flecs_irc_op(c, stmt)->flags |= EcsIrStmtCached;
                flecs_irc_emit(c, EcsIrConstCached,
                    n->computed - 1, stmt + 1, 0, node);
                flecs_irc_op(c, jump)->a = flecs_irc_pc(c);
                stmt = -1;
            }
        }
        break;
    }
    case EcsAstExportConst:
    case EcsAstExportMut:
        break;
    case EcsAstEntity: {
        ecs_script_entity_t *n = (ecs_script_entity_t*)node;
        if (n->is_type) {
            flecs_irc_emit(c, EcsIrAnnotClear, 0, 0, 0, node);
            if (marks != -1) {
                flecs_irc_emit(c, EcsIrMark, 0, 0, marks, node);
            }
        } else {
            result = flecs_irc_compile_entity(c, n);
        }
        break;
    }
    case EcsAstPairScope:
        result = flecs_irc_compile_pair_scope(
            c, (ecs_script_pair_scope_t*)node);
        break;
    case EcsAstIf:
        result = flecs_irc_compile_if(c, (ecs_script_if_t*)node);
        break;
    case EcsAstFor:
        result = flecs_irc_compile_for(c, (ecs_script_for_t*)node);
        break;
    case EcsAstInclude:
        break;
    case EcsAstFunction:
        flecs_irc_entry_add(c, node, EcsIrEntryFunction);
        break;
    case EcsAstAwait:
        result = flecs_irc_compile_await(c, node);
        break;
    case EcsAstTry:
        result = flecs_irc_compile_try(c, (ecs_script_try_t*)node);
        break;
    case EcsAstContinue:
        flecs_irc_emit(c, EcsIrContinue, 0, 0, 0, node);
        break;
    }

    if (result) {
        return -1;
    }

    if (stmt != -1) {
        flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
    }
    return 0;
}

static int32_t flecs_irc_scope_var_count(
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts), result = 0;
    for (i = 0; i < count; i ++) {
        if (stmts[i]->kind == EcsAstConst && !stmts[i]->skip) {
            result ++;
        }
    }
    return result;
}

static int flecs_irc_compile_scope(
    ecs_script_ir_compiler_t *c,
    ecs_script_scope_t *scope,
    uint16_t flags)
{
    int32_t var_count = flecs_irc_scope_var_count(scope);
    if (c->scope && !var_count) {
        var_count = -1;
    }

    int32_t component_count = ecs_vec_count(&scope->components);
    int32_t enter = flecs_irc_emit(
        c, EcsIrScopeEnter, scope->scope_slot, var_count, component_count, scope);
    flecs_irc_op(c, enter)->flags = flags;
    flecs_irc_op(c, enter)->imm.ptr = ecs_vec_first(&scope->components);

    scope->parent = c->scope;
    ecs_script_scope_t *prev = c->scope;
    c->scope = scope;

    int32_t i, count = ecs_vec_count(&scope->stmts);
    int32_t block = -1;
    bool blocks = count >= 32 && !c->force_depth;
    for (i = 0; i < count; i ++) {
        ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
        if (blocks && !(i % 32)) {
            if (block != -1) {
                flecs_irc_op(c, block)->b = flecs_irc_pc(c);
            }
            block = flecs_irc_emit(c, EcsIrStmtBlock, 0, 0, 0, scope);
        }
        int32_t stmt = flecs_irc_pc(c);
        if (flecs_irc_compile_stmt(c, scope, i)) {
            c->scope = prev;
            return -1;
        }
        if (block != -1) {
            const ecs_script_ir_op_t *stmt_op = flecs_irc_op(c, stmt);
            bool always = stmt_op->flags & (EcsIrStmtAlways | EcsIrStmtCached);
            if (stmt_op->c != -1) {
                const ecs_script_region_t *region = ecs_vec_get_t(
                    &c->script->regions, ecs_script_region_t, stmt_op->c);
                always |= region->for_count != 0;
            }
            ecs_script_ir_op_t *block_op = flecs_irc_op(c, block);
            block_op->imm.u64 |= stmts[i]->input;
            block_op->c ++;
            if (always) {
                block_op->flags |= EcsIrStmtAlways;
            }
        }
    }

    if (block != -1) {
        flecs_irc_op(c, block)->b = flecs_irc_pc(c);
    }

    c->scope = prev;
    flecs_irc_emit(c, EcsIrScopeLeave, 0, 0, 0, scope);
    return 0;
}

static int flecs_irc_compile_function(
    ecs_script_ir_compiler_t *c,
    ecs_script_function_node_t *node)
{
    ecs_script_scope_t *body = node->body;
    body->parent = NULL;
    c->scope = body;

    int32_t i, count = ecs_vec_count(&body->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t **stmts = ecs_vec_first(&body->stmts);
        if (flecs_irc_compile_stmt(c, body, i)) {
            c->scope = NULL;
            return -1;
        }
    }

    c->scope = NULL;
    c->reg_count = c->reg_floor;

    if (node->return_expr) {
        int32_t begin = flecs_irc_expr_begin(c, node);
        int32_t value = flecs_irc_reg(c);
        if (flecs_irc_compile_expr(c, node->return_expr, value, false)) {
            return -1;
        }
        flecs_irc_expr_end(c, begin, false);
        flecs_irc_emit(c, EcsIrReturn, value, 0, 0, node);
    }

    return 0;
}

static int flecs_irc_compile_entry(
    ecs_script_ir_compiler_t *c,
    int32_t index)
{
    ecs_script_ir_entry_t *entry = ecs_vec_get_t(
        &c->ir->entries, ecs_script_ir_entry_t, index);
    entry->pc = flecs_irc_pc(c);
    ecs_script_ir_entry_kind_t kind = entry->kind;
    const void *node = entry->node;
    c->entry = index;
    c->reg_count = 0;
    c->reg_floor = 0;
    c->reg_max = 0;
    c->scope = NULL;
    c->force_depth = 0;

    int result = 0;
    switch(kind) {
    case EcsIrEntryRoot:
        result = flecs_irc_compile_scope(
            c, ECS_CONST_CAST(ecs_script_scope_t*, node), 0);
        break;
    case EcsIrEntryTemplate: {
        ecs_script_template_node_t *n =
            ECS_CONST_CAST(ecs_script_template_node_t*, node);
        result = flecs_irc_compile_scope(c, n->scope, 0);
        break;
    }
    case EcsIrEntryEntity:
        result = flecs_irc_compile_entity(
            c, ECS_CONST_CAST(ecs_script_entity_t*, node));
        break;
    case EcsIrEntryFunction:
        result = flecs_irc_compile_function(
            c, ECS_CONST_CAST(ecs_script_function_node_t*, node));
        break;
    }

    flecs_irc_emit(c, EcsIrEnd, 0, 0, 0, node);

    entry = ecs_vec_get_t(&c->ir->entries, ecs_script_ir_entry_t, index);
    entry->reg_count = c->reg_max;
    return result;
}

static void flecs_irc_init(
    ecs_script_ir_t *ir)
{
    ecs_vec_init_t(NULL, &ir->ops, ecs_script_ir_op_t, 0);
    ecs_vec_init_t(NULL, &ir->ids, ecs_script_ir_id_t, 0);
    ecs_vec_init_t(NULL, &ir->catches, int32_t, 0);
    ecs_vec_init_t(NULL, &ir->entries, ecs_script_ir_entry_t, 0);
    ecs_map_init(&ir->entry_index, NULL);
    ir->root_entry = -1;
}

void flecs_script_ir_free(
    ecs_script_ir_t *ir)
{
    if (!ir) {
        return;
    }
    ecs_vec_fini_t(NULL, &ir->ops, ecs_script_ir_op_t);
    ecs_vec_fini_t(NULL, &ir->ids, ecs_script_ir_id_t);
    ecs_vec_fini_t(NULL, &ir->catches, int32_t);
    ecs_vec_fini_t(NULL, &ir->entries, ecs_script_ir_entry_t);
    ecs_map_fini(&ir->entry_index);
    ecs_os_free(ir);
}

ecs_script_ir_t* flecs_script_ir_compile(
    ecs_script_impl_t *script)
{
    ecs_script_ir_t *ir = ecs_os_calloc_t(ecs_script_ir_t);
    flecs_irc_init(ir);

    ecs_script_ir_compiler_t c = {
        .script = script,
        .world = script->pub.world,
        .ir = ir
    };

    ir->root_entry = flecs_irc_entry_add(&c, script->root, EcsIrEntryRoot);

    int32_t i;
    for (i = 0; i < ecs_vec_count(&ir->entries); i ++) {
        if (flecs_irc_compile_entry(&c, i)) {
            flecs_script_ir_free(ir);
            return NULL;
        }
    }

    return ir;
}

ecs_script_ir_t* flecs_script_ir_ensure(
    ecs_script_impl_t *script)
{
    if (!script->ir) {
        script->ir = flecs_script_ir_compile(script);
    }
    return script->ir;
}

const ecs_script_ir_entry_t* flecs_script_ir_entry(
    const ecs_script_ir_t *ir,
    const void *node)
{
    const uint64_t *index = ecs_map_get(&ir->entry_index, (uintptr_t)node);
    return index ? ecs_vec_get_t(
        &ir->entries, ecs_script_ir_entry_t, (int32_t)*index) : NULL;
}

#endif
