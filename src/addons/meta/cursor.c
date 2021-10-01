#include "meta.h"

#ifdef FLECS_META

/* Get current scope */
static
ecs_meta_scope_t* get_scope(
    ecs_meta_cursor_t *cursor)
{
    ecs_assert(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    return &cursor->scope[cursor->depth];
}

/* Get current scope */
static
ecs_meta_type_op_t* get_op(
    ecs_meta_scope_t *scope)
{
    return &scope->ops[scope->op_cur];
}

/* Get component for type in current scope */
static
const EcsComponent* get_component_ptr(
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
    return get_component_ptr(world, scope)->size;
}

/* Get alignment for type in current scope */
static
ecs_size_t get_alignment(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    return get_component_ptr(world, scope)->alignment;
}

static
int32_t get_elem_count(
    ecs_meta_scope_t *scope)
{    
    if (scope->vector) {
        return ecs_vector_count(*(scope->vector));
    }

    ecs_meta_type_op_t *op = get_op(scope);
    return op->count;
}

/* Get pointer to current field/element */
static
ecs_meta_type_op_t* get_ptr(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope)
{
    ecs_meta_type_op_t *op = get_op(scope);
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
void push_type(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope,
    ecs_entity_t type,
    void *ptr)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    ecs_assert(ser != NULL, ECS_INVALID_PARAMETER, NULL);

    scope[0] = (ecs_meta_scope_t) {
        .type = type,
        .ops = ecs_vector_first(ser->ops, ecs_meta_type_op_t),
        .op_count = ecs_vector_count(ser->ops),
        .ptr = ptr
    };
}

ecs_meta_cursor_t ecs_meta_cursor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_meta_cursor_t result = {
        .world = world
    };

    push_type(world, result.scope, type, ptr);

    return result;
}

void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor)
{
    return get_ptr(cursor->world, get_scope(cursor));
}

int ecs_meta_next(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);

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

int ecs_meta_push(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    const ecs_world_t *world = cursor->world;

    void *ptr = get_ptr(world, scope);
    cursor->depth ++;
    ecs_assert(cursor->depth < ECS_META_MAX_SCOPE_DEPTH, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_meta_scope_t *next_scope = get_scope(cursor);

    /* If we're not already in an inline array and this operation is an inline
     * array, push a frame for the array.
     * Doing this first ensures that inline arrays take precedence over other
     * kinds of push operations, such as for a struct element type. */
    if (!scope->is_inline_array && op->count > 1) {
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

    case EcsOpArray:
        push_type(world, next_scope, op->type, ptr);
        break;

    case EcsOpVector:
        next_scope->vector = ptr;
        push_type(world, next_scope, op->type, NULL);
        break;

    default: {
        char *path = ecs_get_fullpath(world, scope->type);
        ecs_err("invalid push for type '%s'", path);
        ecs_os_free(path);
        return -1;
    }
    }

    if (scope->is_collection) {
        next_scope[0].ptr = ECS_OFFSET(next_scope[0].ptr, 
            scope->elem_cur * get_size(world, scope));
    }

    return 0;
}

int ecs_meta_pop(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    cursor->depth --;

    ecs_meta_scope_t *next_scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(next_scope);

    if (!scope->is_inline_array) {
        /* The last op from the previous scope must be a push */
        ecs_assert(op->kind == EcsOpPush, ECS_INTERNAL_ERROR, NULL);
        next_scope->op_cur += op->op_count - 1;

        /* push + op_count should point to the operation after pop */
        op = get_op(next_scope);
        ecs_assert(op->kind == EcsOpPop, ECS_INTERNAL_ERROR, NULL);
    } else {
        /* Make sure that this was an inline array */
        ecs_assert(next_scope->op_count > 1, ECS_INTERNAL_ERROR, NULL);
    }

    return 0;
}

/* Utility macro's to let the compiler do the conversion work for us */
#define set_T(T, ptr, value)\
    ((T*)ptr)[0] = ((T)value)

#define case_T(kind, T, dst, src)\
case kind:\
    set_T(T, dst, src);\
    break

#define cases_T_float(dst, src)\
    case_T(EcsOpF32,  ecs_f32_t,  dst, src);\
    case_T(EcsOpF64,  ecs_f64_t,  dst, src)

#define cases_T_signed(dst, src)\
    case_T(EcsOpChar, ecs_char_t, dst, src);\
    case_T(EcsOpI8,   ecs_i8_t,   dst, src);\
    case_T(EcsOpI16,  ecs_i16_t,  dst, src);\
    case_T(EcsOpI32,  ecs_i32_t,  dst, src);\
    case_T(EcsOpI64,  ecs_i64_t,  dst, src);\
    case_T(EcsOpIPtr, ecs_iptr_t, dst, src)

#define cases_T_unsigned(dst, src)\
    case_T(EcsOpByte, ecs_byte_t, dst, src);\
    case_T(EcsOpU8,   ecs_u8_t,   dst, src);\
    case_T(EcsOpU16,  ecs_u16_t,  dst, src);\
    case_T(EcsOpU32,  ecs_u32_t,  dst, src);\
    case_T(EcsOpU64,  ecs_u64_t,  dst, src);\
    case_T(EcsOpUPtr, ecs_uptr_t, dst, src);\

#define cases_T_bool(dst, src)\
case EcsOpBool:\
    set_T(ecs_bool_t, dst, value != 0);\
    break

int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_unsigned(ptr, value);
    default:
        ecs_err("unsupported conversion from bool");
        return -1;
    }

    return 0;
}

int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value);
    default:
        ecs_err("unsupported conversion from char");
        return -1;
    }

    return 0;
}

int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value);
    cases_T_float(ptr, value);
    default:
        ecs_err("unsupported conversion from int");
        return -1;
    }

    return 0;
}

int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_unsigned(ptr, value);
    cases_T_float(ptr, value);
    default:
        ecs_err("unsupported conversion from uint");
        return -1;
    }

    return 0;
}

int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value);
    cases_T_unsigned(ptr, value);
    cases_T_float(ptr, value);
    default:
        ecs_err("unsupported conversion from float");
        return -1;
    }

    return 0;
}

int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    case EcsOpString:
        ecs_os_free(*(char**)ptr);
        *(char**)ptr = ecs_os_strdup(value);
        break;
    default:
        ecs_err("unsupported conversion from string");
        return -1;
    }

    return 0;
}

int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value)
{
    ecs_meta_scope_t *scope = get_scope(cursor);
    ecs_meta_type_op_t *op = get_op(scope);
    void *ptr = get_ptr(cursor->world, scope);

    switch(op->kind) {
    case EcsOpEntity:
        set_T(ecs_entity_t, ptr, value);
        break;
    default:
        ecs_err("unsupported conversion from entity");
        return -1;
    }

    return 0;
}

#endif
