/**
 * @file addons/script/expr/util.c
 * @brief Script expression utilities.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"
#include <inttypes.h>

void flecs_expr_visit_error_(
    const ecs_script_t *script,
    const void *node,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = flecs_vasprintf(fmt, args);
    va_end(args);

    const ecs_expr_node_t *expr_node = node;
    const char *code = script->code;
    const char *pos = expr_node ? expr_node->pos : NULL;

    if (code && pos && (pos >= code) && (pos <= &code[ecs_os_strlen(code)])) {
        ecs_parser_error(script->name, code, pos - code, "%s", msg);
    } else {
        ecs_parser_error(script->name, NULL, 0, "%s", msg);
    }

    ecs_os_free(msg);
}

flecs_expr_num_class_t flecs_expr_num_class(
    ecs_entity_t type,
    ecs_size_t *size)
{
    const flecs_expr_type_info_t *info = flecs_expr_type_info(type);
    *size = info->size;
    if (info->floating_point) {
        return EcsExprNumFloat;
    }

    if (info->integer) {
        return info->signed_integer ? EcsExprNumSigned : EcsExprNumUnsigned;
    }

    return EcsExprNumNone;
}

#define FLECS_EXPR_CAST(S, D)\
static void flecs_expr_cast_##S##_##D(void *dst, const void *src) {\
    *(D*)dst = (D)*(const S*)src;\
}

#define FLECS_EXPR_CAST_TARGETS(M, S)\
    M(S, int8_t)\
    M(S, int16_t)\
    M(S, int32_t)\
    M(S, int64_t)\
    M(S, uint8_t)\
    M(S, uint16_t)\
    M(S, uint32_t)\
    M(S, uint64_t)\
    M(S, float)\
    M(S, double)

FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, int8_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, int16_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, int32_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, int64_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, uint8_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, uint16_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, uint32_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, uint64_t)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, float)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, double)
FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST, bool)

#undef FLECS_EXPR_CAST
#define FLECS_EXPR_CAST_ENTRY(S, D) flecs_expr_cast_##S##_##D,

flecs_expr_cast_t flecs_expr_cast_handler(
    ecs_entity_t from,
    ecs_entity_t to)
{
    static const flecs_expr_cast_t handlers[11][10] = {
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, int8_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, int16_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, int32_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, int64_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, uint8_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, uint16_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, uint32_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, uint64_t)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, float)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, double)},
        {FLECS_EXPR_CAST_TARGETS(FLECS_EXPR_CAST_ENTRY, bool)},
    };
    static const int8_t indexes[4][9] = {
        [EcsExprNumSigned] = { [1] = 0, [2] = 1, [4] = 2, [8] = 3 },
        [EcsExprNumUnsigned] = { [1] = 4, [2] = 5, [4] = 6, [8] = 7 },
        [EcsExprNumFloat] = { [4] = 8, [8] = 9 }
    };
    ecs_size_t from_size, to_size;
    flecs_expr_num_class_t from_class = flecs_expr_num_class(from, &from_size);
    flecs_expr_num_class_t to_class = flecs_expr_num_class(to, &to_size);
    bool from_bool = from == ecs_id(ecs_bool_t);
    ecs_assert((from_class || from_bool) && to_class, ECS_INTERNAL_ERROR, NULL);
    return handlers[from_bool ? 10 : indexes[from_class][from_size]]
        [indexes[to_class][to_size]];
}

#undef FLECS_EXPR_CAST_ENTRY
#undef FLECS_EXPR_CAST_TARGETS

int flecs_value_copy_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_expr_value_t *src)
{
    ecs_assert(dst->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->value.type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->value.ptr != 0, ECS_INTERNAL_ERROR, NULL);

    if (src->value.type == dst->type) {
        ecs_assert(src->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_ptr_copy_w_type_info(
            world, src->type_info, dst->ptr, src->value.ptr);
    } else if (src->value.type == ecs_id(ecs_script_template_ref_t) &&
        ecs_has(world, dst->type, EcsStruct))
    {
        const ecs_script_template_ref_t *ref = src->value.ptr;
        const ecs_type_info_t *ti = ecs_get_type_info(world, dst->type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        if (ref->value) {
            ecs_ptr_copy_w_type_info(world, ti, dst->ptr, ref->value);
        } else {
            ecs_entity_t type = ref->type ? ref->type : dst->type;
            void *defaults = ecs_ptr_new(world, type);
            ecs_ptr_copy_w_type_info(world, ti, dst->ptr, defaults);
            ecs_ptr_free(world, type, defaults);
        }
    } else if (src->value.type == ecs_id(ecs_bool_t) &&
        flecs_expr_is_type_number(dst->type))
    {
        flecs_expr_cast_handler(src->value.type, dst->type)(
            dst->ptr, src->value.ptr);
    } else {
        /* Cast value to desired output type */
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst->type, dst->ptr);
        if (ecs_meta_set_value(&cur, &src->value)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

int flecs_value_unary(
    const ecs_script_t *script,
    const ecs_value_t *expr,
    ecs_value_t *out,
    ecs_token_kind_t operator)
{
    if (operator == EcsTokNot) {
        ecs_assert(expr->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        ecs_assert(out->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        *(bool*)out->ptr = !*(bool*)expr->ptr;
    } else if (operator == EcsTokSub) {
        int64_t zero = 0;
        ecs_value_t left = { .type = expr->type, .ptr = &zero };
        return flecs_value_binary(script, NULL, &left, expr, out, EcsTokSub);
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for unary expression");
    }

    return 0;
}

#define FLECS_VALUE_COMPARE(T)\
    case EcsTokEq: *(bool*)out = l == r; return 0;\
    case EcsTokNeq: *(bool*)out = l != r; return 0;\
    case EcsTokGt: *(bool*)out = l > r; return 0;\
    case EcsTokGtEq: *(bool*)out = l >= r; return 0;\
    case EcsTokLt: *(bool*)out = l < r; return 0;\
    case EcsTokLtEq: *(bool*)out = l <= r; return 0;

#define FLECS_VALUE_INTEGER(T)\
    case EcsTokMod: *(T*)out = (T)(l % r); return 0;\
    case EcsTokBitwiseAnd: *(T*)out = (T)(l & r); return 0;\
    case EcsTokBitwiseOr: *(T*)out = (T)(l | r); return 0;\
    case EcsTokShiftLeft: *(T*)out = (T)(l << r); return 0;\
    case EcsTokShiftRight: *(T*)out = (T)(l >> r); return 0;

#define FLECS_VALUE_FLOAT(T)

#define FLECS_VALUE_ARITHMETIC(T)\
    case EcsTokAdd: case EcsTokAddAssign: *(T*)out = (T)(l + r); return 0;\
    case EcsTokSub: *(T*)out = (T)(l - r); return 0;\
    case EcsTokMul: case EcsTokMulAssign: *(T*)out = (T)(l * r); return 0;\
    case EcsTokDiv: *(T*)out = (T)(l / r); return 0;

#define FLECS_VALUE_BINARY(T, ARITHMETIC, INTEGER)\
static int flecs_value_binary_##T(\
    const void *left, const void *right, void *out, ecs_token_kind_t operator)\
{\
    T l = *(const T*)left, r = *(const T*)right;\
    switch (operator) {\
    FLECS_VALUE_COMPARE(T)\
    ARITHMETIC(T)\
    INTEGER(T)\
    default: ecs_abort(ECS_INTERNAL_ERROR, "invalid binary operator");\
    }\
    return -1;\
}

FLECS_VALUE_BINARY(ecs_i8_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i16_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u8_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u16_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_iptr_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_uptr_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
FLECS_VALUE_BINARY(ecs_f32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_f64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_FLOAT)
#if defined(ECS_TARGET_GNU) || defined(ECS_TARGET_CLANG)
#pragma GCC diagnostic pop
#endif
FLECS_VALUE_BINARY(ecs_char_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_bool_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_entity_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_id_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_byte_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)

#undef FLECS_VALUE_BINARY
#undef FLECS_VALUE_ARITHMETIC
#undef FLECS_VALUE_INTEGER
#undef FLECS_VALUE_FLOAT
#undef FLECS_VALUE_COMPARE

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_expr_node_t *node,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_token_kind_t operator)
{
    if (operator == EcsTokAddAssign || operator == EcsTokMulAssign) {
        left = out;
    }

    if ((operator == EcsTokDiv || operator == EcsTokMod) && flecs_value_is_0(right)) {
        flecs_expr_visit_error(script, node, "division by zero");
        return -1;
    }

    if (operator == EcsTokAnd || operator == EcsTokOr) {
        bool l = *(bool*)left->ptr, r = *(bool*)right->ptr;
        *(bool*)out->ptr = operator == EcsTokAnd ? l && r : l || r;
        return 0;
    }

    if (right->type == ecs_id(ecs_string_t)) {
        char *l = *(char**)left->ptr, *r = *(char**)right->ptr;
        ecs_assert(operator == EcsTokEq || operator == EcsTokNeq, ECS_INTERNAL_ERROR, NULL);
        *(bool*)out->ptr = l && r
            ? (operator == EcsTokEq ? !ecs_os_strcmp(l, r) : ecs_os_strcmp(l, r) != 0)
            : l == r;
        return 0;
    }

    if (operator == EcsTokAdd || operator == EcsTokSub || operator == EcsTokMul ||
        operator == EcsTokDiv || operator == EcsTokMod || operator == EcsTokBitwiseAnd ||
        operator == EcsTokBitwiseOr || operator == EcsTokShiftLeft || operator == EcsTokShiftRight ||
        operator == EcsTokAddAssign || operator == EcsTokMulAssign)
    {
        ecs_assert(out->type == right->type, ECS_INTERNAL_ERROR, NULL);
    }
#define FLECS_VALUE_DISPATCH(T)\
    if (right->type == ecs_id(T)) {\
        return flecs_value_binary_##T(left->ptr, right->ptr, out->ptr, operator);\
    }
    FLECS_VALUE_DISPATCH(ecs_i8_t)
    FLECS_VALUE_DISPATCH(ecs_i16_t)
    FLECS_VALUE_DISPATCH(ecs_i32_t)
    FLECS_VALUE_DISPATCH(ecs_i64_t)
    FLECS_VALUE_DISPATCH(ecs_u8_t)
    FLECS_VALUE_DISPATCH(ecs_u16_t)
    FLECS_VALUE_DISPATCH(ecs_u32_t)
    FLECS_VALUE_DISPATCH(ecs_u64_t)
    FLECS_VALUE_DISPATCH(ecs_iptr_t)
    FLECS_VALUE_DISPATCH(ecs_uptr_t)
    FLECS_VALUE_DISPATCH(ecs_f32_t)
    FLECS_VALUE_DISPATCH(ecs_f64_t)
    FLECS_VALUE_DISPATCH(ecs_char_t)
    FLECS_VALUE_DISPATCH(ecs_bool_t)
    FLECS_VALUE_DISPATCH(ecs_entity_t)
    FLECS_VALUE_DISPATCH(ecs_id_t)
    FLECS_VALUE_DISPATCH(ecs_byte_t)
#undef FLECS_VALUE_DISPATCH
    ecs_abort(ECS_INTERNAL_ERROR, "invalid binary operand type");
}

bool flecs_string_is_interpolated(
    const char *value)
{
    const char *ptr = value;

    for (ptr = strchr(ptr, '$'); ptr; ptr = strchr(ptr + 1, '$')) {
        if (ptr != value) {
            if (ptr[-1] == '\\') {
                continue; /* Escaped */
            }
        }

        if (isspace((unsigned char)ptr[1]) || !ptr[1]) {
            continue; /* $ by itself */
        }

        return true;
    }

    ptr = value;

    for (ptr = strchr(ptr, '{'); ptr; ptr = strchr(ptr + 1, '{')) {
        if (ptr != value) {
            if (ptr[-1] == '\\') {
                continue; /* Escaped */
            }
        }

        return true;
    }

    return false;
}

char* flecs_string_escape(
    char *str)
{
    const char *ptr;
    char *out = str, ch;

    for (ptr = str; ptr[0]; ) {
        if (ptr[0] == '\\') {
            if (ptr[1] == '{') { /* Escape string interpolation delimiter */
                ch = '{';
                ptr += 2;
            } else if (ptr[1] == '$') { /* Escape string interpolation var */
                ch = '$';
                ptr += 2;
            } else {
                ptr = flecs_chrparse(ptr, &ch);
                if (!ptr) {
                    ecs_err("invalid escape sequence in string '%s'", str);
                    return NULL;
                }
            }
        } else {
            ch = ptr[0];
            ptr ++;
        }

        out[0] = ch;
        out ++;
    }

    out[0] = '\0';

    return out + 1;
}

bool flecs_value_is_0(
    const ecs_value_t *value)
{
    const flecs_expr_type_info_t *info = flecs_expr_type_info(value->type);
    if (!(info->integer || info->floating_point || info->kind == EcsBool)) {
        return true;
    }

    uint64_t zero = 0;
    return !ecs_os_memcmp(value->ptr, &zero, info->size);
}

ecs_expr_swizzle_t* flecs_expr_expand_swizzle_get(
    ecs_expr_node_t *node)
{
    if (node->kind == EcsExprIdentifier) {
        node = ((ecs_expr_identifier_t*)node)->expr;
        if (!node) {
            return NULL;
        }
    }

    if (node->kind != EcsExprSwizzle) {
        return NULL;
    }

    ecs_expr_swizzle_t *swizzle = (ecs_expr_swizzle_t*)node;
    if (!swizzle->expand) {
        return NULL;
    }

    return swizzle;
}

int flecs_expr_identifier_set(
    const ecs_script_t *script,
    const ecs_expr_identifier_t *node,
    ecs_entity_t entity,
    const ecs_value_t *out)
{
    if (!entity) {
        flecs_expr_visit_error(script, node,
            "unresolved identifier '%s'", node->value);
        return -1;
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        script->world, out->type, out->ptr);
    if (ecs_meta_set_entity(&cur, entity)) {
        flecs_expr_visit_error(script, node,
            "failed to assign identifier '%s'", node->value);
        return -1;
    }

    return 0;
}

const ecs_script_var_t* flecs_expr_var_check(
    const ecs_script_t *script,
    const ecs_expr_variable_t *node,
    const ecs_script_var_t *var)
{
    if (!var) {
        flecs_expr_visit_error(script, node,
            "unresolved variable '%s'", node->name);
        return NULL;
    }

    if (!var->value.ptr) {
        flecs_expr_visit_error(script, node,
            "variable '%s' is not a compile time constant", node->name);
        return NULL;
    }

    ecs_assert(var->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    return var;
}

int flecs_expr_element_get(
    const ecs_script_t *script,
    const ecs_expr_element_t *node,
    ecs_type_kind_t kind,
    const ecs_value_t *expr,
    const ecs_value_t *index,
    void **out)
{
    if (kind == EcsMapType) {
        ecs_map_key_t key = 0;
        if (flecs_value_blit_u64(script->world, index, &key)) {
            return -1;
        }

        const ecs_map_t *map = expr->ptr;
        ecs_map_val_t *val = NULL;
        if (ecs_map_is_init(map)) {
            val = ecs_map_get(map, key);
        }

        if (!val) {
            char *key_str = ecs_ptr_to_str(
                script->world, index->type, index->ptr);
            flecs_expr_visit_error(script, node,
                "map does not contain key '%s'", key_str);
            ecs_os_free(key_str);
            return -1;
        }

        *out = node->elem_size > ECS_SIZEOF(ecs_map_val_t)
            ? (void*)(uintptr_t)val[0]
            : (void*)val;
        return 0;
    }

    int64_t index_value = *(int64_t*)index->ptr;
    void *data = expr->ptr;
    int64_t elem_count = node->elem_count;
    if (kind == EcsVectorType) {
        data = ecs_vec_first(expr->ptr);
        if (!elem_count) {
            elem_count = ecs_vec_count(expr->ptr);
        }
    }

    if (index_value < 0 || index_value >= elem_count) {
        flecs_expr_visit_error(script, node,
            "index %" PRId64 " is out of range for collection (count = %"
                PRId64 ")", index_value, elem_count);
        return -1;
    }

    *out = ECS_OFFSET(data, node->elem_size * index_value);
    return 0;
}

const void* flecs_expr_component_get(
    const ecs_script_t *script,
    struct ecs_script_eval_visitor_t *v,
    const ecs_expr_element_t *node,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_world_t *world = script->world;
    if (!entity || !ecs_is_alive(world, entity)) {
        flecs_expr_visit_error(script, node,
            "cannot read component '%s' from invalid entity",
            flecs_errstr(ecs_get_path(world, component)));
        return NULL;
    }

    if (node->dyn_input && v && v->run_refs) {
        flecs_script_record_dyn_ref(v->run_refs, entity, component,
            node->dyn_input, false);
    }

    const void *ptr = ecs_get_id(world, entity, component);
    if (!ptr) {
        flecs_expr_visit_error(script, node,
            "entity '%s' does not have component '%s'",
            flecs_errstr(ecs_get_path(world, entity)),
            flecs_errstr_1(ecs_get_path(world, component)));
    }

    return ptr;
}

bool flecs_expr_has_eval(
    const ecs_script_t *script,
    struct ecs_script_eval_visitor_t *v,
    const ecs_expr_has_t *node,
    ecs_entity_t entity)
{
    ecs_world_t *world = script->world;
    if (!entity || !ecs_is_alive(world, entity)) {
        return false;
    }

    if (node->dyn_input && v && v->run_refs) {
        flecs_script_record_dyn_ref(v->run_refs, entity, node->id,
            node->dyn_input, true);
    }

    return ecs_has_id(world, entity, node->id);
}

void* flecs_expr_template_ref_value(
    const ecs_script_t *script,
    const ecs_expr_function_t *node,
    const ecs_script_template_ref_t *ref)
{
    if (!ref->type) {
        flecs_expr_visit_error(script, node,
            "template prop '%s' has no value", node->function_name);
        return NULL;
    }

    void *value = ecs_ptr_new(script->world, ref->type);
    if (ref->value) {
        ecs_ptr_copy(script->world, ref->type, value, ref->value);
    }

    return value;
}

int flecs_expr_range_eval(
    ecs_world_t *world,
    ecs_entity_t type,
    int32_t from,
    int32_t to,
    void *out)
{
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, out);
    if (ecs_meta_push(&cur)) {
        return -1;
    }

    int32_t i, count = to - from;
    for (i = 0; i < count; i ++) {
        if (i) {
            if (ecs_meta_next(&cur)) {
                return -1;
            }
        }

        if (ecs_meta_set_int(&cur, from + i)) {
            return -1;
        }
    }

    return ecs_meta_pop(&cur);
}

ecs_entity_t flecs_expr_script_new(
    ecs_world_t *world,
    ecs_script_t *script)
{
    ecs_entity_t e = ecs_new(world);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    s->code = ecs_os_strdup(script->code);
    s->script = script;
    flecs_script_impl(script)->refcount ++;
    ecs_modified(world, e, EcsScript);
    return e;
}

int flecs_expr_swizzle_set(
    ecs_meta_cursor_t *cur,
    const ecs_expr_swizzle_t *swizzle,
    const void *src)
{
    int32_t s;
    for (s = 0; s < swizzle->count; s ++) {
        if (s) {
            if (ecs_meta_next(cur)) {
                return -1;
            }
        }

        ecs_value_t value = {
            .ptr = ECS_OFFSET(src, swizzle->src[s]),
            .type = swizzle->node.type
        };
        if (ecs_meta_set_value(cur, &value)) {
            return -1;
        }
    }

    return 0;
}

#endif
