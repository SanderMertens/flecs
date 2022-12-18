/**
 * @file meta/api.c
 * @brief API for assigning values of runtime types with reflection.
 */

#include "meta.h"
#include <ctype.h>

#ifdef FLECS_META

static
const char* flecs_meta_op_kind_str(
    ecs_meta_type_op_kind_t kind)
{
    switch(kind) {

    case EcsOpEnum: return "Enum";
    case EcsOpBitmask: return "Bitmask";
    case EcsOpArray: return "Array";
    case EcsOpVector: return "Vector";
    case EcsOpPush: return "Push";
    case EcsOpPop: return "Pop";
    case EcsOpPrimitive: return "Primitive";
    case EcsOpBool: return "Bool";
    case EcsOpChar: return "Char";
    case EcsOpByte: return "Byte";
    case EcsOpU8: return "U8";
    case EcsOpU16: return "U16";
    case EcsOpU32: return "U32";
    case EcsOpU64: return "U64";
    case EcsOpI8: return "I8";
    case EcsOpI16: return "I16";
    case EcsOpI32: return "I32";
    case EcsOpI64: return "I64";
    case EcsOpF32: return "F32";
    case EcsOpF64: return "F64";
    case EcsOpUPtr: return "UPtr";
    case EcsOpIPtr: return "IPtr";
    case EcsOpString: return "String";
    case EcsOpEntity: return "Entity";
    default: return "<< invalid kind >>";
    }
}

/* Get current scope */
static
ecs_meta_scope_t* flecs_meta_cursor_get_scope(
    const ecs_meta_cursor_t *cursor)
{
    ecs_check(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    return (ecs_meta_scope_t*)&cursor->scope[cursor->depth];
error:
    return NULL;
}

/* Restore scope, if dotmember was used */
static
void flecs_meta_cursor_restore_scope(
    ecs_meta_cursor_t *cursor,
    const ecs_meta_scope_t* scope)
{
    ecs_check(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(scope != NULL, ECS_INVALID_PARAMETER, NULL);
    if (scope->prev_depth) {
        cursor->depth = scope->prev_depth;
    }
error:
    return;
}

/* Get previous scope */
static
ecs_meta_scope_t* get_prev_scope(
    ecs_meta_cursor_t *cursor)
{
    ecs_check(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(cursor->depth > 0, ECS_INVALID_PARAMETER, NULL);
    return &cursor->scope[cursor->depth - 1];
error:
    return NULL;
}

/* Get current operation for scope */
static
ecs_meta_type_op_t* flecs_meta_cursor_get_op(
    ecs_meta_scope_t *scope)
{
    ecs_assert(scope->ops != NULL, ECS_INVALID_OPERATION, NULL);
    return &scope->ops[scope->op_cur];
}

/* Get component for type in current scope */
static
const EcsComponent* get_ecs_component(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    const EcsComponent *comp = scope->comp;
    if (!comp) {
        comp = scope->comp = ecs_get(world, scope->type, EcsComponent);
        ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    return comp;
}

/* Get size for type in current scope */
static
ecs_size_t get_size(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    return get_ecs_component(world, scope)->size;
}

/* Get alignment for type in current scope */
static
ecs_size_t get_alignment(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    return get_ecs_component(world, scope)->alignment;
}

static
int32_t get_elem_count(
    ecs_meta_scope_t *scope)
{
    if (scope->vector) {
        return ecs_vector_count(*(scope->vector));
    }

    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    return op->count;
}

/* Get pointer to current field/element */
static
ecs_meta_type_op_t* flecs_meta_cursor_get_ptr(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    ecs_size_t size = get_size(world, scope);

    if (scope->vector) {
        ecs_size_t align = get_alignment(world, scope);
        ecs_vector_set_min_count_t(
            scope->vector, size, align, scope->elem_cur + 1);
        scope->ptr = ecs_vector_first_t(*(scope->vector), size, align);
    }

    return ECS_OFFSET(scope->ptr, size * scope->elem_cur + op->offset);
}

static
int flecs_meta_cursor_push_type(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope,
    ecs_entity_t type,
    void *ptr)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    if (ser == NULL) {
        char *str = ecs_id_str(world, type);
        ecs_err("cannot open scope for entity '%s' which is not a type", str);
        ecs_os_free(str);
        return -1;
    }

    scope[0] = (ecs_meta_scope_t) {
        .type = type,
        .ops = ecs_vector_first(ser->ops, ecs_meta_type_op_t),
        .op_count = ecs_vector_count(ser->ops),
        .ptr = ptr
    };

    return 0;
}

ecs_meta_cursor_t ecs_meta_cursor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(type != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_meta_cursor_t result = {
        .world = world,
        .valid = true
    };

    if (flecs_meta_cursor_push_type(world, result.scope, type, ptr) != 0) {
        result.valid = false;
    }

    return result;
error:
    return (ecs_meta_cursor_t){ 0 };
}

void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor)
{
    return flecs_meta_cursor_get_ptr(cursor->world, 
        flecs_meta_cursor_get_scope(cursor));
}

int ecs_meta_next(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);

    if (scope->is_collection) {
        scope->elem_cur ++;
        scope->op_cur = 0;
        if (scope->elem_cur >= get_elem_count(scope)) {
            ecs_err("out of collection bounds (%d)", scope->elem_cur);
            return -1;
        }

        return 0;
    }

    scope->op_cur += op->op_count;
    if (scope->op_cur >= scope->op_count) {
        ecs_err("out of bounds");
        return -1;
    }

    return 0;
}

int ecs_meta_elem(
    ecs_meta_cursor_t *cursor,
    int32_t elem)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    if (!scope->is_collection) {
        ecs_err("ecs_meta_elem can be used for collections only");
        return -1;
    }

    scope->elem_cur = elem;
    scope->op_cur = 0;

    if (scope->elem_cur >= get_elem_count(scope) || (scope->elem_cur < 0)) {
        ecs_err("out of collection bounds (%d)", scope->elem_cur);
        return -1;
    }
    
    return 0;
}

int ecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
    if (cursor->depth == 0) {
        ecs_err("cannot move to member in root scope");
        return -1;
    }

    ecs_meta_scope_t *prev_scope = get_prev_scope(cursor);
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *push_op = flecs_meta_cursor_get_op(prev_scope);
    const ecs_world_t *world = cursor->world;

    ecs_assert(push_op->kind == EcsOpPush, ECS_INTERNAL_ERROR, NULL);

    if (!push_op->members) {
        ecs_err("cannot move to member '%s' for non-struct type", name);
        return -1;
    }

    const uint64_t *cur_ptr = flecs_name_index_find_ptr(push_op->members, name, 0, 0);
    if (!cur_ptr) {
        char *path = ecs_get_fullpath(world, scope->type);
        ecs_err("unknown member '%s' for type '%s'", name, path);
        ecs_os_free(path);
        return -1;
    }

    scope->op_cur = flecs_uto(int32_t, cur_ptr[0]);

    return 0;
}

int ecs_meta_dotmember(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
#ifdef FLECS_PARSER
    int32_t prev_depth = cursor->depth;
    int dotcount = 0;

    char token[ECS_MAX_TOKEN_SIZE];
    const char *ptr = name;
    while ((ptr = ecs_parse_token(NULL, NULL, ptr, token, '.'))) {
        if (ptr[0] != '.' && ptr[0]) {
            ecs_parser_error(NULL, name, ptr - name, 
                "expected '.' or end of string");
            goto error;
        }

        if (dotcount) {
            ecs_meta_push(cursor);
        }

        if (ecs_meta_member(cursor, token)) {
            goto error;
        }

        if (!ptr[0]) {
            break;   
        }

        ptr ++; /* Skip . */

        dotcount ++;
    }

    ecs_meta_scope_t *cur_scope = flecs_meta_cursor_get_scope(cursor);
    if (dotcount) {
        cur_scope->prev_depth = prev_depth;
    }

    return 0;
error:
    return -1;
#else
    (void)cursor;
    (void)name;
    ecs_err("the FLECS_PARSER addon is required for ecs_meta_dotmember");
    return -1;
#endif
}

int ecs_meta_push(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    const ecs_world_t *world = cursor->world;

    if (cursor->depth == 0) {
        if (!cursor->is_primitive_scope) {
            if (op->kind > EcsOpScope) {
                cursor->is_primitive_scope = true;
                return 0;
            }
        }
    }

    void *ptr = flecs_meta_cursor_get_ptr(world, scope);
    cursor->depth ++;
    ecs_check(cursor->depth < ECS_META_MAX_SCOPE_DEPTH,
        ECS_INVALID_PARAMETER, NULL);

    ecs_meta_scope_t *next_scope = flecs_meta_cursor_get_scope(cursor);

    /* If we're not already in an inline array and this operation is an inline
     * array, push a frame for the array.
     * Doing this first ensures that inline arrays take precedence over other
     * kinds of push operations, such as for a struct element type. */
    if (!scope->is_inline_array && op->count > 1 && !scope->is_collection) {
        /* Push a frame just for the element type, with inline_array = true */
        next_scope[0] = (ecs_meta_scope_t){
            .ops = op,
            .op_count = op->op_count,
            .ptr = scope->ptr,
            .type = op->type,
            .is_collection = true,
            .is_inline_array = true
        };

        /* With 'is_inline_array' set to true we ensure that we can never push
         * the same inline array twice */

        return 0;
    }

    switch(op->kind) {
    case EcsOpPush:
        next_scope[0] = (ecs_meta_scope_t) {
            .ops = &op[1],                /* op after push */
            .op_count = op->op_count - 1, /* don't include pop */
            .ptr = scope->ptr,
            .type = op->type
        };
        break;

    case EcsOpArray: {
        if (flecs_meta_cursor_push_type(world, next_scope, op->type, ptr) != 0) {
            goto error;
        }

        const EcsArray *type_ptr = ecs_get(world, op->type, EcsArray);
        next_scope->type = type_ptr->type;
        next_scope->is_collection = true;
        break;
    }

    case EcsOpVector:
        next_scope->vector = ptr;
        if (flecs_meta_cursor_push_type(world, next_scope, op->type, NULL) != 0) {
            goto error;
        }

        const EcsVector *type_ptr = ecs_get(world, op->type, EcsVector);
        next_scope->type = type_ptr->type;
        next_scope->is_collection = true;
        break;

    default: {
        char *path = ecs_get_fullpath(world, scope->type);
        ecs_err("invalid push for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }
    }

    if (scope->is_collection) {
        next_scope[0].ptr = ECS_OFFSET(next_scope[0].ptr,
            scope->elem_cur * get_size(world, scope));
    }

    return 0;
error:
    return -1;
}

int ecs_meta_pop(
    ecs_meta_cursor_t *cursor)
{
    if (cursor->is_primitive_scope) {
        cursor->is_primitive_scope = false;
        return 0;
    }

    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    cursor->depth --;
    if (cursor->depth < 0) {
        ecs_err("unexpected end of scope");
        return -1;
    }

    ecs_meta_scope_t *next_scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(next_scope);

    if (!scope->is_inline_array) {
        if (op->kind == EcsOpPush) {
            next_scope->op_cur += op->op_count - 1;

            /* push + op_count should point to the operation after pop */
            op = flecs_meta_cursor_get_op(next_scope);
            ecs_assert(op->kind == EcsOpPop, ECS_INTERNAL_ERROR, NULL);
        } else if (op->kind == EcsOpArray || op->kind == EcsOpVector) {
            /* Collection type, nothing else to do */
        } else {
            /* should not have been able to push if the previous scope was not
             * a complex or collection type */
            ecs_assert(false, ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        /* Make sure that this was an inline array */
        ecs_assert(next_scope->op_count > 1, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_meta_cursor_restore_scope(cursor, next_scope);
    return 0;
}

bool ecs_meta_is_collection(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    return scope->is_collection;
}

ecs_entity_t ecs_meta_get_type(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    return op->type;
}

ecs_entity_t ecs_meta_get_unit(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    return op->unit;
}

const char* ecs_meta_get_member(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    return op->name;
}

/* Utilities for type conversions and bounds checking */
struct {
    int64_t min, max;
} ecs_meta_bounds_signed[EcsMetaTypeOpKindLast + 1] = {
    [EcsOpBool]    = {false,      true},
    [EcsOpChar]    = {INT8_MIN,   INT8_MAX},
    [EcsOpByte]    = {0,          UINT8_MAX},
    [EcsOpU8]      = {0,          UINT8_MAX},
    [EcsOpU16]     = {0,          UINT16_MAX},
    [EcsOpU32]     = {0,          UINT32_MAX},
    [EcsOpU64]     = {0,          INT64_MAX},
    [EcsOpI8]      = {INT8_MIN,   INT8_MAX},
    [EcsOpI16]     = {INT16_MIN,  INT16_MAX},
    [EcsOpI32]     = {INT32_MIN,  INT32_MAX},
    [EcsOpI64]     = {INT64_MIN,  INT64_MAX},
    [EcsOpUPtr]    = {0, ((sizeof(void*) == 4) ? UINT32_MAX : INT64_MAX)},
    [EcsOpIPtr]    = {
        ((sizeof(void*) == 4) ? INT32_MIN : INT64_MIN), 
        ((sizeof(void*) == 4) ? INT32_MAX : INT64_MAX)
    },
    [EcsOpEntity]  = {0,          INT64_MAX},
    [EcsOpEnum]    = {INT32_MIN,  INT32_MAX},
    [EcsOpBitmask] = {0,          INT32_MAX}
};

struct {
    uint64_t min, max;
} ecs_meta_bounds_unsigned[EcsMetaTypeOpKindLast + 1] = {
    [EcsOpBool]    = {false,      true},
    [EcsOpChar]    = {0,          INT8_MAX},
    [EcsOpByte]    = {0,          UINT8_MAX},
    [EcsOpU8]      = {0,          UINT8_MAX},
    [EcsOpU16]     = {0,          UINT16_MAX},
    [EcsOpU32]     = {0,          UINT32_MAX},
    [EcsOpU64]     = {0,          UINT64_MAX},
    [EcsOpI8]      = {0,          INT8_MAX},
    [EcsOpI16]     = {0,          INT16_MAX},
    [EcsOpI32]     = {0,          INT32_MAX},
    [EcsOpI64]     = {0,          INT64_MAX},
    [EcsOpUPtr]    = {0, ((sizeof(void*) == 4) ? UINT32_MAX : UINT64_MAX)},
    [EcsOpIPtr]    = {0, ((sizeof(void*) == 4) ? INT32_MAX : INT64_MAX)},
    [EcsOpEntity]  = {0,          UINT64_MAX},
    [EcsOpEnum]    = {0,          INT32_MAX},
    [EcsOpBitmask] = {0,          UINT32_MAX}
};

struct {
    double min, max;
} ecs_meta_bounds_float[EcsMetaTypeOpKindLast + 1] = {
    [EcsOpBool]    = {false,      true},
    [EcsOpChar]    = {INT8_MIN,   INT8_MAX},
    [EcsOpByte]    = {0,          UINT8_MAX},
    [EcsOpU8]      = {0,          UINT8_MAX},
    [EcsOpU16]     = {0,          UINT16_MAX},
    [EcsOpU32]     = {0,          UINT32_MAX},
    [EcsOpU64]     = {0,          (double)UINT64_MAX},
    [EcsOpI8]      = {INT8_MIN,   INT8_MAX},
    [EcsOpI16]     = {INT16_MIN,  INT16_MAX},
    [EcsOpI32]     = {INT32_MIN,  INT32_MAX},
    [EcsOpI64]     = {INT64_MIN,  (double)INT64_MAX},
    [EcsOpUPtr]    = {0, ((sizeof(void*) == 4) ? UINT32_MAX : (double)UINT64_MAX)},
    [EcsOpIPtr]    = {
        ((sizeof(void*) == 4) ? INT32_MIN : (double)INT64_MIN), 
        ((sizeof(void*) == 4) ? INT32_MAX : (double)INT64_MAX)
    },
    [EcsOpEntity]  = {0,          (double)UINT64_MAX},
    [EcsOpEnum]    = {INT32_MIN,  INT32_MAX},
    [EcsOpBitmask] = {0,          UINT32_MAX}
};

#define set_T(T, ptr, value)\
    ((T*)ptr)[0] = ((T)value)

#define case_T(kind, T, dst, src)\
case kind:\
    set_T(T, dst, src);\
    break

#define case_T_checked(kind, T, dst, src, bounds)\
case kind:\
    if ((src < bounds[kind].min) || (src > bounds[kind].max)){\
        ecs_err("value %.0f is out of bounds for type %s", (double)src,\
            flecs_meta_op_kind_str(kind));\
        return -1;\
    }\
    set_T(T, dst, src);\
    break

#define cases_T_float(dst, src)\
    case_T(EcsOpF32,  ecs_f32_t,  dst, src);\
    case_T(EcsOpF64,  ecs_f64_t,  dst, src)

#define cases_T_signed(dst, src, bounds)\
    case_T_checked(EcsOpChar, ecs_char_t, dst, src, bounds);\
    case_T_checked(EcsOpI8,   ecs_i8_t,   dst, src, bounds);\
    case_T_checked(EcsOpI16,  ecs_i16_t,  dst, src, bounds);\
    case_T_checked(EcsOpI32,  ecs_i32_t,  dst, src, bounds);\
    case_T_checked(EcsOpI64,  ecs_i64_t,  dst, src, bounds);\
    case_T_checked(EcsOpIPtr, ecs_iptr_t, dst, src, bounds);\
    case_T_checked(EcsOpEnum, ecs_i32_t, dst, src, bounds)

#define cases_T_unsigned(dst, src, bounds)\
    case_T_checked(EcsOpByte, ecs_byte_t, dst, src, bounds);\
    case_T_checked(EcsOpU8,   ecs_u8_t,   dst, src, bounds);\
    case_T_checked(EcsOpU16,  ecs_u16_t,  dst, src, bounds);\
    case_T_checked(EcsOpU32,  ecs_u32_t,  dst, src, bounds);\
    case_T_checked(EcsOpU64,  ecs_u64_t,  dst, src, bounds);\
    case_T_checked(EcsOpUPtr, ecs_uptr_t, dst, src, bounds);\
    case_T_checked(EcsOpEntity, ecs_u64_t, dst, src, bounds);\
    case_T_checked(EcsOpBitmask, ecs_u32_t, dst, src, bounds)

#define cases_T_bool(dst, src)\
case EcsOpBool:\
    set_T(ecs_bool_t, dst, value != 0);\
    break

static
void conversion_error(
    ecs_meta_cursor_t *cursor,
    ecs_meta_type_op_t *op,
    const char *from)
{
    char *path = ecs_get_fullpath(cursor->world, op->type);
    ecs_err("unsupported conversion from %s to '%s'", from, path);
    ecs_os_free(path);
}

int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_unsigned);
    default:
        conversion_error(cursor, op, "bool");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_signed);
    default:
        conversion_error(cursor, op, "char");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_signed);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_signed);
    cases_T_float(ptr, value);
    default: {
        if(!value) return ecs_meta_set_null(cursor);
        conversion_error(cursor, op, "int");
        return -1;
    }
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_unsigned);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_unsigned);
    cases_T_float(ptr, value);
    default:
        if(!value) return ecs_meta_set_null(cursor);
        conversion_error(cursor, op, "uint");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_float);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_float);
    cases_T_float(ptr, value);
    default:
        conversion_error(cursor, op, "float");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_value(
    ecs_meta_cursor_t *cursor,
    const ecs_value_t *value)
{
    ecs_check(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t type = value->type;
    ecs_check(type != 0, ECS_INVALID_PARAMETER, NULL);
    const EcsMetaType *mt = ecs_get(cursor->world, type, EcsMetaType);
    if (!mt) {
        ecs_err("type of value does not have reflection data");
        return -1;
    }

    if (mt->kind == EcsPrimitiveType) {
        const EcsPrimitive *prim = ecs_get(cursor->world, type, EcsPrimitive);
        ecs_check(prim != NULL, ECS_INTERNAL_ERROR, NULL);
        switch(prim->kind) {
        case EcsBool: return ecs_meta_set_bool(cursor, *(bool*)value->ptr);
        case EcsChar: return ecs_meta_set_char(cursor, *(char*)value->ptr);
        case EcsByte: return ecs_meta_set_uint(cursor, *(uint8_t*)value->ptr);
        case EcsU8:   return ecs_meta_set_uint(cursor, *(uint8_t*)value->ptr);
        case EcsU16:  return ecs_meta_set_uint(cursor, *(uint16_t*)value->ptr);
        case EcsU32:  return ecs_meta_set_uint(cursor, *(uint32_t*)value->ptr);
        case EcsU64:  return ecs_meta_set_uint(cursor, *(uint64_t*)value->ptr);
        case EcsI8:   return ecs_meta_set_int(cursor, *(int8_t*)value->ptr);
        case EcsI16:  return ecs_meta_set_int(cursor, *(int16_t*)value->ptr);
        case EcsI32:  return ecs_meta_set_int(cursor, *(int32_t*)value->ptr);
        case EcsI64:  return ecs_meta_set_int(cursor, *(int64_t*)value->ptr);
        case EcsF32:  return ecs_meta_set_float(cursor, (double)*(float*)value->ptr);
        case EcsF64:  return ecs_meta_set_float(cursor, *(double*)value->ptr);
        case EcsUPtr: return ecs_meta_set_uint(cursor, *(uintptr_t*)value->ptr);
        case EcsIPtr: return ecs_meta_set_int(cursor, *(intptr_t*)value->ptr);
        case EcsString: return ecs_meta_set_string(cursor, *(char**)value->ptr);
        case EcsEntity: return ecs_meta_set_entity(cursor, 
            *(ecs_entity_t*)value->ptr);
        default:
            ecs_throw(ECS_INTERNAL_ERROR, "invalid type kind");
            goto error;
        }
    } else if (mt->kind == EcsEnumType) {
        return ecs_meta_set_int(cursor, *(int32_t*)value->ptr);
    } else if (mt->kind == EcsBitmaskType) {
        return ecs_meta_set_int(cursor, *(uint32_t*)value->ptr);
    } else {
        ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
        ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
        void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
        if (op->type != value->type) {
            char *type_str = ecs_get_fullpath(cursor->world, value->type);
            conversion_error(cursor, op, type_str);
            ecs_os_free(type_str);
            goto error;
        }
        flecs_meta_cursor_restore_scope(cursor, scope);
        return ecs_value_copy(cursor->world, value->type, ptr, value->ptr);
    }

error:
    return -1;
}

static
int add_bitmask_constant(
    ecs_meta_cursor_t *cursor,
    ecs_meta_type_op_t *op,
    void *out,
    const char *value)
{
    ecs_assert(op->type != 0, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_os_strcmp(value, "0")) {
        return 0;
    }

    ecs_entity_t c = ecs_lookup_child(cursor->world, op->type, value);
    if (!c) {
        char *path = ecs_get_fullpath(cursor->world, op->type);
        ecs_err("unresolved bitmask constant '%s' for type '%s'", value, path);
        ecs_os_free(path);
        return -1;
    }

    const ecs_u32_t *v = ecs_get_pair_object(
        cursor->world, c, EcsConstant, ecs_u32_t);
    if (v == NULL) {
        char *path = ecs_get_fullpath(cursor->world, op->type);
        ecs_err("'%s' is not an bitmask constant for type '%s'", value, path);
        ecs_os_free(path);
        return -1;
    }

    *(ecs_u32_t*)out |= v[0];

    return 0;
}

static
int parse_bitmask(
    ecs_meta_cursor_t *cursor,
    ecs_meta_type_op_t *op,
    void *out,
    const char *value)
{
    char token[ECS_MAX_TOKEN_SIZE];

    const char *prev = value, *ptr = value;

    *(ecs_u32_t*)out = 0;

    while ((ptr = strchr(ptr, '|'))) {
        ecs_os_memcpy(token, prev, ptr - prev);
        token[ptr - prev] = '\0';
        if (add_bitmask_constant(cursor, op, out, token) != 0) {
            return -1;
        }

        ptr ++;
        prev = ptr;
    }

    if (add_bitmask_constant(cursor, op, out, prev) != 0) {
        return -1;
    }

    return 0;
}

int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    case EcsOpBool:
        if (!ecs_os_strcmp(value, "true")) {
            set_T(ecs_bool_t, ptr, true);
        } else if (!ecs_os_strcmp(value, "false")) {
            set_T(ecs_bool_t, ptr, false);
        } else if (isdigit(value[0])) {
            if (!ecs_os_strcmp(value, "0")) {
                set_T(ecs_bool_t, ptr, false);
            } else {
                set_T(ecs_bool_t, ptr, true);
            }
        } else {
            ecs_err("invalid value for boolean '%s'", value);
            return -1;
        }
        break;
    case EcsOpI8:
    case EcsOpU8:
    case EcsOpChar:
    case EcsOpByte:
        set_T(ecs_i8_t, ptr, atol(value));
        break;
    case EcsOpI16:
    case EcsOpU16:
        set_T(ecs_i16_t, ptr, atol(value));
        break;
    case EcsOpI32:
    case EcsOpU32:
        set_T(ecs_i32_t, ptr, atol(value));
        break;
    case EcsOpI64:
    case EcsOpU64:
        set_T(ecs_i64_t, ptr, atol(value));
        break;
    case EcsOpIPtr:
    case EcsOpUPtr:
        set_T(ecs_iptr_t, ptr, atol(value));
        break;
    case EcsOpF32:
        set_T(ecs_f32_t, ptr, atof(value));
        break;
    case EcsOpF64:
        set_T(ecs_f64_t, ptr, atof(value));
        break;
    case EcsOpString: {
        ecs_assert(*(ecs_string_t*)ptr != value, ECS_INVALID_PARAMETER, NULL);
        ecs_os_free(*(ecs_string_t*)ptr);
        char *result = ecs_os_strdup(value);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpEnum: {
        ecs_assert(op->type != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t c = ecs_lookup_child(cursor->world, op->type, value);
        if (!c) {
            char *path = ecs_get_fullpath(cursor->world, op->type);
            ecs_err("unresolved enum constant '%s' for type '%s'", value, path);
            ecs_os_free(path);
            return -1;
        }

        const ecs_i32_t *v = ecs_get_pair_object(
            cursor->world, c, EcsConstant, ecs_i32_t);
        if (v == NULL) {
            char *path = ecs_get_fullpath(cursor->world, op->type);
            ecs_err("'%s' is not an enum constant for type '%s'", value, path);
            ecs_os_free(path);
            return -1;
        }

        set_T(ecs_i32_t, ptr, v[0]);
        break;
    }
    case EcsOpBitmask:
        if (parse_bitmask(cursor, op, ptr, value) != 0) {
            return -1;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = 0;

        if (ecs_os_strcmp(value, "0")) {
            if (cursor->lookup_action) {
                e = cursor->lookup_action(
                    cursor->world, value,
                    cursor->lookup_ctx);
            } else {
                e = ecs_lookup_path(cursor->world, 0, value);
            }

            if (!e) {
                ecs_err("unresolved entity identifier '%s'", value);
                return -1;
            }
        }

        set_T(ecs_entity_t, ptr, e);
        break;
    }
    case EcsOpPop:
        ecs_err("excess element '%s' in scope", value);
        return -1;
    default:
        ecs_err("unsupported conversion from string '%s' to '%s'",
            value, flecs_meta_op_kind_str(op->kind));
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_string_literal(
    ecs_meta_cursor_t *cursor,
    const char *value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    ecs_size_t len = ecs_os_strlen(value);
    if (value[0] != '\"' || value[len - 1] != '\"') {
        ecs_err("invalid string literal '%s'", value);
        return -1;
    }

    switch(op->kind) {
    case EcsOpChar:
        set_T(ecs_char_t, ptr, value[1]);
        break;

    default:
    case EcsOpEntity:
    case EcsOpString:
        len -= 2;

        char *result = ecs_os_malloc(len + 1);
        ecs_os_memcpy(result, value + 1, len);
        result[len] = '\0';

        if (ecs_meta_set_string(cursor, result)) {
            ecs_os_free(result);
            return -1;
        }

        ecs_os_free(result);

        break;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);

    switch(op->kind) {
    case EcsOpEntity:
        set_T(ecs_entity_t, ptr, value);
        break;
    default:
        conversion_error(cursor, op, "entity");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

int ecs_meta_set_null(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch (op->kind) {
    case EcsOpString:
        ecs_os_free(*(char**)ptr);
        set_T(ecs_string_t, ptr, NULL);
        break;
    default:
        conversion_error(cursor, op, "null");
        return -1;
    }

    flecs_meta_cursor_restore_scope(cursor, scope);
    return 0;
}

bool ecs_meta_get_bool(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpBool: return *(ecs_bool_t*)ptr;
    case EcsOpI8:   return *(ecs_i8_t*)ptr != 0;
    case EcsOpU8:   return *(ecs_u8_t*)ptr != 0;
    case EcsOpChar: return *(ecs_char_t*)ptr != 0;
    case EcsOpByte: return *(ecs_u8_t*)ptr != 0;
    case EcsOpI16:  return *(ecs_i16_t*)ptr != 0;
    case EcsOpU16:  return *(ecs_u16_t*)ptr != 0;
    case EcsOpI32:  return *(ecs_i32_t*)ptr != 0;
    case EcsOpU32:  return *(ecs_u32_t*)ptr != 0;
    case EcsOpI64:  return *(ecs_i64_t*)ptr != 0;
    case EcsOpU64:  return *(ecs_u64_t*)ptr != 0;
    case EcsOpIPtr: return *(ecs_iptr_t*)ptr != 0;
    case EcsOpUPtr: return *(ecs_uptr_t*)ptr != 0;
    case EcsOpF32:  return *(ecs_f32_t*)ptr != 0;
    case EcsOpF64:  return *(ecs_f64_t*)ptr != 0;
    case EcsOpString: return *(const char**)ptr != NULL;
    case EcsOpEnum: return *(ecs_i32_t*)ptr != 0;
    case EcsOpBitmask: return *(ecs_u32_t*)ptr != 0;
    case EcsOpEntity: return *(ecs_entity_t*)ptr != 0;
    default: ecs_throw(ECS_INVALID_PARAMETER,
                "invalid element for bool");
    }

error:
    return 0;
}

char ecs_meta_get_char(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpChar: return *(ecs_char_t*)ptr != 0;
    default: ecs_throw(ECS_INVALID_PARAMETER,
                "invalid element for char");
    }

error:
    return 0;
}

int64_t ecs_meta_get_int(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpBool: return *(ecs_bool_t*)ptr;
    case EcsOpI8:   return *(ecs_i8_t*)ptr;
    case EcsOpU8:   return *(ecs_u8_t*)ptr;
    case EcsOpChar: return *(ecs_char_t*)ptr;
    case EcsOpByte: return *(ecs_u8_t*)ptr;
    case EcsOpI16:  return *(ecs_i16_t*)ptr;
    case EcsOpU16:  return *(ecs_u16_t*)ptr;
    case EcsOpI32:  return *(ecs_i32_t*)ptr;
    case EcsOpU32:  return *(ecs_u32_t*)ptr;
    case EcsOpI64:  return *(ecs_i64_t*)ptr;
    case EcsOpU64:  return flecs_uto(int64_t, *(ecs_u64_t*)ptr);
    case EcsOpIPtr: return *(ecs_iptr_t*)ptr;
    case EcsOpUPtr: return flecs_uto(int64_t, *(ecs_uptr_t*)ptr);
    case EcsOpF32:  return (int64_t)*(ecs_f32_t*)ptr;
    case EcsOpF64:  return (int64_t)*(ecs_f64_t*)ptr;
    case EcsOpString: return atoi(*(const char**)ptr);
    case EcsOpEnum: return *(ecs_i32_t*)ptr;
    case EcsOpBitmask: return *(ecs_u32_t*)ptr;
    case EcsOpEntity:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from entity to int");
        break;
    default: ecs_throw(ECS_INVALID_PARAMETER, "invalid element for int");
    }

error:
    return 0;
}

uint64_t ecs_meta_get_uint(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpBool: return *(ecs_bool_t*)ptr;
    case EcsOpI8:   return flecs_ito(uint64_t, *(ecs_i8_t*)ptr);
    case EcsOpU8:   return *(ecs_u8_t*)ptr;
    case EcsOpChar: return flecs_ito(uint64_t, *(ecs_char_t*)ptr);
    case EcsOpByte: return flecs_ito(uint64_t, *(ecs_u8_t*)ptr);
    case EcsOpI16:  return flecs_ito(uint64_t, *(ecs_i16_t*)ptr);
    case EcsOpU16:  return *(ecs_u16_t*)ptr;
    case EcsOpI32:  return flecs_ito(uint64_t, *(ecs_i32_t*)ptr);
    case EcsOpU32:  return *(ecs_u32_t*)ptr;
    case EcsOpI64:  return flecs_ito(uint64_t, *(ecs_i64_t*)ptr);
    case EcsOpU64:  return *(ecs_u64_t*)ptr;
    case EcsOpIPtr: return flecs_ito(uint64_t, *(ecs_i64_t*)ptr);
    case EcsOpUPtr: return *(ecs_uptr_t*)ptr;
    case EcsOpF32:  return flecs_ito(uint64_t, *(ecs_f32_t*)ptr);
    case EcsOpF64:  return flecs_ito(uint64_t, *(ecs_f64_t*)ptr);
    case EcsOpString: return flecs_ito(uint64_t, atoi(*(const char**)ptr));
    case EcsOpEnum: return flecs_ito(uint64_t, *(ecs_i32_t*)ptr);
    case EcsOpBitmask: return *(ecs_u32_t*)ptr;
    case EcsOpEntity: return *(ecs_entity_t*)ptr;
    default: ecs_throw(ECS_INVALID_PARAMETER, "invalid element for uint");
    }
error:
    return 0;
}

double ecs_meta_get_float(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpBool: return *(ecs_bool_t*)ptr;
    case EcsOpI8:   return *(ecs_i8_t*)ptr;
    case EcsOpU8:   return *(ecs_u8_t*)ptr;
    case EcsOpChar: return *(ecs_char_t*)ptr;
    case EcsOpByte: return *(ecs_u8_t*)ptr;
    case EcsOpI16:  return *(ecs_i16_t*)ptr;
    case EcsOpU16:  return *(ecs_u16_t*)ptr;
    case EcsOpI32:  return *(ecs_i32_t*)ptr;
    case EcsOpU32:  return *(ecs_u32_t*)ptr;
    case EcsOpI64:  return (double)*(ecs_i64_t*)ptr;
    case EcsOpU64:  return (double)*(ecs_u64_t*)ptr;
    case EcsOpIPtr: return (double)*(ecs_iptr_t*)ptr;
    case EcsOpUPtr: return (double)*(ecs_uptr_t*)ptr;
    case EcsOpF32:  return (double)*(ecs_f32_t*)ptr;
    case EcsOpF64:  return *(ecs_f64_t*)ptr;
    case EcsOpString: return atof(*(const char**)ptr);
    case EcsOpEnum: return *(ecs_i32_t*)ptr;
    case EcsOpBitmask: return *(ecs_u32_t*)ptr;
    case EcsOpEntity:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from entity to float");
        break;
    default: ecs_throw(ECS_INVALID_PARAMETER, "invalid element for float");
    }
error:
    return 0;
}

const char* ecs_meta_get_string(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpString: return *(const char**)ptr;
    default: ecs_throw(ECS_INVALID_PARAMETER, "invalid element for string");
    }
error:
    return 0;
}

ecs_entity_t ecs_meta_get_entity(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_meta_cursor_get_scope(cursor);
    ecs_meta_type_op_t *op = flecs_meta_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, scope);
    switch(op->kind) {
    case EcsOpEntity: return *(ecs_entity_t*)ptr;
    default: ecs_throw(ECS_INVALID_PARAMETER, "invalid element for entity");
    }
error:
    return 0;
}

#endif
