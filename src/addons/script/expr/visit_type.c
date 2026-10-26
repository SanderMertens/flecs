/**
 * @file addons/script/expr/visit_type.c
 * @brief Script expression type visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../../meta/meta.h"
#include "../script.h"

static int flecs_expr_visit_type_ex(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc,
    bool swizzle_expand_allowed);

static int flecs_expr_visit_type_priv(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    return flecs_expr_visit_type_ex(script, node_ptr, cur, desc, false);
}

static void flecs_expr_visit_type_cursor(
    const ecs_world_t *world,
    ecs_meta_cursor_t *cur,
    ecs_entity_t type)
{
    *cur = ecs_meta_cursor(world, type, NULL);
}

static int flecs_expr_cast_to(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_entity_t type)
{
    if ((*node_ptr)->type == type) {
        return 0;
    }

    ecs_expr_cast_t *cast = flecs_expr_cast(script, *node_ptr, type);
    if (!cast) {
        return -1;
    }

    *node_ptr = (ecs_expr_node_t*)cast;
    return 0;
}

static bool flecs_expr_unresolved_ref(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_expr_node_t *node,
    const char *name,
    flecs_script_unresolved_kind_t kind)
{
    ecs_script_eval_visitor_t *v = desc->script_visitor;
    if (!v || !v->type_visitor) {
        return false;
    }

    flecs_script_unresolved_ref_add(
        flecs_script_impl(script), node->pos, name, kind);
    return true;
}

static void flecs_expr_unresolved_component_ref(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_expr_node_t *node,
    ecs_expr_node_t *left,
    ecs_id_t component,
    bool is_has)
{
    ecs_script_eval_visitor_t *v = desc->script_visitor;
    if (!v || !v->type_visitor) {
        return;
    }

    if (left->kind == EcsExprIdentifier) {
        left = ((ecs_expr_identifier_t*)left)->expr;
    }

    if (!left || left->kind != EcsExprValue ||
        left->type != ecs_id(ecs_entity_t))
    {
        return;
    }

    ecs_entity_t src = ((ecs_expr_value_node_t*)left)->storage.entity;
    if (src && !ecs_has_id(script->world, src, component)) {
        ecs_script_impl_t *impl = flecs_script_impl(script);
        ecs_script_unresolved_component_ref_t *ref = ecs_vec_append_t(
            NULL, &impl->unresolved_component_refs,
            ecs_script_unresolved_component_ref_t);
        ref->entity = src;
        ref->component = component;
        ref->is_has = is_has;
        flecs_script_pos_to_line_col(impl->pub.code, node->pos,
            &ref->line, &ref->column);
    }
}

static bool flecs_expr_skip_unknown_unresolved_ref(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_expr_node_t *node,
    const char *name,
    flecs_script_unresolved_kind_t kind)
{
    if (!flecs_script_is_skip_unknown(script)) {
        return false;
    }

    return flecs_expr_unresolved_ref(script, desc, node, name, kind);
}

static bool flecs_expr_is_skipped_var(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    const char *name)
{
    if (!flecs_script_is_skip_unknown(script)) {
        return false;
    }

    ecs_script_eval_visitor_t *v = desc->script_visitor;
    if (!v || !v->type_visitor) {
        return false;
    }

    return flecs_script_type_is_skipped_var(v->type_visitor, name);
}

#define FLECS_EXPR_TYPE_INFO(T, K, I, S, F, E, R) \
    if (type == ecs_id(T)) { \
        static const flecs_expr_type_info_t info = { \
            K, ECS_SIZEOF(T), E, R, I, S, F}; \
        return &info; \
    }

const flecs_expr_type_info_t* flecs_expr_type_info(
    ecs_entity_t type)
{
    FLECS_EXPR_TYPE_INFO(ecs_bool_t, EcsBool, false, true, false, 1, 1)
    FLECS_EXPR_TYPE_INFO(ecs_char_t, EcsChar, false, true, false, 2, 1)
    FLECS_EXPR_TYPE_INFO(ecs_u8_t, EcsU8, true, false, false, 2, 2)
    FLECS_EXPR_TYPE_INFO(ecs_u16_t, EcsU16, true, false, false, 3, 3)
    FLECS_EXPR_TYPE_INFO(ecs_u32_t, EcsU32, true, false, false, 4, 4)
    FLECS_EXPR_TYPE_INFO(ecs_uptr_t, EcsUPtr, true, false, false, 5, 6)
    FLECS_EXPR_TYPE_INFO(ecs_u64_t, EcsU64, true, false, false, 6, 7)
    FLECS_EXPR_TYPE_INFO(ecs_i8_t, EcsI8, true, true, false, 7, 1)
    FLECS_EXPR_TYPE_INFO(ecs_i16_t, EcsI16, true, true, false, 8, 2)
    FLECS_EXPR_TYPE_INFO(ecs_i32_t, EcsI32, true, true, false, 9, 3)
    FLECS_EXPR_TYPE_INFO(ecs_iptr_t, EcsIPtr, true, true, false, 10, 5)
    FLECS_EXPR_TYPE_INFO(ecs_i64_t, EcsI64, true, true, false, 11, 6)
    FLECS_EXPR_TYPE_INFO(ecs_f32_t, EcsF32, false, false, true, 12, 3)
    FLECS_EXPR_TYPE_INFO(ecs_f64_t, EcsF64, false, false, true, 13, 4)
    FLECS_EXPR_TYPE_INFO(ecs_string_t, EcsString, false, false, false, -1, -1)
    FLECS_EXPR_TYPE_INFO(ecs_entity_t, EcsEntity, false, false, false, -1, -1)

    static const flecs_expr_type_info_t unknown = {
        .expressiveness = -1, .storage = -1 };
    return &unknown;
}

#undef FLECS_EXPR_TYPE_INFO

bool flecs_expr_is_type_integer(
    ecs_entity_t type)
{
    return flecs_expr_type_info(type)->integer;
}

bool flecs_expr_is_type_number(
    ecs_entity_t type)
{
    const flecs_expr_type_info_t *info = flecs_expr_type_info(type);
    return info->integer || info->floating_point;
}

bool flecs_expr_is_type_signed_integer(
    ecs_entity_t type)
{
    return flecs_expr_type_info(type)->signed_integer;
}

bool flecs_expr_is_type_unsigned_integer(
    ecs_entity_t type)
{
    const flecs_expr_type_info_t *info = flecs_expr_type_info(type);
    return info->integer && !info->signed_integer;
}

bool flecs_expr_is_type_float(
    ecs_entity_t type)
{
    return flecs_expr_type_info(type)->floating_point;
}

bool flecs_expr_is_type_string(
    ecs_entity_t type)
{
    return type == ecs_id(ecs_string_t);
}

ecs_size_t flecs_expr_storage_size(
    ecs_entity_t type)
{
    return flecs_expr_type_info(type)->size;
}

/* This function returns true if a type can be cast without changing the
 * precision of the value. It is used to determine a type for operands in a 
 * binary expression in case they are of different types. */
static bool flecs_expr_implicit_cast_allowed(
    ecs_entity_t from,
    ecs_entity_t to)
{
    const flecs_expr_type_info_t *from_info = flecs_expr_type_info(from);
    const flecs_expr_type_info_t *to_info = flecs_expr_type_info(to);
    if (from_info->expressiveness == -1 || to_info->expressiveness == -1) {
        return false;
    }

    return to_info->expressiveness >= from_info->expressiveness &&
        to_info->storage >= from_info->storage;
}

static ecs_entity_t flecs_expr_cast_to_lvalue(
    ecs_entity_t lvalue,
    ecs_entity_t operand)
{
    if (flecs_expr_implicit_cast_allowed(operand, lvalue)) {
        return lvalue;
    }

    return operand;
}

static ecs_entity_t flecs_expr_narrow_type(
    ecs_entity_t lvalue,
    ecs_expr_node_t *node)
{
    if (node->kind != EcsExprValue) {
        return node->type;
    }

    const flecs_expr_type_info_t *info = flecs_expr_type_info(node->type);
    if (!info->integer && !info->floating_point) {
        return node->type;
    }

    if (info->floating_point || info->kind == EcsIPtr || info->kind == EcsUPtr) {
        return lvalue == ecs_id(ecs_f32_t) ? lvalue : node->type;
    }

    double value = ecs_meta_ptr_to_float(
        info->kind, ((ecs_expr_value_node_t*)node)->ptr);
    if (info->signed_integer) {
        if (value >= INT8_MIN && value <= INT8_MAX) {
            return ecs_id(ecs_i8_t);
        } else if (value >= INT16_MIN && value <= INT16_MAX) {
            return ecs_id(ecs_i16_t);
        } else if (value >= INT32_MIN && value <= INT32_MAX) {
            return ecs_id(ecs_i32_t);
        }

        return ecs_id(ecs_i64_t);
    }

    if (value <= UINT8_MAX) {
        return ecs_id(ecs_u8_t);
    } else if (value <= UINT16_MAX) {
        return ecs_id(ecs_u16_t);
    } else if (value <= UINT32_MAX) {
        return ecs_id(ecs_u32_t);
    }

    return ecs_id(ecs_u64_t);
}

static bool flecs_expr_oper_is_arithmetic(
    ecs_token_kind_t op)
{
    switch(op) {
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokMod:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
        return true;
    default:
        return false;
    }
}

static bool flecs_expr_oper_valid_for_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_token_kind_t op)
{
    switch(op) {
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokMod:
    case EcsTokAddAssign:
    case EcsTokMulAssign:
        return flecs_expr_is_type_number(type);
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
        if (ecs_get(world, type, EcsBitmask) != NULL) {
            return true;
        }

        /* fall through */
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
        return flecs_expr_is_type_integer(type);
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokAnd:
    case EcsTokOr:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        return flecs_expr_is_type_number(type) ||
            (type == ecs_id(ecs_bool_t)) ||
            (type == ecs_id(ecs_char_t)) ||
            (type == ecs_id(ecs_entity_t));
    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static int32_t flecs_script_get_vector_type_data(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t *vector_type_out)
{
    const EcsStruct *stype = ecs_get(world, type, EcsStruct);
    if (!stype) {
        return 0;
    }

    ecs_entity_t vector_type = 0;
    ecs_member_t *members = ecs_vec_first(&stype->members);
    int32_t i, count = ecs_vec_count(&stype->members);
    for (i = 0; i < count; i ++) {
        ecs_member_t *member = &members[i];
        if (!i) {
            if (!ecs_owns(world, member->type, EcsPrimitive)) {
                /* Only primitive types can be used in vector ops */
                return 0;
            }

            vector_type = member->type;
        } else if (member->type != vector_type) {
            /* Only structs with members of the same type can be used as vector */
            return 0;
        }
    }

    *vector_type_out = vector_type;

    return count;
}

static int flecs_expr_type_for_operator(
    ecs_script_t *script,
    ecs_expr_node_t *node, /* Only used for error reporting */
    ecs_entity_t node_type,
    ecs_expr_node_t *left,
    ecs_expr_node_t *right,
    ecs_token_kind_t operator,
    ecs_entity_t *operand_type,
    ecs_entity_t *result_type,
    int32_t *vector_elem_count)
{
    ecs_world_t *world = script->world;

    if (operator == EcsTokDiv || operator == EcsTokMod) {
        if (right->kind == EcsExprValue && 
            flecs_expr_is_type_number(right->type)) 
        {
            ecs_expr_value_node_t *val = (ecs_expr_value_node_t*)right;
            ecs_value_t v = { .type = val->node.type, .ptr = val->ptr };
            if (flecs_value_is_0(&v)) {
                flecs_expr_visit_error(script, node, 
                    "invalid division by zero");
                return -1;
            }
        }
    }

    switch(operator) {
    case EcsTokDiv: 
        /* Result type of a division is always a float */
        if (left->type != ecs_id(ecs_f32_t) && left->type != ecs_id(ecs_f64_t)){
            *operand_type = ecs_id(ecs_f64_t);
            *result_type = ecs_id(ecs_f64_t);
        } else {
            *operand_type = left->type;
            *result_type = left->type;
        }

        return 0;
    case EcsTokMod:
        /* Mod only accepts integer operands, and results in an integer. We 
         * could disallow doing mod on floating point types, but in practice
         * that would likely just result in code having to do a manual 
         * conversion to an integer. */
        *operand_type = ecs_id(ecs_i64_t);
        *result_type = ecs_id(ecs_i64_t);
        return 0;
    case EcsTokAnd:
    case EcsTokOr:
        /* Result type of a conditional operator is always a bool */
        *operand_type = ecs_id(ecs_bool_t);
        *result_type = ecs_id(ecs_bool_t);
        return 0;
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        /* Result type of comparison operator is always bool, but operand types
         * should not be cast to bool */
        *result_type = ecs_id(ecs_bool_t);
        break;
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
        break;
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }

    /* If one of the types is an entity or id, the other one should be as well */
    if (left->type == ecs_id(ecs_entity_t) || 
        right->type == ecs_id(ecs_entity_t)) 
    {
        *operand_type = ecs_id(ecs_entity_t);
        goto done;
    }

    if (left->type == ecs_id(ecs_id_t) || right->type == ecs_id(ecs_id_t)) {
        *operand_type = ecs_id(ecs_id_t);
        goto done;
    }

    ecs_entity_t left_type = left->type;
    const EcsPrimitive *ltype_ptr = ecs_get(world, left_type, EcsPrimitive);
    const EcsPrimitive *rtype_ptr = ecs_get(world, right->type, EcsPrimitive);
    if (!ltype_ptr || !rtype_ptr) {
        if (ecs_has(world, left_type, EcsBitmask) ||
            ecs_has(world, right->type, EcsBitmask))
        {
            *operand_type = ecs_id(ecs_u32_t);
            goto done;
        }

        const EcsEnum *enum_type = ecs_get(world, left_type, EcsEnum);
        if (!enum_type) {
            enum_type = ecs_get(world, right->type, EcsEnum);
        }

        if (enum_type) {
            *operand_type = enum_type->underlying_type;
            goto done;
        }

        if (!ltype_ptr) {
            ecs_entity_t vector_type = 0;
            int32_t elem_count = flecs_script_get_vector_type_data(
                world, left_type, &vector_type);
            if (elem_count) {
                /* For vector operations the right operand type must be either a 
                 * primitive type or the same type as the left operand. */
                if (!rtype_ptr && (left_type != right->type)) {
                    flecs_expr_visit_error(script, node, 
                        "invalid types for binary expression (%s, %s)", 
                        flecs_errstr(ecs_get_path(world, left->type)),
                        flecs_errstr_1(ecs_get_path(world, right->type)));
                    goto error;
                }

                if (*result_type == ecs_id(ecs_bool_t)) {
                    flecs_expr_visit_error(script, node, 
                        "comparison operator is not supported for vector "
                        "types (%s, %s)", 
                        flecs_errstr(ecs_get_path(world, left->type)),
                        flecs_errstr_1(ecs_get_path(world, right->type)));
                    goto error;
                }

                /* This is a binary vector operation, check if it's allowed */
                if (!vector_elem_count) {
                    flecs_expr_visit_error(script, node, 
                        "binary vector operation is not allowed here (%s, %s)", 
                        flecs_errstr(ecs_get_path(world, left->type)),
                        flecs_errstr_1(ecs_get_path(world, right->type)));
                    goto error;
                }

                *result_type = left_type;
                *operand_type = vector_type;
                *vector_elem_count = elem_count;
                goto done;
            }
        }

        flecs_expr_visit_error(script, node, 
            "invalid types for binary expression (%s, %s)", 
            flecs_errstr(ecs_get_path(world, left->type)),
            flecs_errstr_1(ecs_get_path(world, right->type)));
        goto error;
    }

    /* If left and right types are the same, do nothing */
    if (left_type == right->type) {
        if ((operator == EcsTokEq || operator == EcsTokNeq) &&
            flecs_expr_is_type_float(left_type) &&
            left->kind == EcsExprValue && right->kind == EcsExprValue)
        {
            flecs_expr_visit_error(script, node,
                "floating point value is invalid in equality comparison");
            goto error;
        }

        *operand_type = left->type;
        goto done;
    }

    /* If types are not the same, find the smallest type for literals that can
     * represent the value without losing precision. */
    ecs_entity_t ltype = flecs_expr_narrow_type(node_type, left);
    ecs_entity_t rtype = flecs_expr_narrow_type(node_type, right);

    /* If types are not the same, try to implicitly cast to expression type */
    ltype = flecs_expr_cast_to_lvalue(node_type, ltype);
    rtype = flecs_expr_cast_to_lvalue(node_type, rtype);

    if (ltype == rtype) {
        *operand_type = ltype;
        goto done;
    }

    if (operator == EcsTokEq || operator == EcsTokNeq) {
        /* If this is an equality comparison and one of the operands is a bool, 
         * cast the other operand to a bool as well. This ensures that 
         * expressions such as true == 2 evaluate to true. */
        if (ltype == ecs_id(ecs_bool_t) || rtype == ecs_id(ecs_bool_t)) {
            *operand_type = ecs_id(ecs_bool_t);
            goto done;
        }

        bool ltype_is_flt = flecs_expr_is_type_float(ltype);
        bool rtype_is_flt = flecs_expr_is_type_float(rtype);

        if (ltype_is_flt || rtype_is_flt) {
            if (left->kind == EcsExprValue && right->kind == EcsExprValue) {
                flecs_expr_visit_error(script, node,
                    "floating point value is invalid in equality comparison");
                goto error;
            }

            if (ltype_is_flt && right->kind == EcsExprValue) {
                *operand_type = ltype;
                goto done;
            }

            if (rtype_is_flt && left->kind == EcsExprValue) {
                *operand_type = rtype;
                goto done;
            }
        }
    }

    /* If after the implicit cast types are not the same, try to implicitly cast
     * to the most expressive type. */
    if (flecs_expr_type_info(ltype)->expressiveness >=
        flecs_expr_type_info(rtype)->expressiveness)
    {
        if (flecs_expr_implicit_cast_allowed(rtype, ltype)) {
            *operand_type = ltype;
            goto done;
        }
    } else {
        if (flecs_expr_implicit_cast_allowed(ltype, rtype)) {
            *operand_type = rtype;
            goto done;
        }
    }

    /* If we get here, one or both operands cannot be coerced to the same type
     * while guaranteeing no loss of precision. Pick the type that's least
     * likely to cause trouble. */

    if (flecs_expr_is_type_number(ltype) && flecs_expr_is_type_number(rtype)) {

        /* If one of the types is a floating point type, use f64 */
        if (flecs_expr_is_type_float(ltype) ||
            flecs_expr_is_type_float(rtype))
        {
            *operand_type = ecs_id(ecs_f64_t);
            goto done;
        }

        /* If one of the types is a signed integer, use i64 */
        if (flecs_expr_is_type_signed_integer(ltype) ||
            flecs_expr_is_type_signed_integer(rtype))
        {
            *operand_type = ecs_id(ecs_i64_t);
            goto done;
        }
    }

    /* If all of that didn't work, give up */
    flecs_expr_visit_error(script, node,
        "incompatible types in expression (%s vs %s)",
        flecs_errstr(ecs_id_str(world, ltype)),
        flecs_errstr_1(ecs_id_str(world, rtype)));
error:
    return -1;

done:
    if (operator == EcsTokSub && *operand_type == ecs_id(ecs_u64_t)) {
        /* Result of subtracting two unsigned ints can be negative */
        *operand_type = ecs_id(ecs_i64_t);
    }

    if (*operand_type == ecs_id(ecs_bool_t) &&
        flecs_expr_oper_is_arithmetic(operator) &&
        (!vector_elem_count || !*vector_elem_count))
    {
        *operand_type = ecs_id(ecs_i32_t);
    }

    if (!*result_type) {
        *result_type = *operand_type;
    }

    if (ecs_get(script->world, *result_type, EcsBitmask) != NULL) {
        *operand_type = ecs_id(ecs_u64_t);
    }

    return 0;
}

static int flecs_expr_interpolated_string_visit_type(
    ecs_script_t *script,
    ecs_expr_interpolated_string_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_allocator_t *a = &flecs_script_impl(script)->allocator;
    char *ptr, *frag = NULL;
    char ch;

    for (ptr = node->value; (ch = ptr[0]); ) {
        if (ch == '\\') {
            ptr ++;
            if (ptr[0]) {
                ptr ++;
            }

            continue;
        }

        if ((ch == '$') && (isspace((unsigned char)ptr[1]) || !ptr[1])) {
            ptr ++;
            continue;
        }

        if (ch == '$' || ch == '{') {
            if (!frag) {
                frag = node->value;
            }

            char *frag_end = ptr;

            ecs_expr_fragment_t *fragment = ecs_vec_append_t(
                a, &node->fragments, ecs_expr_fragment_t);
            *fragment = (ecs_expr_fragment_t){.text = frag != ptr ? frag : NULL};
            ecs_expr_node_t **result = &fragment->expr;
            ecs_expr_format_t *format = &fragment->format;

            if (ch == '$') {
                char *var_name = ++ ptr;
                ptr = ECS_CONST_CAST(char*, flecs_tokenizer_identifier(
                    NULL, ptr, NULL));
                if (!ptr) {
                    goto error;
                }

                ecs_size_t var_name_pos = flecs_ito(int32_t, var_name - node->value);
                var_name = &node->buffer[var_name_pos];
                ecs_size_t var_name_end = flecs_ito(int32_t, ptr - node->value);
                node->buffer[var_name_end] = '\0';

                ecs_expr_variable_t *var = flecs_expr_variable_from(
                    script, (ecs_expr_node_t*)node, var_name);
                if (!var) {
                    goto error;
                }

                *result = (ecs_expr_node_t*)var;
            } else {
                ecs_script_impl_t *impl = flecs_script_impl(script);

                ecs_parser_t parser = {
                    .name = script->name,
                    .code = script->code,
                    .pos = node->node.pos,
                    .fixed_pos = node->node.pos,
                    .script = impl,
                    .scope = impl->root,
                    .significant_newline = false,
                    .token_cur = impl->token_remaining,
                    .token_end = &impl->token_buffer[impl->token_buffer_size]
                };

                ptr = ECS_CONST_CAST(char*, flecs_script_parse_expr(
                    &parser, ptr + 1, 0, result));
                if (!ptr) {
                    goto error;
                }

                impl->token_remaining = parser.token_cur;

                if (ptr[0] == ':') {
                    ptr = ECS_CONST_CAST(char*, flecs_expr_format_parse(
                        &parser, ptr + 1, format, desc));
                    if (!ptr) {
                        goto error;
                    }
                }

                if (ptr[0] != '}') {
                    flecs_expr_visit_error(script, node,
                        "expected '}' at end of interpolated expression");

                    goto error;
                }

                ptr ++;
            }

            ecs_assert(*result != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_expr_eval_desc_t priv_desc = *desc;
            priv_desc.type = format->is_present ? 0 : ecs_id(ecs_string_t);
            priv_desc.allow_unresolved_identifiers = true;

            ecs_meta_cursor_t value_cur = {0};
            ecs_meta_cursor_t *result_cur = format->is_present ? &value_cur : cur;
            if (flecs_expr_visit_type_priv(
                script, result, result_cur, &priv_desc))
            {
                goto error;
            }

            if (format->is_present) {
                ecs_entity_t value_type = (*result)->type;
                bool is_float = flecs_expr_is_type_float(value_type);
                bool is_integer =
                    flecs_expr_is_type_signed_integer(value_type) ||
                    flecs_expr_is_type_unsigned_integer(value_type);
                bool is_string = flecs_expr_is_type_string(value_type);

                const char *format_error = NULL;
                if (!is_float && !is_integer && !is_string) {
                    format_error = "format specifiers require a number or "
                        "string value";
                } else if (!is_float && format->precision) {
                    format_error = "precision is only supported for f32 and "
                        "f64 values";
                } else if (!is_float && format->notation) {
                    format_error = "scientific notation is only supported for "
                        "f32 and f64 values";
                } else if (is_string && format->sign) {
                    format_error = "sign is not supported for string values";
                }

                if (format_error) {
                    flecs_expr_visit_error(script, *result, "%s", format_error);

                    goto error;
                }

                ecs_expr_node_t **format_values[2] = {
                    &format->width, &format->precision
                };
                int32_t f;
                for (f = 0; f < 2; f ++) {
                    ecs_expr_node_t **format_value = format_values[f];
                    if (!format_value[0]) {
                        continue;
                    }

                    ecs_expr_eval_desc_t format_desc = *desc;
                    format_desc.type = 0;
                    ecs_meta_cursor_t format_cur = {0};
                    if (flecs_expr_visit_type_priv(script,
                        format_value, &format_cur, &format_desc))
                    {
                        goto error;
                    }

                    if (!flecs_expr_is_type_integer(format_value[0]->type)) {
                        flecs_expr_visit_error(script, format_value[0],
                            "format width and precision must be integers");

                        goto error;
                    }

                    if (flecs_expr_cast_to(
                        script, format_value, ecs_id(ecs_i32_t)))
                    {
                        goto error;
                    }
                }
            } else if (flecs_expr_cast_to(
                script, result, ecs_id(ecs_string_t)))
            {
                goto error;
            }

            frag_end[0] = '\0';

            frag = ptr;
            if (!ptr[0]) {
                break;
            }
        } else {
            ptr ++;
        }
    }

    ecs_assert(frag != NULL, ECS_INTERNAL_ERROR, NULL);

    if (frag != ptr) {
        ecs_vec_append_t(a, &node->fragments, ecs_expr_fragment_t)[0] =
            (ecs_expr_fragment_t){.text = frag};
    }

    ecs_expr_fragment_t *fragments = ecs_vec_first(&node->fragments);
    int32_t i, count = ecs_vec_count(&node->fragments);
    for (i = 0; i < count; i ++) {
        if (fragments[i].text && !flecs_string_escape(fragments[i].text)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_initializer_collection_check(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    ecs_meta_cursor_t *cur,
    ecs_entity_t init_type)
{
    ecs_world_t *world = script->world;
    if (cur) {
        bool is_collection = ecs_meta_is_collection(cur) ||
            ecs_meta_is_map(cur);
        if (is_collection != node->is_collection) {
            flecs_expr_visit_error(script, node, node->is_collection
                ? "invalid collection literal for non-collection type '%s'"
                : "invalid object literal for collection type '%s'",
                flecs_errstr(ecs_get_path(world, init_type)));
            goto error;
        }
    }

    ecs_entity_t type = node->node.type;
    if (type) {
        const EcsOpaque *op = ecs_get(world, type, EcsOpaque);
        if (op) {
            type = op->as_type;
        }

        const EcsType *ptr = ecs_get(world, type, EcsType);
        if (ptr) {
            ecs_type_kind_t kind = ptr->kind;
            /* Only do the collection check if no cursor is provided. Cursors
             * also handle inline arrays. */
            if (node->is_collection && !cur && kind != EcsArrayType &&
                kind != EcsVectorType && kind != EcsMapType)
            {
                flecs_expr_visit_error(script, node,
                    "invalid collection literal for type '%s'",
                    flecs_errstr(ecs_get_path(world, node->node.type)));
                goto error;
            }

            if (!node->is_collection && kind != EcsStructType &&
                kind != EcsValueType)
            {
                flecs_expr_visit_error(script, node,
                    "invalid object literal for type '%s'",
                    flecs_errstr(ecs_get_path(world, node->node.type)));
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

/* Dynamic initializers use the cursor API to assign values, and are used for 
 * any type where a simple list of offsets into fields doesn't work. */
static bool flecs_expr_initializer_is_dynamic(
    ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsType *t = ecs_get(world, type, EcsType);
    if (t) {
        return t->kind == EcsOpaqueType || t->kind == EcsVectorType ||
            t->kind == EcsMapType || t->kind == EcsValueType;
    }

    return false;
}

static int flecs_expr_empty_initializer_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    ecs_meta_cursor_t *cur)
{
    node->node.type = ecs_meta_get_type(cur);
    if (!node->node.type) {
        flecs_expr_visit_error(script, node,
            "unknown type for initializer");
        goto error;
    }

    node->is_dynamic = node->is_dynamic || flecs_expr_initializer_is_dynamic(
        script->world, node->node.type);

    if (ecs_meta_push(cur)) {
        goto error;
    }

    if (flecs_expr_initializer_collection_check(
        script, node, cur, node->node.type))
    {
        goto error;
    }

    if (ecs_meta_pop(cur)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static bool flecs_expr_member_swizzle_expand_hint(
    ecs_meta_cursor_t *cur,
    const char *name,
    uint16_t *dst)
{
    int32_t i, count = ecs_os_strlen(name);
    if (count < 2 || count > FLECS_EXPR_SWIZZLE_MAX) {
        return false;
    }

    ecs_meta_scope_t *scope = &cur->scope[cur->depth];
    if (scope->is_collection || scope->is_map || scope->is_value ||
        scope->opaque || !scope->ops)
    {
        return false;
    }

    int32_t ops_cur = scope->ops_cur;
    if ((ops_cur + count) > scope->ops_count) {
        return false;
    }

    ecs_meta_op_t *first = &scope->ops[ops_cur];
    if (first->kind <= EcsOpPrimitive) {
        return false;
    }

    for (i = 0; i < count; i ++) {
        ecs_meta_op_t *op = &scope->ops[ops_cur + i];
        if (op->kind <= EcsOpPrimitive || op->type != first->type) {
            return false;
        }

        ecs_size_t offset = op->offset - first->offset;
        if (offset < 0 || offset > UINT16_MAX) {
            return false;
        }

        dst[i] = flecs_ito(uint16_t, offset);
    }

    return true;
}

static int flecs_expr_unify_type(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_expr_node_t *elem,
    bool allow_cast,
    const char *elem_kind,
    int32_t index,
    const char *container)
{
    if (!node->type) {
        node->type = elem->type;
        return 0;
    }

    if (elem->type == node->type) {
        return 0;
    }

    if (flecs_expr_is_type_number(node->type)) {
        ecs_entity_t result_type = 0, operand_type = 0;
        if (flecs_expr_type_for_operator(script, node, 0, node, elem,
            EcsTokAdd, /* Use operator that doesn't change types */
            &operand_type, &result_type, NULL))
        {
            return -1;
        }

        /* "Accumulate" most expressive type in result node */
        node->type = result_type;
        return 0;
    }

    /* If type is not a number it must be castable to the result type */
    if (allow_cast && flecs_expr_explicit_cast_allowed(
        script->world, elem->type, node->type))
    {
        return 0;
    }

    flecs_expr_visit_error(script, node,
        "invalid type for %s %d in %s (got %s, expected %s)",
        elem_kind, index + 1, container,
        flecs_errstr(ecs_get_path(script->world, elem->type)),
        flecs_errstr_1(ecs_get_path(script->world, node->type)));
    return -1;
}

/* Infer the type of a collection literal without a target type. The most
 * expressive element type determines the vector type. */
static int flecs_expr_anonymous_collection_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);

    node->node.type = 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (elem->key) {
            flecs_expr_visit_error(script, node,
                "missing type for map literal");
            goto error;
        }

        if (!elem->value) {
            flecs_expr_visit_error(script, node,
                "missing value for initializer element");
            goto error;
        }

        ecs_meta_cursor_t elem_cur;
        ecs_os_zeromem(&elem_cur);
        if (flecs_expr_visit_type_priv(script, &elem->value, &elem_cur, desc)) {
            goto error;
        }

        if (flecs_expr_unify_type(script, &node->node, elem->value, false,
            "element", i, "collection literal"))
        {
            goto error;
        }
    }

    ecs_entity_t elem_type = node->node.type;
    node->node.type = 0;

    if (!elem_type) {
        flecs_expr_visit_error(script, node,
            "cannot infer element type for collection literal");
        goto error;
    }

    for (i = 0; i < count; i ++) {
        if (flecs_expr_cast_to(script, &elems[i].value, elem_type)) {
            goto error;
        }
    }

    ecs_entity_t vector_type = flecs_script_vector_type(
        script->world, elem_type);
    if (!vector_type) {
        flecs_expr_visit_error(script, node,
            "failed to create vector type for collection literal");
        goto error;
    }

    node->node.type = vector_type;
    node->is_dynamic = true;

    return 0;
error:
    return -1;
}

static int32_t flecs_expr_inline_elem_count(
    const ecs_meta_cursor_t *cur)
{
    ecs_meta_cursor_t elem_cur = *cur;
    int32_t count = 0;
    int prev_log = ecs_log_set_level(-4);
    if (!ecs_meta_push(&elem_cur) && ecs_meta_is_collection(&elem_cur)) {
        count = elem_cur.scope[elem_cur.depth - 1].elem_count;
    }

    ecs_log_set_level(prev_log);
    return count;
}

static bool flecs_expr_is_inline_array(
    ecs_world_t *world,
    const ecs_meta_cursor_t *cur,
    ecs_entity_t value_type,
    ecs_entity_t elem_type)
{
    const EcsArray *arr = ecs_get(world, value_type, EcsArray);
    return arr && arr->type == elem_type &&
        flecs_expr_inline_elem_count(cur) == arr->count;
}

static int flecs_expr_initializer_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_initializer_t *node = (ecs_expr_initializer_t*)node_ptr[0];

    if (!cur || !cur->valid) {
        if (node->is_collection) {
            return flecs_expr_anonymous_collection_visit_type(
                script, node, desc);
        }

        flecs_expr_visit_error(script, node, "missing type for initializer");
        goto error;
    }

    ecs_entity_t type = ecs_meta_get_type(cur);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);

    /* Opaque types do not have deterministic offsets */
    bool is_dynamic = node->is_dynamic ||
        flecs_expr_initializer_is_dynamic(script->world, type);
    node->is_dynamic = is_dynamic;

    if (ecs_meta_push(cur)) {
        goto error;
    }

    if (flecs_expr_initializer_collection_check(script, node, cur, type)) {
        goto error;
    }

    bool is_map = ecs_meta_is_map(cur);
    ecs_entity_t key_type = 0;
    if (is_map) {
        const EcsMap *map_type = ecs_get(script->world, type, EcsMap);
        ecs_assert(map_type != NULL, ECS_INTERNAL_ERROR, NULL);
        key_type = map_type->key_type;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    bool prev_removed = false;
    for (i = 0; i < count; i ++) {
        if (i && !prev_removed) {
            if (ecs_meta_next(cur)) { /* , */
                goto error;
            }
        }

        prev_removed = false;

        ecs_expr_initializer_element_t *elem = &elems[i];
        if (!elem->value) {
            flecs_expr_visit_error(script, node,
                "missing value for initializer element");
            goto error;
        }

        if (elem->key) {
            if (!is_map) {
                flecs_expr_visit_error(script, node,
                    "invalid key for non-map type '%s'",
                    flecs_errstr(ecs_get_path(script->world, type)));
                goto error;
            }

            ecs_meta_cursor_t key_cur = ecs_meta_cursor(
                script->world, key_type, NULL);
            if (flecs_expr_visit_type_priv(
                script, &elem->key, &key_cur, desc))
            {
                goto error;
            }

            if (flecs_expr_cast_to(script, &elem->key, key_type)) {
                goto error;
            }
        } else if (is_map) {
            flecs_expr_visit_error(script, node,
                "missing key for map initializer element");
            goto error;
        }

        if (elem->member) {
            bool skip_unknown = flecs_script_is_skip_unknown(script);
            ecs_meta_cursor_t member_cur = *cur;
            int prev_log = skip_unknown ? ecs_log_set_level(-4) : 0;
            int dotmember_result = ecs_meta_dotmember(cur, elem->member);
            if (skip_unknown) {
                ecs_log_set_level(prev_log);
            }

            if (dotmember_result) { /* x: */
                if (!skip_unknown) {
                    flecs_expr_visit_error(script, node,
                        "cannot resolve member");
                    goto error;
                }

                *cur = member_cur;
                flecs_script_skip_unknown_warn(script, elem->member,
                    "skipped value for unresolved member");
                flecs_expr_visit_free(script, elem->key);
                flecs_expr_visit_free(script, elem->value);
                ecs_vec_remove_ordered_t(&node->elements,
                    ecs_expr_initializer_element_t, i);
                elems = ecs_vec_first(&node->elements);
                count --;
                i --;
                prev_removed = true;
                continue;
            }
        }

        /* Check for "member: $" syntax */
        if (elem->value->kind == EcsExprVariable) {
            ecs_expr_variable_t *var = (ecs_expr_variable_t*)elem->value;
            if (var->name && !var->name[0]) {
                var->name = ecs_meta_get_member(cur);
                if (!var->name) {
                    flecs_expr_visit_error(script, node, 
                        "cannot deduce variable name: not a member");
                    goto error;
                }
            }
        }

        bool swizzle_expand_allowed = !elem->operator && !is_dynamic;

        if (is_dynamic && elem->value->kind == EcsExprInitializer) {
            /* A push for a nested initializer may have to allocate elements in
             * the parent collection value. */
            ((ecs_expr_initializer_t*)elem->value)->is_dynamic = true;
        }

        ecs_entity_t elem_type = ecs_meta_get_type(cur);
        ecs_meta_cursor_t elem_cur = *cur;
        if (elem_type == ecs_id(ecs_value_t) &&
            elem->value->kind != EcsExprInitializer &&
            elem->value->kind != EcsExprEmptyInitializer)
        {
            /* When assigning an expression to a value, derive the type of
             * the value from the expression. */
            ecs_os_zeromem(&elem_cur);
        }

        if (flecs_expr_visit_type_ex(
            script, &elem->value, &elem_cur, desc, swizzle_expand_allowed))
        {
            goto error;
        }

        if (elem_type == ecs_id(ecs_script_template_ref_t) &&
            elem->value->type == ecs_id(ecs_script_template_ref_t))
        {
            const ecs_expr_variable_t *var = flecs_expr_template_ref_var(
                elem->value);
            ecs_script_eval_visitor_t *v = desc ? desc->script_visitor : NULL;
            ecs_entity_t value_interface = var && v
                ? flecs_script_template_member_interface(v->template, var->sp)
                : 0;
            ecs_entity_t member_interface =
                flecs_script_template_prop_interface(script->world,
                    cur->scope[cur->depth].type, ecs_meta_get_member(cur));
            const char *expected = value_interface && member_interface
                ? flecs_script_template_interface_error(
                    script->world, value_interface, member_interface)
                : NULL;
            if (expected) {
                flecs_expr_visit_error(script, elem->value,
                    "'%s' passed to template prop '%s' is not %s",
                    var->name, ecs_meta_get_member(cur), expected);
                goto error;
            }
        }

        ecs_entity_t value_type = elem->value->type;
        if (value_type != elem_type && (elem->operator ||
            !flecs_expr_is_inline_array(
                script->world, cur, value_type, elem_type)))
        {
            bool single_value = count == 1 && !node->is_collection &&
                !elem->member && !elem->key && !elem->operator;
            if (single_value && (value_type == type ||
                flecs_struct_is_derived_from(script->world, value_type, type)))
            {
                ecs_expr_node_t *value = elem->value;
                if (flecs_expr_cast_to(script, &value, type)) {
                    goto error;
                }

                elem->value = NULL;
                if (ecs_meta_pop(cur)) {
                    goto error;
                }

                flecs_expr_visit_free(script, (ecs_expr_node_t*)node);
                *node_ptr = value;
                return 0;
            }

            if (single_value && ecs_has(script->world, value_type, EcsStruct) &&
                !flecs_expr_explicit_cast_allowed(
                    script->world, value_type, elem_type))
            {
                flecs_expr_visit_error(script, elem->value,
                    "cannot assign value of type %s to %s",
                    flecs_errstr(ecs_get_path(script->world, value_type)),
                    flecs_errstr_1(ecs_get_path(script->world, type)));
                goto error;
            }

            if (flecs_expr_cast_to(script, &elem->value, elem_type)) {
                goto error;
            }
        }

        if (elem->operator && !flecs_expr_oper_valid_for_type(
            script->world, elem_type, elem->operator))
        {
            flecs_expr_visit_error(script, node,
                "invalid operator for type '%s'",
                flecs_errstr(ecs_get_path(script->world, elem_type)));
            goto error;
        }

        if (!is_dynamic) {
            elem->offset = (uintptr_t)ecs_meta_get_ptr(cur);
        }

        ecs_expr_swizzle_t *swizzle = flecs_expr_expand_swizzle_get(elem->value);
        if (swizzle) {
            int32_t s;
            for (s = 1; s < swizzle->count; s ++) {
                if (ecs_meta_next(cur)) {
                    goto error;
                }
            }
        }
    }

    node->node.type = type;

    if (ecs_meta_pop(cur)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_unary_visit_type(
    ecs_script_t *script,
    ecs_expr_unary_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    if (flecs_expr_visit_type_priv(script, &node->expr, cur, desc)) {
        goto error;
    }

    ecs_entity_t type = ecs_id(ecs_bool_t);
    if (node->operator == EcsTokSub) {
        type = node->expr->type;
        if (!flecs_expr_is_type_number(type) && type != ecs_id(ecs_bool_t)) {
            flecs_expr_visit_error(script, node, "invalid operator - for type '%s'",
                flecs_errstr(ecs_get_path(script->world, type)));
            goto error;
        }

        if (type == ecs_id(ecs_bool_t) || flecs_expr_is_type_unsigned_integer(type)) {
            type = ecs_id(ecs_i64_t);
        }
    }

    node->node.type = type;

    if (flecs_expr_cast_to(script, &node->expr, type)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_binary_visit_type(
    ecs_script_t *script,
    ecs_expr_binary_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{    
    /* Operands must be of this type or cast to it */
    ecs_entity_t operand_type = 0;

    /* Resulting type of binary expression */
    ecs_entity_t result_type = 0;

    /* Number of elements in vector, if vector operation */
    int32_t vector_elem_count = 0;

    /* Operands are visited with a copy of the cursor. Visiting an operand can
     * move the cursor to the type of the operand, which must not be visible to
     * the expression that this binary expression is a part of. */
    ecs_allocator_t *a = &flecs_script_impl(script)->allocator;
    ecs_meta_cursor_t *operand_cur = flecs_alloc_n(a, ecs_meta_cursor_t, 2);
    *operand_cur = *cur;

    if (operand_cur->valid) {
        /* Provides a hint to the type visitor. The lvalue type will be used to
         * reduce the number of casts where possible. */
        node->node.type = ecs_meta_get_type(operand_cur);

        /* If the result of the binary expression is a boolean, it's likely a
         * conditional expression. We don't want to hint that the operands
         * of conditional expressions should be cast to booleans. */
        if (node->node.type == ecs_id(ecs_bool_t)) {
            ecs_os_zeromem(operand_cur);
        }
    }

    operand_cur[1] = operand_cur[0];

    if (flecs_expr_visit_type_priv(script, &node->left, operand_cur, desc)) {
        goto error;
    }

    if (!node->right) {
        flecs_expr_visit_error(script, node, "invalid binary expression");
        goto error;
    }

    if (node->right->kind == EcsExprIdentifier &&
        ecs_get(script->world, node->left->type, EcsEnum) != NULL)
    {
        /* If the left hand side is an enum, interpret untyped identifiers
         * on the right hand side as enum constants of the same type. */
        flecs_expr_visit_type_cursor(
            script->world, &operand_cur[1], node->left->type);
    }

    if (flecs_expr_visit_type_priv(
        script, &node->right, &operand_cur[1], desc))
    {
        goto error;
    }

    if (flecs_expr_type_for_operator(script, (ecs_expr_node_t*)node,
        node->node.type, node->left, node->right, node->operator,
        &operand_type, &result_type, &vector_elem_count))
    {
        goto error;
    }

    ecs_entity_t check_type = operand_type;
    if (!vector_elem_count && node->operator != EcsTokGt &&
        node->operator != EcsTokGtEq && node->operator != EcsTokLt &&
        node->operator != EcsTokLtEq)
    {
        check_type = result_type;
    }

    if (!flecs_expr_oper_valid_for_type(
        script->world, check_type, node->operator))
    {
        flecs_expr_visit_error(script, node,
            "invalid operator %s for type '%s'",
            flecs_token_str(node->operator),
            flecs_errstr(ecs_get_path(script->world, check_type)));
        goto error;
    }

    if (!vector_elem_count) {
        if (flecs_expr_cast_to(script, &node->left, operand_type)) {
            goto error;
        }

        node->vector_type = 0;
        node->vector_count = 0;
    } else {
        node->vector_type = operand_type;
        node->vector_count = vector_elem_count;
    }

    /* If this is a vector operation between the same types, don't try to cast
     * the right operand to the vector type. */
    if ((!vector_elem_count || (node->right->type != node->left->type)) &&
        flecs_expr_cast_to(script, &node->right, operand_type))
    {
        goto error;
    }

    node->node.type = result_type;

    flecs_free_n(a, ecs_meta_cursor_t, 2, operand_cur);
    return 0;
error:
    flecs_free_n(a, ecs_meta_cursor_t, 2, operand_cur);
    return -1;
}

static int flecs_expr_constant_identifier_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node)
{
    ecs_expr_value_node_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, node->node.type);

    ecs_meta_cursor_t expr_cur = ecs_meta_cursor(
        script->world, node->node.type, &result->storage.u64);
    if (ecs_meta_set_string(&expr_cur, node->value)) {
        flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
        goto error;
    }

    result->ptr = &result->storage.u64;
    node->expr = (ecs_expr_node_t*)result;

    return 0;
error:
    return -1;
}

static bool flecs_expr_is_entity_type(
    ecs_world_t *world,
    ecs_entity_t type,
    bool *is_opaque)
{
    *is_opaque = false;
    if (type == ecs_id(ecs_entity_t) || type == ecs_id(ecs_id_t)) {
        return true;
    }

    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    if (p) {
        return p->kind == EcsEntity || p->kind == EcsId;
    }

    const EcsOpaque *o = ecs_get(world, type, EcsOpaque);
    if (o) {
        *is_opaque = true;
        bool dummy;
        return flecs_expr_is_entity_type(world, o->as_type, &dummy);
    }

    return false;
}

static int flecs_expr_identifier_variable_member_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc,
    bool swizzle_expand_allowed)
{
    char *member_sep = strchr(node->value, '.');
    while (member_sep) {
        if (member_sep != node->value && member_sep[-1] == '\\') {
            member_sep = strchr(member_sep + 1, '.');
            continue;
        }

        member_sep[0] = '\0';

        flecs_script_symbol_t symbol;
        if (!flecs_script_symbol_lookup(script, desc, 0, node->value,
            FlecsScriptLookupAll, &symbol))
        {
            if (symbol.kind == FlecsScriptSymbolVariable) {
                break;
            }

            if (symbol.kind == FlecsScriptSymbolGlobalVariable &&
                symbol.entity && flecs_script_global_var_get(
                    script->world, symbol.entity, NULL).ptr)
            {
                break;
            }
        }

        member_sep[0] = '.';
        member_sep = strchr(member_sep + 1, '.');
    }

    if (!member_sep) {
        return 1;
    }

    char *var_name = flecs_strdup(
        &flecs_script_impl(script)->allocator, node->value);
    member_sep[0] = '.';

    ecs_expr_variable_t *var_node = flecs_expr_variable_from(
        script, (ecs_expr_node_t*)node, var_name);
    var_node->owns_name = true;
    ecs_expr_member_t *member_node = flecs_expr_member_from(
        script, (ecs_expr_node_t*)var_node, &member_sep[1]);

    node->expr = (ecs_expr_node_t*)member_node;

    if (flecs_expr_visit_type_ex(
        script, &node->expr, cur, desc, swizzle_expand_allowed))
    {
        goto error;
    }

    node->node.type = node->expr->type;
    return 0;
error:
    flecs_expr_visit_free(script, node->expr);
    node->expr = NULL;
    return -1;
}

static int flecs_expr_parent_member_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_script_eval_visitor_t *v = desc->script_visitor;
    ecs_script_template_t *template = v ? v->template : NULL;
    if (!template || !template->parent_type) {
        return 1;
    }

    const char *name = node->value;
    bool qualified = !ecs_os_strncmp(name, "parent.", 7);
    if (qualified) {
        name += 7;
    } else {
        const char *dot = strchr(name, '.');
        if (dot) {
            char *root = flecs_strdup(&flecs_script_impl(script)->allocator, name);
            root[dot - name] = '\0';
            bool masked = ecs_script_vars_lookup(desc->vars, root) != NULL;
            root[dot - name] = '.';
            flecs_strfree(&flecs_script_impl(script)->allocator, root);
            if (masked) {
                return 1;
            }
        }
    }

    ecs_entity_t component = 0;
    if (!flecs_script_template_parent_member(
        script->world, template, name, &component))
    {
        return 1;
    }

    ecs_expr_variable_t *parent = flecs_expr_variable_from(
        script, &node->node, "#parent");
    parent->sp = template->parent_sp;
    parent->node.type = ecs_id(ecs_entity_t);
    parent->node.type_info = ecs_get_type_info(script->world, parent->node.type);
    ecs_expr_element_t *ref = flecs_calloc_t(
        &flecs_script_impl(script)->allocator, ecs_expr_element_t);
    ref->node.kind = EcsExprComponent;
    ref->node.alloc_size = ECS_SIZEOF(ecs_expr_element_t);
    ref->node.pos = node->node.pos;
    ref->node.end = node->node.end;
    ref->node.type = component;
    ref->left = &parent->node;
    ecs_expr_value_node_t *index = flecs_expr_value_from(
        script, &node->node, ecs_id(ecs_entity_t));
    index->storage.entity = component;
    ref->index = &index->node;
    node->expr = &flecs_expr_member_from(script, &ref->node, name)->node;
    if (flecs_expr_visit_type_priv(script, &node->expr, cur, desc)) {
        return -1;
    }

    node->node.type = node->expr->type;
    return 0;
}

static int flecs_expr_identifier_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc,
    bool swizzle_expand_allowed)
{
    ecs_world_t *world = script->world;

    if (node->expr) {
        flecs_expr_visit_free(script, node->expr);
        node->expr = NULL;
    }

    node->symbol = -1;

    ecs_entity_t type = node->node.type;
    if (cur->valid) {
        type = ecs_meta_get_type(cur);
    }

    const EcsType *type_ptr = NULL;
    if (type) {
        type_ptr = ecs_get(world, type, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (type_ptr &&
       (type_ptr->kind == EcsEnumType || type_ptr->kind == EcsBitmaskType) &&
        ecs_lookup_child(world, type, node->value) != 0)
    {
        /* If the requested type is an enum or bitmask, use the cursor to resolve
         * the identifier to the correct type constant. This lets us type 'Red'
         * in places where we expect a value of type Color, instead of
         * Color.Red. If the identifier doesn't resolve to a constant, fall back
         * to regular lookup so variables can be used in enum/bitmask
         * contexts. */
        node->node.type = type;
        return flecs_expr_constant_identifier_visit_type(script, node);
    }

    flecs_script_symbol_t symbol;
    int lookup_result = flecs_script_symbol_lookup(
        script, desc, 0, node->value, FlecsScriptLookupAll, &symbol);
    if (!lookup_result && symbol.kind == FlecsScriptSymbolVariable) {
        ecs_script_var_t *var = ecs_script_vars_from_sp(desc->vars, symbol.sp);
        ecs_expr_variable_t *var_node = flecs_expr_variable_from(
            script, (ecs_expr_node_t*)node, node->value);
        var_node->sp = symbol.sp;
        node->expr = (ecs_expr_node_t*)var_node;
        node->node.type = var->value.type;
        return flecs_expr_visit_type_priv(script, &node->expr, cur, desc);
    }

    int parent_result = flecs_expr_parent_member_visit_type(
        script, node, cur, desc);
    if (parent_result != 1) {
        return parent_result;
    }

    if (!lookup_result || !ecs_os_strcmp(node->value, "#0")) {
        ecs_entity_t e = symbol.entity;
        if (e == EcsAny) {
            flecs_expr_visit_error(script, node,
                "cannot use anonymous entity as value");
            goto error;
        }

        bool is_slot = symbol.kind == FlecsScriptSymbolEntitySlot;
        ecs_value_t global = {0};
        if (e && !is_slot) {
            global = flecs_script_global_var_get(world, e, NULL);
        }

        if (global.ptr) {
            ecs_expr_variable_t *var_node = flecs_expr_variable_from(
                script, (ecs_expr_node_t*)node, node->value);
            node->expr = (ecs_expr_node_t*)var_node;
            node->node.type = global.type;
            return flecs_expr_visit_type_priv(script, &node->expr, cur, desc);
        }

        bool is_opaque = false;
        if (!type || type == ecs_id(ecs_value_t)) {
            type = ecs_id(ecs_entity_t);
        } else if (!flecs_expr_is_entity_type(world, type, &is_opaque)) {
            flecs_expr_visit_error(script, node,
                "cannot cast identifier '%s' to %s", node->value,
                flecs_errstr(ecs_get_path(world, type)));
            goto error;
        }

        node->node.type = type;

        if (is_slot) {
            node->symbol = symbol.slot;
            return 0;
        }

        ecs_expr_value_node_t *result = flecs_expr_value_from(
            script, (ecs_expr_node_t*)node, type);

        if (!is_opaque) {
            result->storage.entity = e;
            result->ptr = &result->storage.entity;
        } else {
            result->ptr = ecs_ptr_new_w_type_info(
                world, result->node.type_info);
            flecs_type_info_claim(result->node.type_info);

            ecs_meta_cursor_t expr_cur = ecs_meta_cursor(
                world, type, result->ptr);
            if (ecs_meta_set_entity(&expr_cur, e)) {
                flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
                goto error;
            }
        }

        node->expr = (ecs_expr_node_t*)result;
        return 0;
    }

    int var_member_result = flecs_expr_identifier_variable_member_visit_type(
        script, node, cur, desc, swizzle_expand_allowed);
    if (var_member_result != 1) {
        return var_member_result;
    }

    /* If unresolved identifiers aren't allowed here, throw error */
    if (!desc->allow_unresolved_identifiers ||
        flecs_expr_is_skipped_var(script, desc, node->value))
    {
        if (!flecs_expr_unresolved_ref(script, desc,
            (ecs_expr_node_t*)node, node->value,
            FlecsScriptUnresolvedEntity))
        {
            flecs_expr_visit_error(script, node,
                "unresolved identifier '%s'", node->value);
        }

        goto error;
    }

    /* Identifier will be resolved at eval time, default to entity */
    node->node.type = ecs_id(ecs_entity_t);
    return 0;
error:
    return -1;
}

static int flecs_expr_global_variable_resolve(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_world_t *world = script->world;
    flecs_script_symbol_t symbol;
    if (flecs_script_symbol_lookup(script, desc, 0, node->name,
        FlecsScriptLookupAll, &symbol) ||
        symbol.kind != FlecsScriptSymbolGlobalVariable)
    {
        if (!flecs_expr_unresolved_ref(script, desc,
            (ecs_expr_node_t*)node, node->name,
            FlecsScriptUnresolvedVariable))
        {
            flecs_expr_visit_error(script, node, "unresolved variable '%s'",
                node->name);
        }

        goto error;
    }

    ecs_entity_t global = symbol.entity;

    ecs_id_t component = 0;
    ecs_value_t value = flecs_script_global_var_get(world, global, &component);
    if (!value.ptr) {
        flecs_expr_visit_error(script, node,
            "entity '%s' is not a variable", node->name);
        goto error;
    }

    node->node.kind = EcsExprGlobalVariable;
    node->node.type = value.type;
    node->global = global;
    node->global_component = component;

    return 0;
error:
    return -1;
}

static int flecs_expr_variable_visit_type(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    flecs_script_symbol_t symbol;
    if (!flecs_script_symbol_lookup(script, desc, 0, node->name,
        FlecsScriptLookupVariable, &symbol))
    {
        ecs_script_var_t *var = ecs_script_vars_from_sp(
            desc->vars, symbol.sp);
        node->sp = symbol.sp;
        node->node.type = var->value.type;
        if (!node->node.type) {
            flecs_expr_visit_error(script, node, 
                "variable '%s' is not initialized", node->name);
            goto error;
        }
    } else {
        if (flecs_expr_global_variable_resolve(script, node, desc)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_arguments_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_expr_eval_desc_t *desc,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t *vector_type_out)
{
    ecs_script_parameter_t *params = ecs_vec_first(&func_data->params);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_entity_t vector_type = 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (!elem->value) {
            flecs_expr_visit_error(script, node, 
                "missing value for function argument");
            goto error;
        }

        ecs_entity_t argtype = params[i].type;

        if (argtype != EcsScriptVectorType) {
            ecs_meta_cursor_t cur = ecs_meta_cursor(
                script->world, argtype, NULL);

            if (flecs_expr_visit_type_priv(script, &elem->value, &cur, desc)){
                goto error;
            }
        } else {
            ecs_meta_cursor_t cur;
            ecs_os_zeromem(&cur);

            if (flecs_expr_visit_type_priv(script, &elem->value, &cur, desc)){
                goto error;
            }

            if (!vector_type) {
                vector_type = elem->value->type;
            }

            argtype = vector_type;

            /* Check if function provides implementation for type if argument
             * is of a primitive type. If it doesn't, try to cast. */
            const EcsPrimitive *p = ecs_get(
                script->world, argtype, EcsPrimitive);
            if (p) {
                if (!func_data->vector_callbacks[p->kind]) {
                    /* Fall back to types with max expressiveness */
                    if (func_data->vector_callbacks[EcsF64]) {
                        vector_type = argtype = ecs_id(ecs_f64_t);
                    } else if (func_data->vector_callbacks[EcsI64]) {
                        vector_type = argtype = ecs_id(ecs_i64_t);
                    } else if (func_data->vector_callbacks[EcsU64]) {
                        vector_type = argtype = ecs_id(ecs_u64_t);
                    } else {
                        /* No matching implementation. Error will be caught 
                         * later in the code. */
                    }
                }
            }
        }

        if (flecs_expr_cast_to(script, &elem->value, argtype)) {
            goto error;
        }
    }

    *vector_type_out = vector_type;

    return 0;
error:
    return -1;
}

static int flecs_expr_populate_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type)
{
    ecs_world_t *world = script->world;
    const EcsType *type = ecs_get(world, vector_type, EcsType);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t elem_type = vector_type;
    int32_t count = 1;
    switch(type->kind) {
    case EcsPrimitiveType:
        break;
    case EcsStructType:
        count = flecs_script_get_vector_type_data(
            world, vector_type, &elem_type);
        if (!count) {
            flecs_expr_visit_error(script, node,
                "type '%s' cannot be passed to vector argument of "
                "function '%s': all its members must be of the same "
                "primitive type",
                flecs_errstr(ecs_get_path(world, vector_type)),
                node->function_name);
            return -1;
        }
        break;
    case EcsArrayType: {
        const EcsArray *array = ecs_get(world, vector_type, EcsArray);
        elem_type = array->type;
        count = array->count;
        break;
    }
    case EcsVectorType:
        elem_type = ecs_get(world, vector_type, EcsVector)->type;
        count = -1;
        break;
    default:
        flecs_expr_visit_error(script, node,
            "type '%s' cannot be passed to vector argument of function '%s'",
            flecs_errstr(ecs_get_path(world, vector_type)),
            node->function_name);
        return -1;
    }

    const EcsPrimitive *ptype = ecs_get(world, elem_type, EcsPrimitive);
    if (!ptype) {
        flecs_expr_visit_error(script, node,
            "type '%s' cannot be passed to vector argument of "
            "function '%s': (member) type '%s' is not a primitive type",
            flecs_errstr(ecs_get_path(world, vector_type)),
            node->function_name,
            flecs_errstr_1(ecs_get_path(world, elem_type)));
        return -1;
    }

    ecs_vector_function_callback_t cb = func_data->vector_callbacks[ptype->kind];
    if (!cb) {
        flecs_expr_visit_error(script, node, "function '%s' does not implement "
            "matching vector operation for type '%s'",
            node->function_name,
            flecs_errstr(ecs_get_path(world, elem_type)));
        return -1;
    }

    node->calldata.is.vector_callback = cb;
    node->calldata.vector_elem_count = count;
    return 0;
}

const ecs_expr_variable_t* flecs_expr_template_ref_var(
    const ecs_expr_node_t *left)
{
    if (!left || left->type != ecs_id(ecs_script_template_ref_t)) {
        return NULL;
    }

    if (left->kind == EcsExprIdentifier) {
        left = ((const ecs_expr_identifier_t*)left)->expr;
    }

    if (left && left->kind == EcsExprVariable) {
        return (const ecs_expr_variable_t*)left;
    }

    return NULL;
}

static int flecs_expr_template_visit_type(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_world_t *world = script->world;
    ecs_entity_t tmpl = 0;
    const ecs_expr_variable_t *var = flecs_expr_template_ref_var(node->left);
    if (var) {
        ecs_script_eval_visitor_t *v = desc ? desc->script_visitor : NULL;
        tmpl = flecs_script_template_member_interface(
            v ? v->template : NULL, var->sp);
    } else {
        flecs_script_symbol_t symbol;
        if (flecs_script_symbol_lookup(script, desc, 0, node->function_name,
            FlecsScriptLookupEntity, &symbol) || !symbol.entity)
        {
            flecs_expr_visit_error(script, node,
                "unresolved template identifier '%s'", node->function_name);
            goto error;
        }

        tmpl = symbol.entity;
        const EcsScript *tmpl_script = ecs_get(world, tmpl, EcsScript);
        if (!tmpl_script || !tmpl_script->template_) {
            flecs_expr_visit_error(script, node,
                "'%s' is not a template", node->function_name);
            goto error;
        }
    }

    ecs_entity_t target = cur->valid ? ecs_meta_get_type(cur) : 0;
    bool is_ref = var || target == ecs_id(ecs_script_template_ref_t);
    const ecs_type_info_t *ti = tmpl ? ecs_get_type_info(world, tmpl) : NULL;

    node->node.kind = EcsExprTemplate;
    node->calldata.function = tmpl;
    node->node.type = is_ref ? ecs_id(ecs_script_template_ref_t) : tmpl;

    ecs_expr_initializer_t *args = node->args;
    ecs_assert(args != NULL, ECS_INTERNAL_ERROR, NULL);
    args->is_partial = true;
    int32_t i, count = ecs_vec_count(&args->elements);
    if (!count) {
        if (!is_ref && !ti) {
            flecs_expr_visit_error(script, node,
                "template '%s' has no props, cannot use as value",
                node->function_name);
            goto error;
        }

        args->node.type = tmpl;
        args->node.type_info = ti;
        return 0;
    }

    if (!tmpl) {
        flecs_expr_visit_error(script, node,
            "cannot pass arguments to template prop '%s' without interface",
            node->function_name);
        goto error;
    }

    const EcsStruct *st = ecs_get(world, tmpl, EcsStruct);
    int32_t member_count = st ? ecs_vec_count(&st->members) : 0;
    const ecs_member_t *members = st ? ecs_vec_first(&st->members) : NULL;
    int32_t positional = 0;
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&args->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member || elem->key) {
            continue;
        }

        if (positional >= member_count) {
            flecs_expr_visit_error(script, node,
                "too many arguments for template '%s'", node->function_name);
            goto error;
        }

        elem->member = members[positional ++].name;
    }

    ecs_meta_cursor_t args_cur = ecs_meta_cursor(world, tmpl, NULL);
    if (flecs_expr_visit_type_priv(
        script, (ecs_expr_node_t**)&node->args, &args_cur, desc))
    {
        goto error;
    }

    ecs_assert(node->args->node.type == tmpl, ECS_INTERNAL_ERROR, NULL);
    return 0;
error:
    return -1;
}

static int flecs_expr_function_visit_type(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    bool is_method = false;
    bool is_split = false;
    char *last_elem = NULL;
    const char *func_identifier = NULL;
    ecs_expr_member_t *replaced_member = NULL;

    if (node->left->kind == EcsExprIdentifier) {
        /* If identifier contains '.' separator(s), this is a method call,
         * otherwise it's a regular function. */
        ecs_expr_identifier_t *ident = (ecs_expr_identifier_t*)node->left;
        func_identifier = ident->value;

        last_elem = strrchr(func_identifier, '.');
        if (last_elem && last_elem != ident->value && last_elem[-1] != '\\') {
            node->function_name = last_elem + 1;
            last_elem[0] = '\0';
            is_method = true;
            is_split = true;
        } else {
            node->function_name = ident->value;
        }

    } else if (node->left->kind == EcsExprMember) {
        /* This is a method. Just like identifiers, method strings can contain
         * separators. Split off last separator to get the method. */
        ecs_expr_member_t *member = (ecs_expr_member_t*)node->left;
        last_elem = strrchr(member->member_name, '.');
        if (!last_elem) {
            node->left = member->left;
            node->function_name = member->member_name;
            replaced_member = member;
        } else {
            node->function_name = last_elem + 1;
            last_elem[0] = '\0';
            is_split = true;
        }

        is_method = true;
    }

    /* Left of function expression should not inherit lvalue type, since the
     * function return type is what's going to be assigned. */
    ecs_meta_cursor_t target_cur = *cur;
    ecs_os_zeromem(cur);

    if (flecs_expr_visit_type_priv(script, &node->left, cur, desc)) {
        goto error;
    }

    if (!is_method && flecs_expr_template_ref_var(node->left)) {
        return flecs_expr_template_visit_type(
            script, node, &target_cur, desc);
    }

    ecs_world_t *world = script->world;
    const struct ecs_script_function_t *func_data = NULL;
    ecs_entity_t func = 0;

    /* If this is a method, lookup function entity in scope of type */
    if (is_method) {
        flecs_script_symbol_t symbol;
        if (!flecs_script_symbol_lookup(script, desc, node->left->type,
            node->function_name,
            FlecsScriptLookupEntity | FlecsScriptLookupDynamic, &symbol))
        {
            func = symbol.entity;
        }

        if (!func) {
            const EcsType *type = ecs_get(world, node->left->type, EcsType);
            if (type && (type->kind == EcsArrayType ||
                type->kind == EcsVectorType || type->kind == EcsMapType))
            {
                ecs_entity_t collection = ecs_lookup(
                    world, "flecs.script.collection");
                func = ecs_lookup_child(world, collection, node->function_name);
            }
        }

        if (!func) {
            /* If identifier could be a function (not a method), try that */
            if (func_identifier) {
                is_method = false;
                is_split = false;
                last_elem[0] = '.';
                node->function_name = func_identifier;
                goto try_function;
            }

            if (flecs_expr_skip_unknown_unresolved_ref(script, desc,
                (ecs_expr_node_t*)node, node->function_name,
                FlecsScriptUnresolvedEntity))
            {
                goto error;
            }

            flecs_expr_visit_error(script, node,
                "unresolved method identifier '%s' for type '%s'",
                node->function_name,
                flecs_errstr(ecs_get_path(world, node->left->type)));
            goto error;
        }

        func_data = ecs_get(world, func, EcsScriptMethod);
        node->node.kind = EcsExprMethod;
    }

try_function:
    if (!is_method) {
        flecs_script_symbol_t symbol;
        if (flecs_script_symbol_lookup(script, desc, 0,
            node->function_name, FlecsScriptLookupEntity, &symbol) ||
            !symbol.entity)
        {
            if (!flecs_expr_skip_unknown_unresolved_ref(script, desc,
                (ecs_expr_node_t*)node, node->function_name,
                FlecsScriptUnresolvedEntity))
            {
                flecs_expr_visit_error(script, node,
                    "unresolved function identifier '%s'",
                    node->function_name);
            }

            goto error;
        }

        func = symbol.entity;

        const EcsScript *tmpl_script = ecs_get(world, func, EcsScript);
        if (tmpl_script && tmpl_script->template_) {
            return flecs_expr_template_visit_type(
                script, node, &target_cur, desc);
        }

        func_data = ecs_get(world, func, EcsScriptFunction);
    }

    if (!func_data) {
        flecs_expr_visit_error(script, node,
            "entity '%s' is not a valid method",
            flecs_errstr(ecs_get_path(world, func)));
        goto error;
    }

    node->node.type = func_data->return_type;
    node->calldata.function = func;
    node->calldata.is.callback = func_data->callback;
#ifdef FLECS_SCRIPT_ASYNC
    node->calldata.async_callback = func_data->async_callback;
    node->calldata.async_cancel = func_data->async_cancel;
#endif
    node->calldata.ctx = func_data->ctx;
    node->calldata.vector_elem_count = 0;

    int32_t count = ecs_vec_count(&node->args->elements);
    if (count != ecs_vec_count(&func_data->params)) {
        flecs_expr_visit_error(script, node, "expected %d arguments, got %d",
            ecs_vec_count(&func_data->params), count);
        goto error;
    }

    ecs_entity_t vector_type = 0;
    if (flecs_expr_arguments_visit_type(
        script, node->args, desc, func_data, &vector_type)) 
    {
        goto error;
    }

    if (vector_type) {
        if (node->node.type == EcsScriptVectorType) {
            node->node.type = vector_type;
        }

        if (flecs_expr_populate_vector_calldata(
            script, node, func_data, vector_type))
        {
            goto error;
        }
    } else {
        ecs_assert(node->node.type != EcsScriptVectorType,
            ECS_INTERNAL_ERROR, NULL);
    }

    if (replaced_member) {
        replaced_member->left = NULL; /* Prevent cleanup */
        flecs_expr_visit_free(script, (ecs_expr_node_t*)replaced_member);
    }

    return 0;
error:
    /* Undo mutations so the expression can be visited again, which can happen
     * when a script is evaluated multiple times. */
    if (is_split) {
        last_elem[0] = '.';
    }
    if (replaced_member) {
        node->left = (ecs_expr_node_t*)replaced_member;
        node->function_name = NULL;
    }

    if (node->node.kind == EcsExprMethod) {
        node->node.kind = EcsExprFunction;
    }

    return -1;
}

static int flecs_expr_member_try_swizzle(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_entity_t left_type,
    ecs_entity_t target_type,
    const uint16_t *expand_dst)
{
    ecs_expr_member_t *node = (ecs_expr_member_t*)*node_ptr;
    ecs_world_t *world = script->world;

    ecs_entity_t elem_type = 0;
    int32_t i, member_count = flecs_script_get_vector_type_data(
        world, left_type, &elem_type);
    if (!member_count) {
        return 1;
    }

    const EcsStruct *stype = ecs_get(world, left_type, EcsStruct);
    ecs_member_t *members = ecs_vec_first(&stype->members);
    for (i = 0; i < member_count; i ++) {
        if (!members[i].name[0] || members[i].name[1]) {
            return 1;
        }
    }

    const char *name = node->member_name;
    int32_t swizzle_count = ecs_os_strlen(name);
    if (!swizzle_count || swizzle_count > FLECS_EXPR_SWIZZLE_MAX) {
        return 1;
    }

    uint16_t offsets[FLECS_EXPR_SWIZZLE_MAX];
    for (i = 0; i < swizzle_count; i ++) {
        int32_t m;
        for (m = 0; m < member_count; m ++) {
            if (members[m].name[0] == name[i]) {
                break;
            }
        }

        if (m == member_count) {
            return 1;
        }

        offsets[i] = flecs_ito(uint16_t, members[m].offset);
    }

    ecs_entity_t result_type = 0;
    bool expand = false;
    if (target_type) {
        ecs_entity_t target_elem_type = 0;
        int32_t target_count = flecs_script_get_vector_type_data(
            world, target_type, &target_elem_type);
        if (target_count == swizzle_count && target_elem_type == elem_type) {
            result_type = target_type;
        } else if (expand_dst && elem_type == target_type) {
            result_type = target_type;
            expand = true;
        } else {
            flecs_expr_visit_error(script, node,
                "swizzle '%s' of type '%s' is incompatible with type '%s'",
                name, flecs_errstr(ecs_get_path(world, left_type)),
                flecs_errstr_1(ecs_get_path(world, target_type)));
            return -1;
        }
    } else {
        if (swizzle_count != member_count) {
            flecs_expr_visit_error(script, node,
                "cannot determine type for swizzle '%s'", name);
            return -1;
        }

        result_type = left_type;
    }

    const ecs_type_info_t *eti = ecs_get_type_info(world, elem_type);
    ecs_assert(eti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_expr_swizzle_t *swizzle = flecs_expr_swizzle_from(
        script, (ecs_expr_node_t*)node, node->left, name);
    swizzle->count = swizzle_count;
    swizzle->elem_size = eti->size;
    ecs_os_memcpy_n(swizzle->src, offsets, uint16_t, swizzle_count);
    swizzle->node.type = result_type;

    if (expand) {
        ecs_os_memcpy_n(swizzle->dst, expand_dst, uint16_t, swizzle_count);
        swizzle->expand = true;
    }

    node->left = NULL;
    flecs_expr_visit_free(script, (ecs_expr_node_t*)node);

    *node_ptr = (ecs_expr_node_t*)swizzle;

    return 0;
}

static int flecs_expr_member_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc,
    bool swizzle_expand_allowed)
{
    ecs_expr_member_t *node = (ecs_expr_member_t*)*node_ptr;

    ecs_entity_t target_type = 0;
    if (cur && cur->valid) {
        target_type = ecs_meta_get_type(cur);
    }

    uint16_t expand_dst[FLECS_EXPR_SWIZZLE_MAX];
    bool can_expand = false;
    if (swizzle_expand_allowed && cur && cur->valid) {
        can_expand = flecs_expr_member_swizzle_expand_hint(
            cur, node->member_name, expand_dst);
    }

    if (flecs_expr_visit_type_priv(script, &node->left, cur, desc)) {
        goto error;
    }

    ecs_world_t *world = script->world;
    ecs_entity_t left_type = node->left->type;

    if (left_type == ecs_id(ecs_script_template_ref_t)) {
        const ecs_expr_variable_t *var = flecs_expr_template_ref_var(
            node->left);
        ecs_script_eval_visitor_t *v = desc ? desc->script_visitor : NULL;
        ecs_entity_t interface = var ? flecs_script_template_member_interface(
            v ? v->template : NULL, var->sp) : 0;
        if (!interface || !ecs_has(world, interface, EcsStruct)) {
            flecs_expr_visit_error(script, node,
                "cannot resolve member '%s' on template prop without "
                "interface", node->member_name);
            goto error;
        }

        if (flecs_expr_cast_to(script, &node->left, interface)) {
            goto error;
        }

        left_type = interface;
        *cur = ecs_meta_cursor(world, interface, NULL);
    }

    const EcsType *type = ecs_get(world, left_type, EcsType);
    if (!type) {
        flecs_expr_visit_error(script, node,
            "cannot resolve member on value of type '%s' "
            "(missing reflection data)",
            flecs_errstr(ecs_get_path(world, left_type)));
        goto error;
    }

    if (type->kind != EcsStructType) {
        flecs_expr_visit_error(script, node,
            "cannot resolve member on non-struct type '%s'",
            flecs_errstr(ecs_get_path(world, left_type)));
        goto error;
    }

    if (ecs_meta_push(cur)) {
        goto error;
    }

    int prev_log = ecs_log_set_level(-4);
    if (ecs_meta_dotmember(cur, node->member_name)) {
        ecs_log_set_level(prev_log);

        int swizzle = flecs_expr_member_try_swizzle(
            script, node_ptr, left_type, target_type,
            can_expand ? expand_dst : NULL);
        if (swizzle <= 0) {
            return swizzle;
        }

        if (flecs_expr_skip_unknown_unresolved_ref(script, desc,
            (ecs_expr_node_t*)node, node->member_name,
            FlecsScriptUnresolvedEntity))
        {
            goto error;
        }

        flecs_expr_visit_error(script, node,
            "unresolved member '%s' for type '%s'",
            node->member_name,
            flecs_errstr(ecs_get_path(world, left_type)));
        goto error;
    }

    ecs_log_set_level(prev_log);

    node->node.type = ecs_meta_get_type(cur);
    node->offset = (uintptr_t)ecs_meta_get_ptr(cur);

    /* Members with an inline array count ("values: i32[3]") report the element
     * type. Expose the member as an array type so the value can be used as a
     * collection. */
    const EcsType *member_type = ecs_get(world, node->node.type, EcsType);
    if (!member_type || (member_type->kind != EcsArrayType &&
        member_type->kind != EcsVectorType &&
        member_type->kind != EcsMapType))
    {
        int32_t elem_count = flecs_expr_inline_elem_count(cur);
        if (elem_count > 0) {
            ecs_entity_t array_type = flecs_script_array_type(
                world, node->node.type, elem_count);
            if (!array_type) {
                goto error;
            }

            node->node.type = array_type;
        }
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_element_visit_type(
    ecs_script_t *script,
    ecs_expr_element_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_world_t *world = script->world;

    /* Check if this is a component expression */
    if (node->index->kind == EcsExprIdentifier) {
        /* Fetch type of left side of expression to check if it's of an entity
         * type. Pass in an empty cursor object so we don't fail type checks in
         * case it's a regular element expression. */
        ecs_meta_cursor_t tmp_cur = {0};
        if (flecs_expr_visit_type_priv(script, &node->left, &tmp_cur, desc)) {
            goto error;
        }

        ecs_entity_t left_type = node->left->type;
        if (!ecs_has(world, left_type, EcsType)) {
            flecs_expr_visit_error(script, node,
                "cannot use [] on value of type '%s' (missing reflection data)",
                flecs_errstr(ecs_get_path(world, left_type)));
            goto error;
        }

        const EcsPrimitive *ptype = ecs_get(world, left_type, EcsPrimitive);
        if (ptype && ptype->kind == EcsEntity) {
            ecs_meta_cursor_t index_cur = {0};
            if (flecs_expr_visit_type_priv(
                script, &node->index, &index_cur, desc))
            {
                goto error;
            }

            ecs_expr_identifier_t *ident = (ecs_expr_identifier_t*)node->index;
            flecs_script_symbol_t symbol;
            if (flecs_script_symbol_lookup(script, desc, 0, ident->value,
                FlecsScriptLookupEntity, &symbol) || !symbol.entity)
            {
                if (!flecs_expr_skip_unknown_unresolved_ref(script, desc,
                    (ecs_expr_node_t*)node, ident->value,
                    FlecsScriptUnresolvedEntity))
                {
                    flecs_expr_visit_error(script, node,
                        "unresolved component identifier '%s'",
                            ident->value);
                }

                goto error;
            }

            node->node.type = symbol.entity;

            if (!ecs_get_type_info(world, node->node.type)) {
                if (flecs_expr_skip_unknown_unresolved_ref(script, desc,
                    (ecs_expr_node_t*)node, ident->value,
                    FlecsScriptUnresolvedEntity))
                {
                    goto error;
                }

                flecs_expr_visit_error(script, node,
                    "cannot use [] with component '%s' "
                    "(missing reflection data)",
                    flecs_errstr(ecs_get_path(world, node->node.type)));
                goto error;
            }

            node->node.kind = EcsExprComponent;

            flecs_expr_unresolved_component_ref(script, desc, &node->node,
                node->left, node->node.type, false);

            return 0;
        }
    }

    if (flecs_expr_visit_type_priv(script, &node->left, cur, desc)) {
        goto error;
    }

    if (!cur->valid) {
        goto not_a_collection;
    }

    if (ecs_meta_push(cur)) {
        goto not_a_collection;
    }

    bool is_map = ecs_meta_is_map(cur);
    if (!is_map && !ecs_meta_is_collection(cur)) {
        goto not_a_collection;
    }

    ecs_entity_t index_type = ecs_id(ecs_i64_t);
    ecs_meta_cursor_t index_cur = {0};
    if (is_map) {
        const EcsMap *map_type = ecs_get(world, node->left->type, EcsMap);
        ecs_assert(map_type != NULL, ECS_INTERNAL_ERROR, NULL);
        index_type = map_type->key_type;
        index_cur = ecs_meta_cursor(world, index_type, NULL);
    }

    if (flecs_expr_visit_type_priv(script, &node->index, &index_cur, desc) ||
        flecs_expr_cast_to(script, &node->index, index_type))
    {
        goto error;
    }

    node->node.type = ecs_meta_get_type(cur);
    node->elem_size = ecs_get_type_info(world, node->node.type)->size;
    node->elem_count = is_map ? 0 : cur->scope[cur->depth - 1].elem_count;

    return 0;
not_a_collection:
    flecs_expr_visit_error(script, node,
        "invalid usage of [] on non collection/entity type '%s'",
        flecs_errstr(ecs_get_path(world, node->left->type)));
error:
    return -1;
}

static int flecs_expr_has_id_elem_name(
    ecs_script_t *script,
    ecs_expr_has_t *node,
    ecs_expr_node_t *elem,
    const char **name)
{
    if (elem->kind != EcsExprIdentifier) {
        flecs_expr_visit_error(script, node,
            "expected component identifier in ?[] expression");
        return -1;
    }

    *name = ((ecs_expr_identifier_t*)elem)->value;
    return 0;
}

static int flecs_expr_has_visit_type(
    ecs_script_t *script,
    ecs_expr_has_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    const char *first_name = NULL, *second_name = NULL;
    if (flecs_expr_has_id_elem_name(script, node, node->first, &first_name)) {
        goto error;
    }

    if (node->second && flecs_expr_has_id_elem_name(
        script, node, node->second, &second_name))
    {
        goto error;
    }

    ecs_entity_t first = 0;
    const char *unresolved = NULL;
    if (flecs_script_id_lookup(script, desc, first_name, second_name,
        FlecsScriptLookupEntity, &first, &node->id, &unresolved))
    {
        if (!flecs_expr_unresolved_ref(script, desc,
            (ecs_expr_node_t*)node, unresolved,
            FlecsScriptUnresolvedEntity))
        {
            flecs_expr_visit_error(script, node,
                "unresolved component identifier '%s'", unresolved);
        }

        goto error;
    }

    if (node->left->kind == EcsExprVariable &&
        !((ecs_expr_variable_t*)node->left)->name[0])
    {
        /* Singleton expression. Replace the $ with the entity that stores the
         * singleton component, which is the first element of the id. */
        flecs_expr_visit_free(script, node->left);
        ecs_expr_value_node_t *left = flecs_expr_value_from(
            script, (ecs_expr_node_t*)node, ecs_id(ecs_entity_t));
        left->storage.entity = first;
        left->ptr = &left->storage.entity;
        node->left = (ecs_expr_node_t*)left;
    } else {
        ecs_meta_cursor_t left_cur = {0};
        if (flecs_expr_visit_type_priv(script, &node->left, &left_cur, desc)) {
            goto error;
        }

        bool is_opaque = false;
        if (!flecs_expr_is_entity_type(
            script->world, node->left->type, &is_opaque) || is_opaque)
        {
            flecs_expr_visit_error(script, node,
                "invalid usage of ?[] on non-entity type '%s'",
                flecs_errstr(ecs_get_path(script->world, node->left->type)));
            goto error;
        }
    }

    flecs_expr_unresolved_component_ref(script, desc, &node->node,
        node->left, node->id, true);

    node->node.type = ecs_id(ecs_bool_t);

    return 0;
error:
    return -1;
}

static bool flecs_expr_identifier_is_any(
    ecs_expr_node_t *node)
{
    if (node->kind == EcsExprIdentifier) {
        ecs_expr_identifier_t *id = (ecs_expr_identifier_t*)node;
        if (id->value && !ecs_os_strcmp(id->value, "_")) {
            return true;
        }
    }

    return false;
}

static int flecs_expr_match_visit_type(
    ecs_script_t *script,
    ecs_expr_match_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_meta_cursor_t expr_cur;
    ecs_os_zeromem(&expr_cur);
    if (flecs_expr_visit_type_priv(script, &node->expr, &expr_cur, desc)) {
        goto error;
    }

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    if (!count) {
        flecs_expr_visit_error(script, node, 
            "match statement must have at least one case");
        goto error;
    }

    /* Determine most expressive type of all elements */
    node->node.type = ecs_meta_get_type(cur);

    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];

        if (node->node.type) {
            expr_cur = ecs_meta_cursor(script->world, node->node.type, NULL);
        } else {
            ecs_os_zeromem(&expr_cur);
        }

        if (flecs_expr_visit_type_priv(script, &elem->expr, &expr_cur, desc)) {
            goto error;
        }

        if (flecs_expr_unify_type(script, &node->node, elem->expr, true,
            "case", i, "match"))
        {
            goto error;
        }
    }

    /* Loop over elements again, cast values to result type */
    for (i = 0; i < count; i ++) {
        if (flecs_expr_cast_to(script, &elems[i].expr, node->node.type)) {
            goto error;
        }
    }

    /* If this is an enum type, cast to the underlying type. This is necessary
     * because the compare operation executed by the match evaluation code isn't
     * implemented for enums. Bitmasks are always cast to u32. */
    ecs_entity_t expr_type = node->expr->type;
    const EcsEnum *enum_type = ecs_get(script->world, expr_type, EcsEnum);
    ecs_entity_t compare_type = expr_type;
    if (enum_type) {
        compare_type = enum_type->underlying_type;
    } else if (ecs_has(script->world, expr_type, EcsBitmask)) {
        compare_type = ecs_id(ecs_u32_t);
    }

    if (flecs_expr_cast_to(script, &node->expr, compare_type)) {
        goto error;
    }

    /* Make sure that case values match the input type */
    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];

        if (flecs_expr_identifier_is_any(elem->compare)) {
            if (i != count - 1) {
                flecs_expr_visit_error(script, node, 
                    "any (_) must be the last case in match");
                goto error;
            }

            node->any.compare = elem->compare;
            node->any.expr = elem->expr;
            ecs_vec_remove_last(&node->elements);
        } else {
            expr_cur = ecs_meta_cursor(script->world, expr_type, NULL);
            if (flecs_expr_visit_type_priv(
                script, &elem->compare, &expr_cur, desc)) 
            {
                goto error;
            }

            if (flecs_expr_cast_to(
                script, &elem->compare, node->expr->type))
            {
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_range_visit_type(
    ecs_script_t *script,
    ecs_expr_range_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_node_t **bounds[2] = { &node->from, &node->to };
    int32_t i;
    for (i = 0; i < 2; i ++) {
        ecs_meta_cursor_t bound_cur = ecs_meta_cursor(
            script->world, ecs_id(ecs_i32_t), NULL);
        if (flecs_expr_visit_type_priv(script, bounds[i], &bound_cur, desc) ||
            flecs_expr_cast_to(script, bounds[i], ecs_id(ecs_i32_t)))
        {
            goto error;
        }
    }

    ecs_entity_t type = 0;
    if (cur && cur->valid) {
        type = ecs_meta_get_type(cur);
    }

    if (type && type != ecs_id(ecs_value_t)) {
        const EcsType *ptr = ecs_get(script->world, type, EcsType);
        if (!ptr || ptr->kind != EcsVectorType) {
            flecs_expr_visit_error(script, node,
                "cannot assign range to non-vector type '%s'",
                flecs_errstr(ecs_get_path(script->world, type)));
            goto error;
        }

        node->node.type = type;
    } else {
        node->node.type = flecs_script_vector_type(
            script->world, ecs_id(ecs_i32_t));
        if (!node->node.type) {
            flecs_expr_visit_error(script, node,
                "failed to create vector type for range");
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_new_visit_type(
    ecs_script_t *script,
    ecs_expr_new_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_script_eval_visitor_t *v = desc->script_visitor;
    if (flecs_script_visit_type_entity_expr(
        script, desc, v, node->entity))
    {
        return -1;
    }

    node->node.type = ecs_id(ecs_entity_t);
    return 0;
}

static int flecs_expr_visit_type_ex(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc,
    bool swizzle_expand_allowed)
{
    ecs_assert(node_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_expr_node_t *node = *node_ptr;
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    int r = 0;
    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprInterpolatedString:
        r = flecs_expr_interpolated_string_visit_type(
            script, (ecs_expr_interpolated_string_t*)node, cur, desc);
        break;
    case EcsExprEmptyInitializer:
        r = flecs_expr_empty_initializer_visit_type(
            script, (ecs_expr_initializer_t*)node, cur);
        break;
    case EcsExprInitializer:
        r = flecs_expr_initializer_visit_type(script, node_ptr, cur, desc);
        break;
    case EcsExprUnary:
        r = flecs_expr_unary_visit_type(
            script, (ecs_expr_unary_t*)node, cur, desc);
        break;
    case EcsExprBinary:
        r = flecs_expr_binary_visit_type(
            script, (ecs_expr_binary_t*)node, cur, desc);
        break;
    case EcsExprIdentifier:
        r = flecs_expr_identifier_visit_type(
            script, (ecs_expr_identifier_t*)node, cur, desc,
            swizzle_expand_allowed);
        break;
    case EcsExprVariable:
        r = flecs_expr_variable_visit_type(
            script, (ecs_expr_variable_t*)node, desc);
        break;
    case EcsExprGlobalVariable:
        r = flecs_expr_global_variable_resolve(
            script, (ecs_expr_variable_t*)node, desc);
        break;
    case EcsExprFunction:
        r = flecs_expr_function_visit_type(
            script, (ecs_expr_function_t*)node, cur, desc);
        break;
    case EcsExprTemplate:
        r = flecs_expr_template_visit_type(
            script, (ecs_expr_function_t*)node, cur, desc);
        break;
    case EcsExprMember:
        r = flecs_expr_member_visit_type(
            script, node_ptr, cur, desc, swizzle_expand_allowed);
        break;
    case EcsExprElement:
        r = flecs_expr_element_visit_type(
            script, (ecs_expr_element_t*)node, cur, desc);
        break;
    case EcsExprHas:
        r = flecs_expr_has_visit_type(script, (ecs_expr_has_t*)node, desc);
        break;
    case EcsExprMatch:
        r = flecs_expr_match_visit_type(
            script, (ecs_expr_match_t*)node, cur, desc);
        break;
    case EcsExprRange:
        r = flecs_expr_range_visit_type(
            script, (ecs_expr_range_t*)node, cur, desc);
        break;
    case EcsExprNew:
        r = flecs_expr_new_visit_type(script, (ecs_expr_new_t*)node, desc);
        break;
    case EcsExprScript:
        node->type = ecs_id(ecs_entity_t);
        break;
    case EcsExprCast:
    case EcsExprCastNumber:
    case EcsExprSwizzle:
    case EcsExprMethod:
    case EcsExprComponent:
        break;
    }

    if (r) {
        return -1;
    }

    node = *node_ptr;

    ecs_assert(node->type != 0, ECS_INTERNAL_ERROR, NULL);
    node->type_info = ecs_get_type_info(script->world, node->type);
    ecs_assert(node->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Every node produces a value of its own type. Rebase the cursor on that
     * type so that members and elements resolved on the value get an offset
     * that is relative to the value. */
    flecs_expr_visit_type_cursor(script->world, cur, node->type);

    return 0;
}

int flecs_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_node_t *node = *node_ptr;

    if (node->kind == EcsExprEmptyInitializer) {
        node->type = desc->type;
        
        if (node->type) {
            ecs_expr_initializer_t* init_node = (ecs_expr_initializer_t*)node;
            init_node->is_dynamic = flecs_expr_initializer_is_dynamic(
                script->world, node->type);

            if (flecs_expr_initializer_collection_check(
                script, init_node, NULL, node->type))
            {
                return -1;
            }

            node->type_info = ecs_get_type_info(script->world, node->type);
            return 0;
        }
    }

    ecs_meta_cursor_t cur = {0};
    if (desc->type) {
        cur = ecs_meta_cursor(script->world, desc->type, NULL);
    }

    return flecs_expr_visit_type_priv(script, node_ptr, &cur, desc);
}

#endif
