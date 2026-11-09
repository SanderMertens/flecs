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

static void flecs_irc_compile_expr(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *node,
    int32_t dst,
    bool in_place);

static void flecs_irc_compile_scope(
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

static int32_t flecs_irc_compile_tmp(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *node)
{
    if (!node) {
        return -1;
    }

    int32_t reg = flecs_irc_reg(c);
    flecs_irc_compile_expr(c, node, reg, false);
    return reg;
}

static int32_t flecs_irc_id(
    ecs_script_ir_compiler_t *c,
    ecs_script_id_t *id,
    bool is_component,
    int32_t component_slot)
{
    int32_t index_reg = flecs_irc_compile_tmp(c, id->index_expr);
    int32_t first_reg = flecs_irc_compile_tmp(c, id->first_expr);
    int32_t second_reg = flecs_irc_compile_tmp(c, id->second_expr);

    ecs_script_ir_id_t *desc = ecs_vec_append_t(
        NULL, &c->ir->ids, ecs_script_ir_id_t);
    ecs_os_zeromem(desc);
    desc->id = id;
    desc->eval = id->eval;
    desc->flag = id->flag;
    desc->first_eval = id->first_eval;
    desc->second_eval = id->second_eval;
    desc->first = id->first;
    desc->second = id->second;
    desc->first_symbol = id->first_symbol;
    desc->second_symbol = id->second_symbol;
    desc->first_sp = id->first_sp;
    desc->second_sp = id->second_sp;
    desc->first_reg = first_reg;
    desc->second_reg = second_reg;
    desc->index_reg = index_reg;
    desc->component_slot = component_slot;
    desc->resolved = id->eval != 0;
    desc->has_second = id->second != NULL;
    desc->dynamic = id->first_expr || id->second_expr || id->index_expr ||
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

    return ecs_vec_count(&c->ir->ids) - 1;
}

static bool flecs_irc_id_needs_expr(
    const ecs_script_id_t *id)
{
    return id->first_expr != NULL || id->second_expr != NULL ||
        id->index_expr != NULL;
}

static void flecs_irc_compile_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst);

static void flecs_irc_compile_dynamic_initializer(
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
            int32_t key = flecs_irc_compile_tmp(c, elem->key);
            flecs_irc_emit(c, EcsIrDynKey, key, 0, 0, node);
        } else if (elem->member) {
            int32_t op = flecs_irc_emit(c, EcsIrDynMember, 0, 0, 0, node);
            flecs_irc_op(c, op)->imm.str = elem->member;
        }

        if (elem->value->kind == EcsExprInitializer) {
            flecs_irc_compile_dynamic_initializer(
                c, (ecs_expr_initializer_t*)elem->value, dst);
            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(
            elem->value);
        if (swizzle) {
            int32_t left = flecs_irc_compile_tmp(c, swizzle->left);
            int32_t op = flecs_irc_emit(c, EcsIrDynSwizzle, left, 0, 0, node);
            flecs_irc_op(c, op)->imm.ptr = swizzle;
            continue;
        }

        int32_t value = flecs_irc_compile_tmp(c, elem->value);
        flecs_irc_emit(c, EcsIrDynSet, value, 0, 0, node);
    }

    flecs_irc_emit(c, EcsIrDynPop, dst, 0, 0, node);
}

static void flecs_irc_compile_static_initializer(
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
                flecs_irc_compile_initializer(c, inner, member);
            } else {
                flecs_irc_compile_static_initializer(c, inner, dst);
            }

            continue;
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(
            elem->value);
        if (swizzle) {
            int32_t left = flecs_irc_compile_tmp(c, swizzle->left);
            int32_t op = flecs_irc_emit(
                c, EcsIrInitSwizzle, dst, offset, left, node);
            flecs_irc_op(c, op)->imm.ptr = swizzle;
            continue;
        }

        if (elem->value->kind == EcsExprValue) {
            ecs_expr_value_node_t *value = (ecs_expr_value_node_t*)elem->value;
            int32_t op = flecs_irc_emit(
                c, EcsIrInitConst, dst, offset, 0, elem->value);
            flecs_irc_op(c, op)->imm.ptr = value->ptr;
            continue;
        }

        int32_t value = flecs_irc_compile_tmp(c, elem->value);
        int32_t op = flecs_irc_emit(c,
            EcsIrInitAssign, dst, offset, value, elem->value);
        flecs_irc_op(c, op)->imm.ptr = node;
    }
}

static void flecs_irc_compile_initializer(
    ecs_script_ir_compiler_t *c,
    ecs_expr_initializer_t *node,
    int32_t dst)
{
    if (!node->is_dynamic) {
        flecs_irc_compile_static_initializer(c, node, dst);
        return;
    }

    flecs_irc_emit(c, EcsIrDynBegin, dst, 0, 0, node);
    flecs_irc_compile_dynamic_initializer(c, node, dst);
    flecs_irc_emit(c, EcsIrDynEnd, dst, 0, 0, node);
}

static void flecs_irc_compile_interpolated_string(
    ecs_script_ir_compiler_t *c,
    ecs_expr_interpolated_string_t *node,
    int32_t dst)
{
    flecs_irc_emit(c, EcsIrStrBegin, dst, 0, 0, node);

    int32_t i, count = ecs_vec_count(&node->fragments);
    ecs_expr_fragment_t *fragments = ecs_vec_first(&node->fragments);
    for (i = 0; i < count; i ++) {
        char *fragment = fragments[i].text;
        if (fragment) {
            int32_t op = flecs_irc_emit(c, EcsIrStrFrag, 0, 0, 0, node);
            flecs_irc_op(c, op)->imm.str = fragment;
        }

        if (!fragments[i].expr) {
            continue;
        }

        ecs_expr_node_t *expr = fragments[i].expr;
        ecs_expr_format_t *format = &fragments[i].format;

        if (!format->is_present && expr->kind == EcsExprCast &&
            expr->type == ecs_id(ecs_string_t))
        {
            ecs_expr_node_t *inner = ((ecs_expr_cast_t*)expr)->expr;
            int32_t size;
            flecs_expr_num_class_t cls = inner->type == ecs_id(ecs_char_t)
                ? EcsExprNumChar : flecs_expr_num_class(inner->type, &size);
            if (cls) {
                int32_t src = flecs_irc_compile_tmp(c, inner);
                int32_t op = flecs_irc_emit(c, EcsIrStrCast, src, 0, 0, inner);
                flecs_irc_op(c, op)->flags = (uint16_t)cls;
                if (cls != EcsExprNumChar) {
                    flecs_irc_op(c, op)->imm.cast = flecs_expr_cast_handler(
                        inner->type, cls == EcsExprNumSigned ? ecs_id(ecs_i64_t)
                            : cls == EcsExprNumUnsigned ? ecs_id(ecs_u64_t)
                            : ecs_id(ecs_f64_t));
                }

                continue;
            }
        }

        int32_t value = flecs_irc_compile_tmp(c, expr);
        if (format->is_present) {
            int32_t width = flecs_irc_compile_tmp(c, format->width);
            int32_t precision = flecs_irc_compile_tmp(c, format->precision);
            int32_t op = flecs_irc_emit(
                c, EcsIrStrFormat, value, width, precision, expr);
            flecs_irc_op(c, op)->imm.ptr = format;
        } else {
            flecs_irc_emit(c, EcsIrStrValue, value, 0, 0, expr);
        }
    }

    flecs_irc_emit(c, EcsIrStrEnd, dst, 0, 0, node);
}

static int32_t flecs_irc_compile_args(
    ecs_script_ir_compiler_t *c,
    ecs_expr_function_t *node,
    int32_t *count)
{
    int32_t method = node->node.kind == EcsExprMethod;
    int32_t i, argc = ecs_vec_count(&node->args->elements);
    int32_t first = c->reg_count;
    *count = argc + method;
    for (i = 0; i < *count; i ++) {
        flecs_irc_reg(c);
    }

    if (method) {
        flecs_irc_compile_expr(c, node->left, first, false);
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &node->args->elements);
    for (i = 0; i < argc; i ++) {
        flecs_irc_compile_expr(c, elems[i].value, first + method + i, false);
    }

    return first;
}

static void flecs_irc_compile_call(
    ecs_script_ir_compiler_t *c,
    ecs_expr_function_t *node,
    int32_t dst,
    bool in_place)
{
    int32_t count, first = flecs_irc_compile_args(c, node, &count);
    int32_t op = flecs_irc_emit(c, EcsIrCall, dst, first, count, node);
    flecs_irc_op(c, op)->flags = (uint16_t)((in_place ? EcsIrInPlace : 0) |
        (node->node.kind == EcsExprMethod ? EcsIrCallMethod : 0));
}

static void flecs_irc_patch_jumps(
    ecs_script_ir_compiler_t *c,
    ecs_vec_t *jumps)
{
    int32_t i, end = flecs_irc_pc(c);
    int32_t *jump_ops = ecs_vec_first(jumps);
    for (i = 0; i < ecs_vec_count(jumps); i ++) {
        flecs_irc_op(c, jump_ops[i])->a = end;
    }

    ecs_vec_fini_t(NULL, jumps, int32_t);
}

static void flecs_irc_compile_match(
    ecs_script_ir_compiler_t *c,
    ecs_expr_match_t *node,
    int32_t dst,
    bool in_place)
{
    int32_t value = flecs_irc_compile_tmp(c, node->expr);

    ecs_vec_t jumps;
    ecs_vec_init_t(NULL, &jumps, int32_t, 0);

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);
    for (i = 0; i < count; i ++) {
        int32_t compare = flecs_irc_compile_tmp(c, elems[i].compare);
        int32_t cmp = flecs_irc_emit(c, EcsIrMatchCmp, value, compare, 0, node);
        flecs_irc_compile_expr(c, elems[i].expr, dst, in_place);
        ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
            flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
        flecs_irc_op(c, cmp)->c = flecs_irc_pc(c);
    }

    if (node->any.expr) {
        flecs_irc_compile_expr(c, node->any.expr, dst, in_place);
    } else {
        flecs_irc_emit(c, EcsIrMatchFail, value, 0, 0, node);
    }

    flecs_irc_patch_jumps(c, &jumps);
}

static void flecs_irc_compile_binary(
    ecs_script_ir_compiler_t *c,
    ecs_expr_binary_t *node,
    int32_t dst,
    bool in_place)
{
    uint16_t place = in_place ? EcsIrInPlace : 0;
    int32_t left = flecs_irc_compile_tmp(c, node->left);

    int32_t skip = -1;
    if (!node->vector_count && node->left->type == ecs_id(ecs_bool_t) &&
        (node->operator == EcsTokAnd || node->operator == EcsTokOr))
    {
        skip = flecs_irc_emit(c,
            node->operator == EcsTokAnd ? EcsIrJumpIfFalse : EcsIrJumpIfTrue,
            left, 0, dst, node);
        flecs_irc_op(c, skip)->flags = place;
    }

    int32_t right = flecs_irc_compile_tmp(c, node->right);
    int32_t op = flecs_irc_emit(c, EcsIrBinary, dst, left, right, node);
    flecs_irc_op(c, op)->flags = (uint16_t)((uint32_t)node->operator |
        (in_place ? EcsIrBinaryInPlace : 0u));

    int32_t size;
    flecs_expr_num_class_t class = flecs_expr_num_class(
        node->left->type, &size);
    if (!node->vector_count && class && (node->node.type == node->left->type ||
        node->node.type == ecs_id(ecs_bool_t)))
    {
        static const uint16_t kinds[4][9] = {
            [EcsExprNumSigned] = {
                [1] = EcsIrBinaryI8, [2] = EcsIrBinaryI16,
                [4] = EcsIrBinaryI32, [8] = EcsIrBinaryI64
            },
            [EcsExprNumUnsigned] = {
                [1] = EcsIrBinaryU8, [2] = EcsIrBinaryU16,
                [4] = EcsIrBinaryU32, [8] = EcsIrBinaryU64
            },
            [EcsExprNumFloat] = {
                [4] = EcsIrBinaryF32, [8] = EcsIrBinaryF64
            }
        };
        flecs_irc_op(c, op)->kind = kinds[class][size];
    }

    if (skip != -1) {
        flecs_irc_op(c, skip)->b = flecs_irc_pc(c);
    }
}

static void flecs_irc_compile_expr(
    ecs_script_ir_compiler_t *c,
    ecs_expr_node_t *node,
    int32_t dst,
    bool in_place)
{
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
    uint16_t place = in_place ? EcsIrInPlace : 0;
    int32_t op;

    switch(node->kind) {
    case EcsExprValue:
        op = flecs_irc_emit(c, EcsIrLoadConst, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.ptr = ((ecs_expr_value_node_t*)node)->ptr;
        return;
    case EcsExprInterpolatedString:
        flecs_irc_compile_interpolated_string(
            c, (ecs_expr_interpolated_string_t*)node, dst);
        return;
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        if (!in_place) {
            flecs_irc_emit(c, EcsIrInitAlloc, dst, 0, 0, node);
        }

        flecs_irc_compile_initializer(c, (ecs_expr_initializer_t*)node, dst);
        return;
    case EcsExprUnary: {
        int32_t src = flecs_irc_compile_tmp(
            c, ((ecs_expr_unary_t*)node)->expr);
        op = flecs_irc_emit(c, EcsIrUnary, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return;
    }
    case EcsExprBinary:
        flecs_irc_compile_binary(c, (ecs_expr_binary_t*)node, dst, in_place);
        return;
    case EcsExprIdentifier: {
        ecs_expr_identifier_t *n = (ecs_expr_identifier_t*)node;
        if (n->expr) {
            flecs_irc_compile_expr(c, n->expr, dst, in_place);
            return;
        }

        op = flecs_irc_emit(c, n->symbol != -1 ? EcsIrLoadSymbol : EcsIrLookup,
            dst, n->symbol, 0, node);
        flecs_irc_op(c, op)->imm.str = n->value;
        flecs_irc_op(c, op)->flags = (uint16_t)(place |
            (node->type == ecs_id(ecs_entity_t) ? EcsIrIdDirect : 0));
        return;
    }
    case EcsExprVariable:
        flecs_irc_emit(c, EcsIrLoadVar, dst,
            ((ecs_expr_variable_t*)node)->sp, 0, node);
        return;
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        op = flecs_irc_emit(c, EcsIrLoadGlobal, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.entity = n->global;
        flecs_irc_op(c, op)->flags =
            n->global_component == ecs_id(EcsScriptMutVar) ? EcsIrGlobalMut : 0;
        return;
    }
    case EcsExprFunction:
    case EcsExprMethod:
        flecs_irc_compile_call(c, (ecs_expr_function_t*)node, dst, in_place);
        return;
    case EcsExprTemplate: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        ecs_expr_node_t *args = (ecs_expr_node_t*)n->args;
        bool has_type = n->args && n->args->node.type_info != NULL;
        bool has_args = n->args && ecs_vec_count(&n->args->elements) != 0;
        bool from_var = flecs_expr_template_ref_var(n->left) != NULL;
        if (!from_var && node->type != ecs_id(ecs_script_template_ref_t)) {
            flecs_irc_compile_expr(c, args, dst, in_place);
            return;
        }

        int32_t src = flecs_irc_compile_tmp(
            c, from_var ? n->left : (has_type ? args : NULL));
        op = flecs_irc_emit(c, EcsIrTemplateRef, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = (uint16_t)(place |
            (from_var ? EcsIrTemplateRefVar : 0));
        flecs_irc_op(c, op)->imm.entity = n->calldata.function;

        if (from_var && has_args) {
            int32_t value = flecs_irc_reg(c);
            flecs_irc_emit(c, EcsIrTemplateRefValue, value, dst, 0, args);
            flecs_irc_compile_expr(c, args, value, true);
        }

        return;
    }
    case EcsExprMember: {
        ecs_expr_member_t *n = (ecs_expr_member_t*)node;
        int32_t src = flecs_irc_compile_tmp(c, n->left);
        flecs_irc_emit(c, EcsIrMember, dst, src,
            flecs_uto(int32_t, n->offset), node);
        return;
    }
    case EcsExprSwizzle: {
        int32_t src = flecs_irc_compile_tmp(
            c, ((ecs_expr_swizzle_t*)node)->left);
        op = flecs_irc_emit(c, EcsIrSwizzle, dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return;
    }
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        int32_t left = flecs_irc_compile_tmp(c, n->left);
        int32_t index = flecs_irc_compile_tmp(c, n->index);
        op = flecs_irc_emit(c, EcsIrElement, dst, left, index, node);
        const EcsType *type = ecs_get(c->world, n->left->type, EcsType);
        if (type && type->kind == EcsMapType) {
            flecs_irc_op(c, op)->flags = EcsIrElementMap;
        } else if (type && type->kind == EcsVectorType) {
            flecs_irc_op(c, op)->flags = EcsIrElementVector;
        }

        return;
    }
    case EcsExprComponent: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        int32_t left = flecs_irc_compile_tmp(c, n->left);

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

        op = flecs_irc_emit(c, EcsIrComponentGet, dst, left, 0, node);
        flecs_irc_op(c, op)->imm.entity = component;
        return;
    }
    case EcsExprHas: {
        ecs_expr_has_t *n = (ecs_expr_has_t*)node;
        int32_t left = flecs_irc_compile_tmp(c, n->left);
        op = flecs_irc_emit(c, EcsIrHas, dst, left, 0, node);
        flecs_irc_op(c, op)->imm.id = n->id;
        flecs_irc_op(c, op)->flags = place;
        return;
    }
    case EcsExprCast:
    case EcsExprCastNumber: {
        ecs_expr_cast_t *n = (ecs_expr_cast_t*)node;
        int32_t src = flecs_irc_compile_tmp(c, n->expr);
        int32_t size;
        bool number = node->kind == EcsExprCastNumber ||
            (n->expr->type == ecs_id(ecs_bool_t) &&
                flecs_expr_num_class(node->type, &size) != EcsExprNumNone);
        op = flecs_irc_emit(c, number ? EcsIrCastNumber : EcsIrCast,
            dst, src, 0, node);
        flecs_irc_op(c, op)->flags = place;
        if (number) {
            flecs_irc_op(c, op)->imm.cast = flecs_expr_cast_handler(
                n->expr->type, node->type);
        }

        return;
    }
    case EcsExprMatch:
        flecs_irc_compile_match(c, (ecs_expr_match_t*)node, dst, in_place);
        return;
    case EcsExprRange: {
        ecs_expr_range_t *n = (ecs_expr_range_t*)node;
        int32_t from = flecs_irc_compile_tmp(c, n->from);
        int32_t to = flecs_irc_compile_tmp(c, n->to);
        op = flecs_irc_emit(c, EcsIrRange, dst, from, to, node);
        flecs_irc_op(c, op)->flags = place;
        return;
    }
    case EcsExprNew: {
        int32_t entry = flecs_irc_entry_add(
            c, ((ecs_expr_new_t*)node)->entity, EcsIrEntryEntity);
        op = flecs_irc_emit(c, EcsIrNew, dst, entry, 0, node);
        flecs_irc_op(c, op)->flags = place;
        return;
    }
    case EcsExprScript:
        op = flecs_irc_emit(c, EcsIrScript, dst, 0, 0, node);
        flecs_irc_op(c, op)->imm.ptr = ((ecs_expr_script_t*)node)->script;
        flecs_irc_op(c, op)->flags = place;
        return;
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt expression node kind");
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

static void flecs_irc_compile_body(
    ecs_script_ir_compiler_t *c,
    ecs_script_scope_t *scope,
    uint16_t flags)
{
    int32_t floor = c->reg_floor, count = c->reg_count;
    c->reg_floor = count;
    flecs_irc_compile_scope(c, scope, flags);
    c->reg_floor = floor;
    c->reg_count = count;
}

static void flecs_irc_compile_id_op(
    ecs_script_ir_compiler_t *c,
    const void *node,
    ecs_script_id_t *id,
    int32_t component_slot,
    ecs_script_ir_op_kind_t kind)
{
    bool expr = flecs_irc_id_needs_expr(id);
    int32_t begin = expr ? flecs_irc_expr_begin(c, node) : -1;
    int32_t index = flecs_irc_id(c, id, false, component_slot);
    int32_t op = flecs_irc_emit(c, kind, index, 0, 0, node);
    if (expr) {
        bool scope = kind == EcsIrPairScopeEnter;
        flecs_irc_expr_end(c, begin, !scope);
        flecs_irc_op(c, op)->flags = scope ? EcsIrEndsExpr : 0;
    }
}

static void flecs_irc_compile_tag(
    ecs_script_ir_compiler_t *c,
    ecs_script_tag_t *node,
    bool with)
{
    flecs_irc_compile_id_op(c, node, &node->id, node->component_slot,
        with ? EcsIrWithTag : EcsIrTag);
}

static void flecs_irc_compile_component(
    ecs_script_ir_compiler_t *c,
    ecs_script_component_t *node,
    bool with)
{
    bool expr = node->expr || flecs_irc_id_needs_expr(&node->id);
    int32_t begin = expr ? flecs_irc_expr_begin(c, node) : -1;
    int32_t id = flecs_irc_id(c, &node->id, true, node->component_slot);
    const ecs_script_ir_id_t *desc = ecs_vec_get_t(
        &c->ir->ids, ecs_script_ir_id_t, id);

    int32_t visitor = -1;
    if (!with && !desc->has_second && (desc->visitor || desc->dynamic)) {
        visitor = flecs_irc_emit(c, EcsIrComponentVisitor, id, 0, 0, node);
        if (desc->visitor) {
            flecs_irc_op(c, visitor)->flags = EcsIrVisitorStatic;
        }
    }

    bool static_visitor = visitor != -1 && desc->visitor;
    if (!static_visitor && node->expr) {
        int32_t tmp = flecs_irc_reg(c);
        int32_t op = flecs_irc_emit(c,
            with ? EcsIrWithComponentBegin : EcsIrComponentBegin,
            id, tmp, 0, node);
        flecs_irc_op(c, op)->flags = node->is_partial ? EcsIrComponentPartial : 0;

        bool in_place;
        if (desc->ti) {
            in_place = flecs_irc_type_matches(
                c, node->expr, desc->ti->component);
        } else {
            in_place = node->expr->kind == EcsExprInitializer ||
                node->expr->kind == EcsExprEmptyInitializer;
        }

        int32_t value = in_place ? tmp : flecs_irc_reg(c);
        flecs_irc_compile_expr(c, node->expr, value, in_place);
        flecs_irc_emit(c,
            with ? EcsIrWithComponentEnd : EcsIrComponentEnd,
            id, value, tmp, node);
    } else if (!static_visitor && with) {
        flecs_irc_emit(c, EcsIrWithComponentBegin, id, -1, 0, node);
    } else if (!static_visitor) {
        flecs_irc_emit(c, EcsIrComponentEnd, id, -1, -1, node);
    }

    if (visitor != -1) {
        flecs_irc_op(c, visitor)->b = flecs_irc_pc(c);
    }

    if (expr) {
        flecs_irc_expr_end(c, begin, true);
    }
}

static void flecs_irc_compile_const(
    ecs_script_ir_compiler_t *c,
    ecs_script_var_node_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t value = flecs_irc_compile_tmp(c, node->expr);
    int32_t op = flecs_irc_emit(c, EcsIrConstEnd, value,
        node->computed - 1, 0, node);
    flecs_irc_op(c, op)->imm.ptr = node->eval_type
        ? ECS_CONST_CAST(void*, ecs_get_type_info(c->world, node->eval_type))
        : NULL;
    flecs_irc_expr_end(c, begin, true);
}

static void flecs_irc_compile_await(
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
        int32_t begin = flecs_irc_expr_begin(c, stmt);
        int32_t count, first = flecs_irc_compile_args(c, call, &count);
        int32_t launch = flecs_irc_emit(
            c, EcsIrAwaitLaunch, first, count, 0, stmt);
        flecs_irc_op(c, launch)->flags = flags;
        flecs_irc_op(c, launch)->imm.ptr = call;
        flecs_irc_expr_end(c, begin, true);
    }

    flecs_irc_op(c, check)->b = flecs_irc_pc(c);
    int32_t poll = flecs_irc_emit(c, EcsIrAwaitPoll, 0, 0, 0, stmt);
    flecs_irc_op(c, poll)->flags = flags;
}

static void flecs_irc_compile_entity(
    ecs_script_ir_compiler_t *c,
    ecs_script_entity_t *node)
{
    int32_t begin = node->name_expr ? flecs_irc_expr_begin(c, node) : -1;
    int32_t name = flecs_irc_compile_tmp(c, node->name_expr);
    int32_t op = flecs_irc_emit(c, EcsIrEntityEnter, name, 0, 0, node);
    if (name != -1) {
        flecs_irc_expr_end(c, begin, false);
        flecs_irc_op(c, op)->flags = EcsIrEndsExpr;
    }

    flecs_irc_compile_body(c, node->scope, EcsIrScopeEntity);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameEntity, 0, 0, node);
}

static void flecs_irc_compile_pair_scope(
    ecs_script_ir_compiler_t *c,
    ecs_script_pair_scope_t *node)
{
    flecs_irc_compile_id_op(c, node, &node->id, -1, EcsIrPairScopeEnter);
    flecs_irc_compile_body(c, node->scope, 0);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFramePairScope, 0, 0, node);
}

static void flecs_irc_compile_with(
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
            flecs_irc_compile_tag(c, (ecs_script_tag_t*)stmt, true);
        } else {
            ecs_assert(stmt->kind == EcsAstWithComponent,
                ECS_INTERNAL_ERROR, NULL);
            flecs_irc_compile_component(
                c, (ecs_script_component_t*)stmt, true);
        }
    }

    flecs_irc_emit(c, EcsIrWithBody, 0, 0, 0, node);
    flecs_irc_compile_body(c, node->scope, 0);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameWith, 0, 0, node);
}

static void flecs_irc_compile_if(
    ecs_script_ir_compiler_t *c,
    ecs_script_if_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t cond = flecs_irc_compile_tmp(c, node->expr);
    flecs_irc_emit(c, EcsIrToBool, cond, 0, 0, node);
    flecs_irc_expr_end(c, begin, true);

    int32_t enter = flecs_irc_emit(c, EcsIrIfEnter, 0, 0, 0, node);
    flecs_irc_compile_body(c, node->if_true, 0);
    int32_t jump = flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
    flecs_irc_op(c, enter)->b = flecs_irc_pc(c);
    flecs_irc_compile_body(c, node->if_false, 0);
    flecs_irc_op(c, jump)->a = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameIf, 0, 0, node);
}

static void flecs_irc_compile_for(
    ecs_script_ir_compiler_t *c,
    ecs_script_for_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t from = flecs_irc_compile_tmp(
        c, node->expr ? node->expr : node->from);
    int32_t to = flecs_irc_compile_tmp(c, node->expr ? NULL : node->to);
    flecs_irc_expr_end(c, begin, false);

    flecs_irc_emit(c, EcsIrForEnter, from, to, 0, node);
    int32_t next = flecs_irc_emit(c, EcsIrForNext, 0, 0, 0, node);

    c->force_depth ++;
    flecs_irc_compile_body(c, node->scope, 0);
    c->force_depth --;

    flecs_irc_emit(c, EcsIrJump, next, 0, 0, node);
    flecs_irc_op(c, next)->a = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameFor, 0, 0, node);
    flecs_irc_emit(c, EcsIrExprEnd, 0, 0, 0, node);
}

static void flecs_irc_compile_try(
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

    flecs_irc_compile_body(c, node->try_scope, 0);
    ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
        flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);

    for (i = 0; i < catch_count; i ++) {
        ecs_script_catch_t *catch_ = ecs_vec_get_t(
            &node->catches, ecs_script_catch_t, i);
        ecs_vec_get_t(&c->ir->catches, int32_t,
            catch_first + i)[0] = flecs_irc_pc(c);
        flecs_irc_compile_body(c, catch_->scope, 0);
        ecs_vec_append_t(NULL, &jumps, int32_t)[0] =
            flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
    }

    flecs_irc_patch_jumps(c, &jumps);
    flecs_irc_op(c, enter)->c = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameTry, 0, 0, node);
}

static void flecs_irc_compile_while(
    ecs_script_ir_compiler_t *c,
    ecs_script_while_t *node)
{
    flecs_irc_emit(c, EcsIrWhileEnter, 0, 0, 0, node);
    int32_t cond_pc = flecs_irc_pc(c);

    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t cond = flecs_irc_compile_tmp(c, node->expr);
    flecs_irc_emit(c, EcsIrToBool, cond, 0, 0, node);
    flecs_irc_expr_end(c, begin, true);

    int32_t next = flecs_irc_emit(c, EcsIrWhileNext, 0, 0, 0, node);

    c->force_depth ++;
    flecs_irc_compile_body(c, node->scope, 0);
    c->force_depth --;

    flecs_irc_emit(c, EcsIrJump, cond_pc, 0, 0, node);
    flecs_irc_op(c, next)->a = flecs_irc_pc(c);
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameWhile, 0, 0, node);
}

static void flecs_irc_compile_assign(
    ecs_script_ir_compiler_t *c,
    ecs_script_assign_t *node)
{
    int32_t begin = flecs_irc_expr_begin(c, node);
    int32_t value = flecs_irc_compile_tmp(c, node->expr);
    flecs_irc_emit(c, EcsIrAssign, value, 0, 0, node);
    flecs_irc_expr_end(c, begin, true);
}

static void flecs_irc_compile_stmt(
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
            (flecs_script_stmt_support(node) ? EcsIrStmtAlways : 0) |
            (node->skip ? EcsIrStmtSkip : 0));
    }

    if (node->skip) {
        flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
        return;
    }

    switch(node->kind) {
    case EcsAstScope:
        flecs_irc_compile_scope(c, (ecs_script_scope_t*)node, 0);
        break;
    case EcsAstTag:
        flecs_irc_compile_tag(c, (ecs_script_tag_t*)node, false);
        break;
    case EcsAstComponent:
        flecs_irc_compile_component(c, (ecs_script_component_t*)node, false);
        break;
    case EcsAstWithTag:
    case EcsAstWithComponent:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid context for with expression");
    case EcsAstWith:
        flecs_irc_compile_with(c, (ecs_script_with_t*)node);
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
    case EcsAstTemplate:
        flecs_irc_entry_add(c, node, EcsIrEntryTemplate);
        flecs_irc_emit(c, EcsIrTemplate, 0, 0, 0, node);
        break;
    case EcsAstProp:
        break;
    case EcsAstMut:
        flecs_irc_emit(c, EcsIrMutCheck, 0, 0, 0, node);
        break;
    case EcsAstConst: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (n->is_await) {
            flecs_irc_compile_await(c, node);
            break;
        }

        flecs_irc_compile_const(c, n);
        if (n->computed && stmt != -1) {
            int32_t jump = flecs_irc_emit(c, EcsIrJump, 0, 0, 0, node);
            flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
            flecs_irc_op(c, stmt)->flags |= EcsIrStmtCached;
            flecs_irc_emit(c, EcsIrConstCached,
                n->computed - 1, stmt + 1, 0, node);
            flecs_irc_op(c, jump)->a = flecs_irc_pc(c);
            stmt = -1;
        }

        break;
    }
    case EcsAstExportConst:
    case EcsAstExportMut:
        break;
    case EcsAstEntity: {
        ecs_script_entity_t *n = (ecs_script_entity_t*)node;
        if (!n->is_type) {
            flecs_irc_compile_entity(c, n);
            break;
        }

        flecs_irc_emit(c, EcsIrAnnotClear, 0, 0, 0, node);
        if (marks != -1) {
            flecs_irc_emit(c, EcsIrMark, 0, 0, marks, node);
        }

        break;
    }
    case EcsAstPairScope:
        flecs_irc_compile_pair_scope(c, (ecs_script_pair_scope_t*)node);
        break;
    case EcsAstIf:
        flecs_irc_compile_if(c, (ecs_script_if_t*)node);
        break;
    case EcsAstFor:
        flecs_irc_compile_for(c, (ecs_script_for_t*)node);
        break;
    case EcsAstInclude:
        break;
    case EcsAstFunction:
        flecs_irc_entry_add(c, node, EcsIrEntryFunction);
        break;
    case EcsAstAwait:
        flecs_irc_compile_await(c, node);
        break;
    case EcsAstTry:
        flecs_irc_compile_try(c, (ecs_script_try_t*)node);
        break;
    case EcsAstContinue:
        flecs_irc_emit(c, EcsIrContinue, 0, 0, 0, node);
        break;
    case EcsAstAsync:
        flecs_irc_entry_add(c, node, EcsIrEntryAsync);
        flecs_irc_emit(c, EcsIrAsync, 0, 0, 0, node);
        break;
    case EcsAstWhile:
        flecs_irc_compile_while(c, (ecs_script_while_t*)node);
        break;
    case EcsAstAssign:
        flecs_irc_compile_assign(c, (ecs_script_assign_t*)node);
        break;
    }

    if (stmt != -1) {
        flecs_irc_op(c, stmt)->b = flecs_irc_pc(c);
    }
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

static void flecs_irc_compile_scope(
    ecs_script_ir_compiler_t *c,
    ecs_script_scope_t *scope,
    uint16_t flags)
{
    int32_t enter = flecs_irc_emit(c, EcsIrScopeEnter, scope->scope_slot,
        flecs_irc_scope_var_count(scope), ecs_vec_count(&scope->components),
        scope);
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
        flecs_irc_compile_stmt(c, scope, i);

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
    flecs_irc_emit(c, EcsIrLeave, EcsIrFrameScope, 0, 0, scope);
}

static void flecs_irc_compile_function(
    ecs_script_ir_compiler_t *c,
    ecs_script_function_node_t *node)
{
    ecs_script_scope_t *body = node->body;
    body->parent = NULL;
    c->scope = body;

    int32_t i, count = ecs_vec_count(&body->stmts);
    for (i = 0; i < count; i ++) {
        flecs_irc_compile_stmt(c, body, i);
    }

    c->scope = NULL;
    c->reg_count = c->reg_floor;

    if (node->return_expr) {
        int32_t begin = flecs_irc_expr_begin(c, node);
        int32_t value = flecs_irc_compile_tmp(c, node->return_expr);
        flecs_irc_expr_end(c, begin, false);
        flecs_irc_emit(c, EcsIrReturn, value, 0, 0, node);
    }
}

static void flecs_irc_compile_entry(
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

    switch(kind) {
    case EcsIrEntryRoot:
        flecs_irc_compile_scope(
            c, ECS_CONST_CAST(ecs_script_scope_t*, node), 0);
        break;
    case EcsIrEntryTemplate:
        flecs_irc_compile_scope(c, ((const ecs_script_template_node_t*)
            node)->scope, 0);
        break;
    case EcsIrEntryEntity:
        flecs_irc_compile_entity(
            c, ECS_CONST_CAST(ecs_script_entity_t*, node));
        break;
    case EcsIrEntryFunction:
        flecs_irc_compile_function(
            c, ECS_CONST_CAST(ecs_script_function_node_t*, node));
        break;
    case EcsIrEntryAsync:
        flecs_irc_compile_scope(
            c, ((const ecs_script_async_t*)node)->scope, 0);
        break;
    }

    flecs_irc_emit(c, EcsIrEnd, 0, 0, 0, node);

    entry = ecs_vec_get_t(&c->ir->entries, ecs_script_ir_entry_t, index);
    entry->reg_count = c->reg_max;
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
        flecs_irc_compile_entry(&c, i);
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
