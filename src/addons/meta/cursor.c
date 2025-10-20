/**
 * @file addons/meta/cursor.c
 * @brief API for assigning values of runtime types with reflection.
 */

#include "meta.h"
#include <inttypes.h>

#ifdef FLECS_META
#ifdef FLECS_QUERY_DSL
#include "../query_dsl/query_dsl.h"
#endif

const char* flecs_meta_op_kind_str(
    ecs_meta_op_kind_t kind)
{
    switch(kind) {

    case EcsOpEnum: return "Enum";
    case EcsOpBitmask: return "Bitmask";
    case EcsOpForward: return "Forward";
    case EcsOpPushStruct: return "Push";
    case EcsOpPushArray: return "PushArray";
    case EcsOpPushVector: return "PushVector";
    case EcsOpOpaqueValue: return "OpaqueValue";
    case EcsOpOpaqueStruct: return "OpaqueStruct";
    case EcsOpOpaqueArray: return "OpaqueArray";
    case EcsOpOpaqueVector: return "OpaqueVector";
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
    case EcsOpId: return "Id";
    case EcsOpScope: return "Scope";
    default: return "<< invalid kind >>";
    }
}

ecs_size_t ecs_meta_op_get_elem_count(
    const ecs_meta_op_t *op,
    const void *ptr)
{
    ecs_assert(op->kind == EcsOpPushArray || op->kind == EcsOpPushVector, 
        ECS_INTERNAL_ERROR, NULL);
    if (op->kind == EcsOpPushArray) {
        const ecs_meta_op_t *pop = &op[op->op_count - 1];
        ecs_assert(pop != NULL, ECS_INTERNAL_ERROR, NULL);
        return pop->elem_size;
    } else if (op->kind == EcsOpPushVector) {
        return ((const ecs_vec_t*)ptr)->count;
    }
    ecs_throw(ECS_INVALID_OPERATION, 
        "cannot get element count for %s operation",
        flecs_meta_op_kind_str(op->kind));
error:
    return 0;
}

/* Get current scope */
static
ecs_meta_scope_t* flecs_cursor_get_scope(
    const ecs_meta_cursor_t *cursor)
{
    ecs_check(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    return ECS_CONST_CAST(ecs_meta_scope_t*, &cursor->scope[cursor->depth]);
error:
    return NULL;
}

/* Restore scope, if dotmember was used */
static
ecs_meta_scope_t* flecs_cursor_restore_scope(
    ecs_meta_cursor_t *cursor,
    const ecs_meta_scope_t* scope)
{
    ecs_check(cursor != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(scope != NULL, ECS_INVALID_PARAMETER, NULL);
    if (scope->prev_depth) {
        cursor->depth = scope->prev_depth;
    }
error:
    return (ecs_meta_scope_t*)&cursor->scope[cursor->depth];
}

/* Get current operation for scope */
static
ecs_meta_op_t* flecs_cursor_get_op(
    ecs_meta_scope_t *scope)
{
    ecs_assert(scope->ops != NULL, ECS_INVALID_OPERATION, 
        "type serializer is missing instructions");
    return &scope->ops[scope->ops_cur];
}

static
ecs_meta_op_kind_t flecs_cursor_get_collection_kind(
    ecs_meta_scope_t *scope)
{
    /* Can only get collection kind for collection scope */
    ecs_assert(scope->is_collection, ECS_INTERNAL_ERROR, NULL);

    /* If this is a collection scope, the operation preceding the 
     * current one must be PushArray or PushVector. 
     * We can't get this from parent->ops, because if the collection was
     * pushed from a struct/other collection scope, it will have done so
     * with a Forward instruction. */
    ecs_meta_op_t *push = &scope->ops[-1];
    ecs_assert(push->kind == EcsOpPushArray || 
        push->kind == EcsOpPushVector, ECS_INTERNAL_ERROR, NULL);
    return push->kind;
}

static
ecs_size_t flecs_cursor_get_elem_size(
    ecs_meta_scope_t *scope)
{
    /* Can only get collection kind for collection scope */
    ecs_assert(scope->is_collection, ECS_INTERNAL_ERROR, NULL);

    /* The first operation in a collection scope always has the element size.
     * The reason is that if this is a primitive scope it only contains a single
     * operation with the type and size, and if it's a composite type the first
     * operation is a Push for the element type. */
    ecs_assert(scope->ops[0].type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    return scope->ops[0].type_info->size;
}

/* Get pointer to current field/element */
static
void* flecs_meta_cursor_get_ptr(
    const ecs_world_t *world,
    const ecs_meta_cursor_t *cursor,
    ecs_meta_scope_t *scope)
{
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    const EcsOpaque *opaque = scope->opaque;

    if (!opaque) {
        if (scope->is_collection) {
            ecs_assert(cursor->scope != scope, ECS_INTERNAL_ERROR, NULL);
            /* Parent scope contains the index to the current element and total
             * number of elements in collection. */
            ecs_meta_scope_t *parent = &scope[-1];
            ecs_size_t elem_size = flecs_cursor_get_elem_size(scope);
            ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_meta_op_kind_t kind = flecs_cursor_get_collection_kind(scope);

            if (kind == EcsOpPushVector) {
                if (parent->elem >= parent->elem_count) {
                    ecs_vec_t *vec = flecs_meta_cursor_get_ptr(
                        world, cursor, parent);
                    if (vec) {
                        ecs_vec_init_if(vec, elem_size);
                        ecs_vec_set_min_count_w_type_info(
                            NULL, vec, elem_size, parent->elem + 1, 
                            op->type_info);
                        scope->ptr = vec->array;
                        if (parent->elem >= parent->elem_count) {
                            parent->elem_count = parent->elem + 1;
                        }
                    }
                }
            }

            scope->is_empty_scope = false;

            void *elem = ECS_ELEM(scope->ptr, elem_size, parent->elem);
            return ECS_OFFSET(elem, op->offset);
        } else {
            return ECS_OFFSET(scope->ptr, op->offset);
        }
    }

    /* Opaque type */

    if (scope->is_collection) {
        ecs_assert(scope != cursor->scope, ECS_INTERNAL_ERROR, NULL);
        ecs_meta_scope_t *parent = &scope[-1];

        if (!opaque->ensure_element) {
            char *str = ecs_get_path(world, scope->type);
            ecs_err("missing ensure_element() for opaque type %s", str);
            ecs_os_free(str);
            return NULL;
        }

        scope->is_empty_scope = false;

        void *opaque_ptr = opaque->ensure_element(scope->ptr, 
            flecs_itosize(parent->elem));
        ecs_assert(opaque_ptr != NULL, ECS_INVALID_OPERATION, 
            "ensure_element() returned NULL");

        return opaque_ptr;
    } else if (op->name) {
        if (!opaque->ensure_member) {
            char *str = ecs_get_path(world, scope->type);
            ecs_err("missing ensure_member() for opaque type %s", str);
            ecs_os_free(str);
            return NULL;
        }

        ecs_assert(scope->ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        return opaque->ensure_member(scope->ptr, op->name);
    } else {
        ecs_err("invalid operation for opaque type");
        return NULL;
    }
}

static
int flecs_meta_cursor_push_type(
    const ecs_world_t *world,
    ecs_meta_scope_t *scope,
    ecs_entity_t type,
    void *ptr)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *str = ecs_id_str(world, type);
        ecs_err("cannot open scope for '%s' (missing reflection data)", str);
        ecs_os_free(str);
        return -1;
    }

    scope[0] = (ecs_meta_scope_t) {
        .type = type,
        .ops = ecs_vec_first_t(&ser->ops, ecs_meta_op_t),
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

    ecs_meta_cursor_t result = {
        .world = world,
        .valid = true
    };

    if (flecs_meta_cursor_push_type(world, result.scope, type, ptr) < 0) {
        result.valid = false;
    }

    return result;
error:
    return (ecs_meta_cursor_t){ 0 };
}

void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor)
{
    return flecs_meta_cursor_get_ptr(cursor->world, cursor,
        flecs_cursor_get_scope(cursor));
}

static
int flecs_meta_array_bounds_check(
    ecs_meta_cursor_t *cursor,
    ecs_meta_scope_t *scope)
{
    ecs_assert(cursor->scope != scope, ECS_INTERNAL_ERROR, NULL);
    ecs_meta_scope_t *parent = &scope[-1];
    ecs_meta_op_kind_t kind = flecs_cursor_get_collection_kind(scope);
    if (parent->ptr && kind != EcsOpPushVector) {
        if (parent->elem >= parent->elem_count || parent->elem < 0) {
            char *str = ecs_get_path(cursor->world, parent->type);
            ecs_err("out of collection bounds for type '%s' (%d elements vs. size %d)", 
                str, parent->elem + 1, parent->elem_count);
            ecs_os_free(str);
            return -1;
        }
    }

    return 0;
}

int ecs_meta_next(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    scope = flecs_cursor_restore_scope(cursor, scope);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);

    if (scope->is_collection) {
        ecs_assert(cursor->scope != scope, ECS_INTERNAL_ERROR, NULL);
        ecs_meta_scope_t *parent = &scope[-1];
        parent->elem ++;

        scope->ops_cur = 0;

        if (scope->opaque) {
            return 0;
        }

        if (flecs_meta_array_bounds_check(cursor, scope)) {
            return -1;
        }

        return 0;
    }

    scope->ops_cur += op->op_count;

    if (scope->ops_cur >= scope->ops_count) {
        char *str = ecs_get_path(cursor->world, scope->type);
        ecs_err("too many elements for scope for type %s", str);
        ecs_os_free(str);
        return -1;
    }

    return 0;
}

int ecs_meta_elem(
    ecs_meta_cursor_t *cursor,
    int32_t elem)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    if (!scope->is_collection) {
        ecs_err("ecs_meta_elem can be used for collections only");
        return -1;
    }

    ecs_assert(scope != cursor->scope, ECS_INTERNAL_ERROR, NULL);
    ecs_meta_scope_t *parent = &scope[-1];

    parent->elem = elem;
    scope->ops_cur = 0;
    scope->is_moved_scope = true;

    if (flecs_meta_array_bounds_check(cursor, scope)) {
        return -1;
    }
    
    return 0;
}

static
int flecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name,
    bool try)
{
    if (cursor->depth == 0) {
        if (!try) ecs_err("cannot move to member in root scope (push first)");
        return -1;
    }

    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    scope = flecs_cursor_restore_scope(cursor, scope);

    ecs_hashmap_t *members = scope->members;
    const ecs_world_t *world = cursor->world;

    if (!members) {
        if (!try) ecs_err("cannot move to member '%s' for non-struct type '%s'", 
            name, flecs_errstr(ecs_get_path(world, scope->type)));
        return -1;
    }

    const uint64_t *cur_ptr = flecs_name_index_find_ptr(members, name, 0, 0);
    if (!cur_ptr) {
        if (!try) ecs_err("unknown member '%s' for type '%s'", 
            name, flecs_errstr(ecs_get_path(world, scope->type)));
        return -1;
    }

    scope->ops_cur = flecs_uto(int16_t, cur_ptr[0]);

    const EcsOpaque *opaque = scope->opaque;
    if (opaque) {
        if (!opaque->ensure_member) {
            if (!try) ecs_err("missing ensure_member for opaque type %s",
                flecs_errstr(ecs_get_path(world, scope->type)));
        }
    }

    return 0;
}

int ecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
    return flecs_meta_member(cursor, name, false);
}

int ecs_meta_try_member(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
    return flecs_meta_member(cursor, name, true);
}

static
const char* flecs_meta_parse_member(
    const char *start,
    char *token_out)
{
    const char *ptr;
    char ch;
    for (ptr = start; (ch = *ptr); ptr ++) {
        if (ch == '.') {
            break;
        }
    }

    int32_t len = flecs_ito(int32_t, ptr - start);
    ecs_os_memcpy(token_out, start, len);
    token_out[len] = '\0';
    if (ch == '.') {
        ptr ++;
    }

    return ptr;
}

static
int flecs_meta_dotmember(
    ecs_meta_cursor_t *cursor,
    const char *name,
    bool try)
{
    ecs_meta_scope_t *cur_scope = flecs_cursor_get_scope(cursor);
    flecs_cursor_restore_scope(cursor, cur_scope);

    int16_t prev_depth = cursor->depth;
    int dotcount = 0;

    char token[ECS_MAX_TOKEN_SIZE];
    const char *ptr = name;
    while ((ptr = flecs_meta_parse_member(ptr, token))) {
        if (dotcount) {
            ecs_meta_push(cursor);
        }

        if (flecs_meta_member(cursor, token, try)) {
            goto error;
        }

        if (!ptr[0]) {
            break;   
        }

        dotcount ++;
    }

    cur_scope = flecs_cursor_get_scope(cursor);
    if (dotcount) {
        cur_scope->prev_depth = prev_depth;
    }

    return 0;
error:
    return -1;
}

int ecs_meta_dotmember(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
    return flecs_meta_dotmember(cursor, name, false);
}

int ecs_meta_try_dotmember(
    ecs_meta_cursor_t *cursor,
    const char *name)
{
    return flecs_meta_dotmember(cursor, name, true);
}

int ecs_meta_push(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    const ecs_world_t *world = cursor->world;

    /* Allow push for primitive scope if it's the root. Useful for scenarios 
     * where a value always has to be surrounded by { }. */
    if (cursor->depth == 0) {
        if (!cursor->is_primitive_scope && (op->kind > EcsOpScope)) {
            cursor->is_primitive_scope = true;
            return 0;
        }
    }

    void *ptr = flecs_meta_cursor_get_ptr(world, cursor, scope);
    cursor->depth ++;
    ecs_check(cursor->depth < ECS_META_MAX_SCOPE_DEPTH,
        ECS_INVALID_PARAMETER, NULL);

    ecs_meta_scope_t *next_scope = flecs_cursor_get_scope(cursor);
    *next_scope = (ecs_meta_scope_t) {0};

    /* Check if we need to forward to serializer of other type. */
    if (op->kind == EcsOpForward) {
        const EcsTypeSerializer *ts = ecs_get(
            world, op->type, EcsTypeSerializer);
        ecs_assert(ts != NULL, ECS_INTERNAL_ERROR, NULL);
        op = ecs_vec_first(&ts->ops);
    } else if (op->kind == EcsOpOpaqueStruct) {
        const EcsOpaque *opaque = ecs_get(world, op->type, EcsOpaque);
        next_scope->opaque = opaque;

        const EcsTypeSerializer *ts = ecs_get(
            world, opaque->as_type, EcsTypeSerializer);
        ecs_assert(ts != NULL, ECS_INTERNAL_ERROR, NULL);
        op = ecs_vec_first(&ts->ops);

    } else if (op->kind == EcsOpOpaqueVector || op->kind == EcsOpOpaqueArray) {
        const EcsOpaque *opaque = ecs_get(world, op->type, EcsOpaque);
        next_scope->opaque = opaque;

        const EcsTypeSerializer *ts = ecs_get(
            world, opaque->as_type, EcsTypeSerializer);
        ecs_assert(ts != NULL, ECS_INTERNAL_ERROR, NULL);
        op = ecs_vec_first(&ts->ops);

        next_scope->is_collection = true;
        next_scope->is_empty_scope = true;
        next_scope->is_moved_scope = false;
        scope->elem = 0;
    }

    next_scope->ops = &op[1]; /* op after push */
    next_scope->ops_count = op->op_count - 1;
    next_scope->ptr = ptr;
    next_scope->type = op->type;
    next_scope->members = op->is.members;

    if (!next_scope->opaque) {
        if (op->kind == EcsOpPushStruct) {
            ecs_assert(next_scope->members != NULL, ECS_INTERNAL_ERROR, NULL);
        } else 
        if (op->kind == EcsOpPushArray) {
            next_scope->is_collection = true;

            ecs_meta_op_t *pop = &op[op->op_count - 1];
            ecs_assert(pop->kind == EcsOpPop, ECS_INTERNAL_ERROR, NULL);
            scope->elem = 0;
            scope->elem_count = pop->elem_size; /* Array count is encoded on pop */
        } else
        if (op->kind == EcsOpPushVector) {
            ecs_vec_t *vec = ptr;

            next_scope->ptr = vec ? vec->array : NULL;
            next_scope->is_collection = true;
            next_scope->is_empty_scope = true;
            next_scope->is_moved_scope = false;

            scope->elem = 0;
            scope->elem_count = vec ? vec->count : 0;
        } else {
            goto error;
        }
    }

    return 0;
error: {
    char *path = ecs_get_path(world, scope->type);
    ecs_err("cannot open scope for type '%s'", path);
    ecs_os_free(path);
    return -1;
}
}

int ecs_meta_pop(
    ecs_meta_cursor_t *cursor)
{
    if (cursor->is_primitive_scope) {
        cursor->is_primitive_scope = false;
        return 0;
    }

    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    scope = flecs_cursor_restore_scope(cursor, scope);
    cursor->depth --;
    if (cursor->depth < 0) {
        ecs_err("unexpected end of scope");
        return -1;
    }

    ecs_meta_scope_t *next_scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(next_scope);

    if (op->kind == EcsOpPushStruct || op->kind == EcsOpPushArray || 
        op->kind == EcsOpPushVector)
    {
        next_scope->ops_cur += flecs_ito(int16_t, op->op_count - 1);

        if (op->kind == EcsOpPushVector) {
            /* If scope got moved around in this is a partially assigned vector
             * so don't shrink it. */
            if (!scope->is_moved_scope) {
                ecs_assert(cursor->scope != scope, ECS_INTERNAL_ERROR, NULL);
                ecs_meta_scope_t *parent = &scope[-1];
                ecs_vec_t *vec = parent->ptr;

                if (vec) {
                    ecs_size_t elem_size = flecs_cursor_get_elem_size(scope);
                    ecs_assert(elem_size > 0, ECS_INTERNAL_ERROR, NULL);
                    ecs_vec_init_if(vec, elem_size);

                    ecs_meta_op_t *elem_op = &scope->ops[0];
                    if (scope->is_empty_scope) {
                        ecs_vec_set_count_w_type_info(
                            NULL, vec, elem_size, 0, elem_op->type_info);
                    } else {
                        ecs_vec_set_count_w_type_info(NULL, vec, elem_size, 
                            parent->elem + 1, elem_op->type_info);
                    }
                }
            }
        }

        /* push + op_count should point to the operation after pop */
        op = flecs_cursor_get_op(next_scope);
        ecs_assert(op->kind == EcsOpPop, ECS_INTERNAL_ERROR, NULL);
    } else if (op->kind == EcsOpForward || op->kind == EcsOpOpaqueStruct || 
        op->kind == EcsOpOpaqueArray) 
    {
        next_scope->ops_cur += flecs_ito(int16_t, op->op_count - 1);
    } else if (op->kind == EcsOpOpaqueVector) {
        const EcsOpaque *opaque = scope->opaque;

        /* When popping an opaque collection type, call resize to make
         * sure the vector isn't larger than the number of elements we
         * deserialized. If the opaque type represents an array, don't call 
         * resize. */
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!opaque->resize) {
            char *str = ecs_get_path(cursor->world, scope->type);
            ecs_err("missing resize for opaque type %s", str);
            ecs_os_free(str);
            return -1;
        }

        if (scope->ptr) {
            if (scope->is_empty_scope) {
                /* If no values were serialized for scope, resize 
                * collection to 0 elements. */
                opaque->resize(scope->ptr, 0);
            } else {
                /* Otherwise resize collection to the index of the last
                * deserialized element + 1 */
                opaque->resize(scope->ptr, 
                    flecs_ito(size_t, next_scope->elem + 1));
            }
        }
    } else {
        /* should not have been able to push if the previous scope was not
         * a complex or collection type */
        ecs_assert(false, ECS_INTERNAL_ERROR, "invalid operation for pop: %s",
            flecs_meta_op_kind_str(op->kind));
    }

    return 0;
}

bool ecs_meta_is_collection(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    return scope->is_collection;
}

ecs_entity_t ecs_meta_get_type(
    const ecs_meta_cursor_t *cursor)
{
    if (cursor->depth == 0) {
        return cursor->scope[0].type;
    }

    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    return op->type;
}

ecs_entity_t ecs_meta_get_unit(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_entity_t type = scope->type;
    const EcsStruct *st = ecs_get(cursor->world, type, EcsStruct);
    if (!st) {
        return 0;
    }

    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    ecs_member_t *m = ecs_vec_get_t(
        &st->members, ecs_member_t, op->member_index);

    return m->unit;
}

const char* ecs_meta_get_member(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    return op->name;
}

ecs_entity_t ecs_meta_get_member_id(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_entity_t type = scope->type;
    const EcsStruct *st = ecs_get(cursor->world, type, EcsStruct);
    if (!st) {
        return 0;
    }

    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    ecs_member_t *m = ecs_vec_get_t(
        &st->members, ecs_member_t, op->member_index);

    return m->member;
}

/* Utilities for type conversions and bounds checking */
static struct {
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
    [EcsOpId]      = {0,          INT64_MAX},
    [EcsOpEnum]    = {INT32_MIN,  INT32_MAX},
    [EcsOpBitmask] = {0,          INT32_MAX}
};

static struct {
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
    [EcsOpId]      = {0,          UINT64_MAX},
    [EcsOpEnum]    = {0,          INT32_MAX},
    [EcsOpBitmask] = {0,          UINT32_MAX}
};

static struct {
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
    [EcsOpId]      = {0,          (double)UINT64_MAX},
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
    case_T_checked(EcsOpId, ecs_u64_t, dst, src, bounds);\
    case_T_checked(EcsOpBitmask, ecs_u32_t, dst, src, bounds)

#define cases_T_bool(dst, src)\
case EcsOpBool:\
    set_T(ecs_bool_t, dst, value != 0);\
    break

static
void flecs_meta_conversion_error(
    ecs_meta_cursor_t *cursor,
    ecs_meta_op_t *op,
    const char *from)
{
    if (op->kind == EcsOpPop) {
        ecs_err("cursor: out of bounds");
    } else {
        char *path = ecs_get_path(cursor->world, op->type);
        ecs_err("unsupported conversion from %s to '%s' (for %s)", 
            from, path, flecs_meta_op_kind_str(op->kind));
        ecs_os_free(path);
    }
}

int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_signed);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_unsigned);
    case EcsOpString: {
        char *result;
        if (value) {
            result = ecs_os_strdup("true");
        } else {
            result = ecs_os_strdup("false");
        }
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);
        if (opaque->assign_bool) {
            opaque->assign_bool(ptr, value);
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpPrimitive:
    case EcsOpF32:
    case EcsOpF64:
        flecs_meta_conversion_error(cursor, op, "bool");
        return -1;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_signed);
    case EcsOpString: {
        char *result = flecs_asprintf("%c", value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);
        if (opaque->assign_char) { /* preferred operation */
            opaque->assign_char(ptr, value);
            break;
        } else if (opaque->assign_uint) {
            opaque->assign_uint(ptr, (uint64_t)value);
            break;
        } else if (opaque->assign_int) {
            opaque->assign_int(ptr, value);
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpEntity:
    case EcsOpId:
        flecs_meta_conversion_error(cursor, op, "char");
        return -1;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    ecs_assert(ptr != NULL, ECS_INVALID_OPERATION, "no object to assign");

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_signed);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_signed);
    cases_T_float(ptr, value);
    case EcsOpString: {
        char *result = flecs_asprintf("%"PRId64, value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);
        if (opaque->assign_int) { /* preferred operation */
            opaque->assign_int(ptr, value);
            break;
        } else if (opaque->assign_float) { /* most expressive */
            opaque->assign_float(ptr, (double)value);
            break;
        } else if (opaque->assign_uint && (value > 0)) {
            opaque->assign_uint(ptr, flecs_ito(uint64_t, value));
            break;
        } else if (opaque->assign_char && (value > 0) && (value < 256)) {
            opaque->assign_char(ptr, flecs_ito(char, value));
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpPrimitive: {
        if(!value) return ecs_meta_set_null(cursor);
        flecs_meta_conversion_error(cursor, op, "int");
        return -1;
    }
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    ecs_assert(ptr != NULL, ECS_INVALID_OPERATION, "no object to assign");

    switch(op->kind) {
    cases_T_bool(ptr, value);
    cases_T_signed(ptr, value, ecs_meta_bounds_unsigned);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_unsigned);
    cases_T_float(ptr, value);
    case EcsOpString: {
        char *result = flecs_asprintf("%"PRIu64, value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);
        if (opaque->assign_uint) { /* preferred operation */
            opaque->assign_uint(ptr, value);
            break;
        } else if (opaque->assign_float) { /* most expressive */
            opaque->assign_float(ptr, (double)value);
            break;
        } else if (opaque->assign_int && (value < INT64_MAX)) {
            opaque->assign_int(ptr, flecs_uto(int64_t, value));
            break;
        } else if (opaque->assign_char && (value < 256)) {
            opaque->assign_char(ptr, flecs_uto(char, value));
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpPrimitive:
        if(!value) return ecs_meta_set_null(cursor);
        flecs_meta_conversion_error(cursor, op, "uint");
        return -1;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    ecs_assert(ptr != NULL, ECS_INVALID_OPERATION, "no object to assign");

    switch(op->kind) {
    case EcsOpBool:
        if (ECS_EQZERO(value)) {
            set_T(bool, ptr, false);
        } else {
            set_T(bool, ptr, true);
        }
        break;
    cases_T_signed(ptr, value, ecs_meta_bounds_float);
    cases_T_unsigned(ptr, value, ecs_meta_bounds_float);
    cases_T_float(ptr, value);
    case EcsOpString: {
        char *result = flecs_asprintf("%f", value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INTERNAL_ERROR, NULL);
        if (opaque->assign_float) { /* preferred operation */
            opaque->assign_float(ptr, value);
            break;
        } else if (opaque->assign_int && /* most expressive */
            (value <= (double)INT64_MAX) && (value >= (double)INT64_MIN)) 
        {
            opaque->assign_int(ptr, (int64_t)value);
            break;
        } else if (opaque->assign_uint && (value >= 0)) {
            opaque->assign_uint(ptr, (uint64_t)value);
            break;
        } else if (opaque->assign_entity && (value >= 0)) {
            opaque->assign_entity(
                ptr, ECS_CONST_CAST(ecs_world_t*, cursor->world), 
                    (ecs_entity_t)value);
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpPrimitive:
        flecs_meta_conversion_error(cursor, op, "float");
        return -1;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_value(
    ecs_meta_cursor_t *cursor,
    const ecs_value_t *value)
{
    ecs_check(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t type = value->type;
    ecs_check(type != 0, ECS_INVALID_PARAMETER, NULL);
    const EcsType *mt = ecs_get(cursor->world, type, EcsType);
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
        case EcsEntity: return ecs_meta_set_entity(cursor, *(ecs_entity_t*)value->ptr);
        case EcsId: return ecs_meta_set_id(cursor, *(ecs_id_t*)value->ptr);
        default:
            ecs_throw(ECS_INTERNAL_ERROR, "invalid type kind");
            goto error;
        }
    } else if (mt->kind == EcsEnumType) {
        return ecs_meta_set_int(cursor, *(int32_t*)value->ptr);
    } else if (mt->kind == EcsBitmaskType) {
        return ecs_meta_set_int(cursor, *(uint32_t*)value->ptr);
    } else {
        ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
        ecs_meta_op_t *op = flecs_cursor_get_op(scope);
        void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
        if (op->type != value->type) {
            char *type_str = ecs_get_path(cursor->world, value->type);
            flecs_meta_conversion_error(cursor, op, type_str);
            ecs_os_free(type_str);
            goto error;
        }
        return ecs_value_copy(cursor->world, value->type, ptr, value->ptr);
    }

error:
    return -1;
}

static
int flecs_meta_add_bitmask_constant(
    ecs_meta_cursor_t *cursor,
    ecs_meta_op_t *op,
    void *out,
    const char *value)
{
    ecs_assert(op->type != 0, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_os_strcmp(value, "0")) {
        return 0;
    }

    ecs_entity_t c = ecs_lookup_child(cursor->world, op->type, value);
    if (!c) {
        char *path = ecs_get_path(cursor->world, op->type);
        ecs_err("unresolved bitmask constant '%s' for type '%s'", value, path);
        ecs_os_free(path);
        return -1;
    }

    const ecs_u32_t *v = ecs_get_pair_second(
        cursor->world, c, EcsConstant, ecs_u32_t);
    if (v == NULL) {
        char *path = ecs_get_path(cursor->world, op->type);
        ecs_err("'%s' is not an bitmask constant for type '%s'", value, path);
        ecs_os_free(path);
        return -1;
    }

    *(ecs_u32_t*)out |= v[0];

    return 0;
}

static
int flecs_meta_parse_bitmask(
    ecs_meta_cursor_t *cursor,
    ecs_meta_op_t *op,
    void *out,
    const char *value)
{
    char token[ECS_MAX_TOKEN_SIZE];

    const char *prev = value, *ptr = value;

    *(ecs_u32_t*)out = 0;

    while ((ptr = strchr(ptr, '|'))) {
        ecs_os_memcpy(token, prev, ptr - prev);
        token[ptr - prev] = '\0';
        if (flecs_meta_add_bitmask_constant(cursor, op, out, token) != 0) {
            return -1;
        }

        ptr ++;
        prev = ptr;
    }

    if (flecs_meta_add_bitmask_constant(cursor, op, out, prev) != 0) {
        return -1;
    }

    return 0;
}

static
int flecs_meta_cursor_lookup(
    ecs_meta_cursor_t *cursor,
    const char *value,
    ecs_entity_t *out)
{
    if (ecs_os_strcmp(value, "#0")) {
        if (cursor->lookup_action) {
            *out = cursor->lookup_action(
                ECS_CONST_CAST(ecs_world_t*, cursor->world), value,
                cursor->lookup_ctx);
        } else {
            *out = ecs_lookup_from(cursor->world, 0, value);
        }
        if (!*out) {
            ecs_err("unresolved entity identifier '%s'", value);
            return -1;
        }
    }
    return 0;
}

static
bool flecs_meta_valid_digit(
    const char *str)
{
    return str[0] == '-' || isdigit(str[0]);
}

int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    switch(op->kind) {
    case EcsOpI8:
    case EcsOpU8:
    case EcsOpByte:
    case EcsOpI16:
    case EcsOpU16:
    case EcsOpI32:
    case EcsOpU32:
    case EcsOpI64:
    case EcsOpU64:
    case EcsOpIPtr:
    case EcsOpUPtr:
    case EcsOpF32:
    case EcsOpF64:
        if (!flecs_meta_valid_digit(value)) {
            ecs_err("expected number, got '%s'", value);
            goto error;
        }
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpOpaqueValue:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpChar:
    case EcsOpString:
    case EcsOpEntity:
    case EcsOpId:
    case EcsOpScope:
        break;
    }

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
            goto error;
        }
        break;
    case EcsOpI8:
    case EcsOpU8:
    case EcsOpByte:
        set_T(ecs_i8_t, ptr, atol(value));
        break;
    case EcsOpChar:
        set_T(char, ptr, value[0]);
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
        set_T(ecs_i64_t, ptr, atoll(value));
        break;
    case EcsOpIPtr:
    case EcsOpUPtr:
        set_T(ecs_iptr_t, ptr, atoll(value));
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
            char *path = ecs_get_path(cursor->world, op->type);
            ecs_err("unresolved enum constant '%s' for type '%s'", value, path);
            ecs_os_free(path);
            goto error;
        }

        ecs_entity_t underlying = ecs_get_target(
            cursor->world, c, EcsConstant, 0);
        if (!underlying) {
            char *path = ecs_get_path(cursor->world, op->type);
            ecs_err("'%s' is not an enum constant for type '%s'", value, path);
            ecs_os_free(path);
            goto error;
        }

        const void *cptr = ecs_get_id(
            cursor->world, c, ecs_pair(EcsConstant, underlying));
        if (!cptr) {
            char *path = ecs_get_path(cursor->world, op->type);
            ecs_err("constant '%s' enum '%s' is of an invalid underlying type", 
                value, path);
            ecs_os_free(path);
            goto error;
        }

        ecs_value_t cv = { 
            .type = underlying, 
            .ptr = ECS_CONST_CAST(void*, cptr) 
        };
        ecs_meta_set_value(cursor, &cv);
        break;
    }
    case EcsOpBitmask:
        if (flecs_meta_parse_bitmask(cursor, op, ptr, value) != 0) {
            goto error;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = 0;
        if (flecs_meta_cursor_lookup(cursor, value, &e)) {
            goto error;
        }
        set_T(ecs_entity_t, ptr, e);
        break;
    }
    case EcsOpId: {
    #ifdef FLECS_QUERY_DSL
        ecs_id_t id = 0;
        if (flecs_id_parse(cursor->world, NULL, value, &id) == NULL) {
            goto error;
        }

        set_T(ecs_id_t, ptr, id);
    #else
        ecs_err("cannot parse component expression: script addon required");
    #endif
        break;
    }
    case EcsOpPop:
        ecs_err("excess element '%s' in scope", value);
        goto error;
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        ecs_assert(opaque != NULL, ECS_INVALID_OPERATION, 
            "entity %s is not an opaque type but serializer thinks so",
                ecs_get_name(cursor->world, op->type));
        if (opaque->assign_string) { /* preferred */
            opaque->assign_string(ptr, value);
            break;
        } else if (opaque->assign_char && value[0] && !value[1]) {
            opaque->assign_char(ptr, value[0]);
            break;
        } else if (opaque->assign_entity) {
            ecs_entity_t e = 0;
            if (flecs_meta_cursor_lookup(cursor, value, &e)) {
                goto error;
            }
            opaque->assign_entity(ptr, 
                ECS_CONST_CAST(ecs_world_t*, cursor->world), e);
            break;
        }
    }
    /* fall through */
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpForward:
    case EcsOpPrimitive:
    case EcsOpScope:
        ecs_err("unsupported conversion from string '%s' to '%s'",
            value, flecs_meta_op_kind_str(op->kind));
        goto error;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_string_literal(
    ecs_meta_cursor_t *cursor,
    const char *value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    if (!value) {
        return -1;
    }

    ecs_size_t len = ecs_os_strlen(value);
    if (value[0] != '\"' || value[len - 1] != '\"') {
        ecs_err("invalid string literal '%s'", value);
        goto error;
    }

    switch(op->kind) {
    case EcsOpChar:
        set_T(ecs_char_t, ptr, value[1]);
        break;

    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpString:
    case EcsOpEntity:
    case EcsOpId:
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
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    switch(op->kind) {
    case EcsOpEntity:
        set_T(ecs_entity_t, ptr, value);
        break;
    case EcsOpId:
        set_T(ecs_id_t, ptr, value); /* entities are valid ids */
        break;
    case EcsOpString: {
        char *result = ecs_get_path(cursor->world, value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }   
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        if (opaque && opaque->assign_entity) {
            opaque->assign_entity(ptr, 
                ECS_CONST_CAST(ecs_world_t*, cursor->world), value);
            break;
        }
    }
    /* fall through */
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpChar:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
        flecs_meta_conversion_error(cursor, op, "entity");
        goto error;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_id(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);

    switch(op->kind) {
    case EcsOpId:
        set_T(ecs_id_t, ptr, value);
        break;
    case EcsOpString: {
        char *result = ecs_id_str(cursor->world, value);
        ecs_os_free(*(ecs_string_t*)ptr);
        set_T(ecs_string_t, ptr, result);
        break;
    }
    case EcsOpOpaqueValue: {
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        if (opaque && opaque->assign_id) {
            opaque->assign_id(ptr, 
                ECS_CONST_CAST(ecs_world_t*, cursor->world), value);
            break;
        }
    }
    /* fall through */
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpChar:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpEntity:
        flecs_meta_conversion_error(cursor, op, "id");
        goto error;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

    return 0;
error:
    return -1;
}

int ecs_meta_set_null(
    ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch (op->kind) {
    case EcsOpString:
        ecs_os_free(*(char**)ptr);
        set_T(ecs_string_t, ptr, NULL);
        break;
    case EcsOpOpaqueValue: {
        const EcsOpaque *ot = ecs_get(cursor->world, op->type, EcsOpaque);
        if (ot && ot->assign_null) {
            ot->assign_null(ptr);
            break;
        }
    }
    /* fall through */
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpChar:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpEntity:
    case EcsOpId:
        flecs_meta_conversion_error(cursor, op, "null");
        goto error;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

    return 0;
error:
    return -1;
}

bool ecs_meta_get_bool(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
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
    case EcsOpF32:  return ECS_NEQZERO(*(ecs_f32_t*)ptr);
    case EcsOpF64:  return ECS_NEQZERO(*(ecs_f64_t*)ptr);
    case EcsOpString: return *(const char**)ptr != NULL;
    case EcsOpEnum: return *(ecs_i32_t*)ptr != 0;
    case EcsOpBitmask: return *(ecs_u32_t*)ptr != 0;
    case EcsOpEntity: return *(ecs_entity_t*)ptr != 0;
    case EcsOpId: return *(ecs_id_t*)ptr != 0;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpPrimitive:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for bool");
        break;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }

error:
    return 0;
}

char ecs_meta_get_char(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpChar: 
        return *(ecs_char_t*)ptr != 0;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpBool:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpString:
    case EcsOpEntity:
    case EcsOpId:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for char");
        break;
    }

error:
    return 0;
}

int64_t ecs_meta_get_int(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpBool: return *(const ecs_bool_t*)ptr;
    case EcsOpI8:   return *(const ecs_i8_t*)ptr;
    case EcsOpU8:   return *(const ecs_u8_t*)ptr;
    case EcsOpChar: return *(const ecs_char_t*)ptr;
    case EcsOpByte: return *(const ecs_u8_t*)ptr;
    case EcsOpI16:  return *(const ecs_i16_t*)ptr;
    case EcsOpU16:  return *(const ecs_u16_t*)ptr;
    case EcsOpI32:  return *(const ecs_i32_t*)ptr;
    case EcsOpU32:  return *(const ecs_u32_t*)ptr;
    case EcsOpI64:  return *(const ecs_i64_t*)ptr;
    case EcsOpU64:  return flecs_uto(int64_t, *(const ecs_u64_t*)ptr);
    case EcsOpIPtr: return *(const ecs_iptr_t*)ptr;
    case EcsOpUPtr: return flecs_uto(int64_t, *(const ecs_uptr_t*)ptr);
    case EcsOpF32:  return (int64_t)*(const ecs_f32_t*)ptr;
    case EcsOpF64:  return (int64_t)*(const ecs_f64_t*)ptr;
    case EcsOpString: return atoi(*(const char**)ptr);
    case EcsOpEnum: return *(const ecs_i32_t*)ptr;
    case EcsOpBitmask: return *(const ecs_u32_t*)ptr;
    case EcsOpEntity:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from entity to int");
        break;
    case EcsOpId:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from id to int");
        break;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpPrimitive:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for int");
        break;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }
error:
    return 0;
}

uint64_t ecs_meta_get_uint(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpBool: return *(ecs_bool_t*)ptr;
    case EcsOpI8:   return flecs_ito(uint64_t, *(const ecs_i8_t*)ptr);
    case EcsOpU8:   return *(ecs_u8_t*)ptr;
    case EcsOpChar: return flecs_ito(uint64_t, *(const ecs_char_t*)ptr);
    case EcsOpByte: return flecs_ito(uint64_t, *(const ecs_u8_t*)ptr);
    case EcsOpI16:  return flecs_ito(uint64_t, *(const ecs_i16_t*)ptr);
    case EcsOpU16:  return *(ecs_u16_t*)ptr;
    case EcsOpI32:  return flecs_ito(uint64_t, *(const ecs_i32_t*)ptr);
    case EcsOpU32:  return *(ecs_u32_t*)ptr;
    case EcsOpI64:  return flecs_ito(uint64_t, *(const ecs_i64_t*)ptr);
    case EcsOpU64:  return *(ecs_u64_t*)ptr;
    case EcsOpIPtr: return flecs_ito(uint64_t, *(const ecs_i64_t*)ptr);
    case EcsOpUPtr: return *(ecs_uptr_t*)ptr;
    case EcsOpF32:  return flecs_ito(uint64_t, *(const ecs_f32_t*)ptr);
    case EcsOpF64:  return flecs_ito(uint64_t, *(const ecs_f64_t*)ptr);
    case EcsOpString: return flecs_ito(uint64_t, atoi(*(const char**)ptr));
    case EcsOpEnum: return flecs_ito(uint64_t, *(const ecs_i32_t*)ptr);
    case EcsOpBitmask: return *(const ecs_u32_t*)ptr;
    case EcsOpEntity: return *(const ecs_entity_t*)ptr;
    case EcsOpId: return *(const ecs_id_t*)ptr;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpPrimitive:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for uint");
        break;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }
error:
    return 0;
}

static
double flecs_meta_to_float(
    ecs_meta_op_kind_t kind,
    const void *ptr)
{
    switch(kind) {
    case EcsOpBool: return *(const ecs_bool_t*)ptr;
    case EcsOpI8:   return *(const ecs_i8_t*)ptr;
    case EcsOpU8:   return *(const ecs_u8_t*)ptr;
    case EcsOpChar: return *(const ecs_char_t*)ptr;
    case EcsOpByte: return *(const ecs_u8_t*)ptr;
    case EcsOpI16:  return *(const ecs_i16_t*)ptr;
    case EcsOpU16:  return *(const ecs_u16_t*)ptr;
    case EcsOpI32:  return *(const ecs_i32_t*)ptr;
    case EcsOpU32:  return *(const ecs_u32_t*)ptr;
    case EcsOpI64:  return (double)*(const ecs_i64_t*)ptr;
    case EcsOpU64:  return (double)*(const ecs_u64_t*)ptr;
    case EcsOpIPtr: return (double)*(const ecs_iptr_t*)ptr;
    case EcsOpUPtr: return (double)*(const ecs_uptr_t*)ptr;
    case EcsOpF32:  return (double)*(const ecs_f32_t*)ptr;
    case EcsOpF64:  return *(const ecs_f64_t*)ptr;
    case EcsOpString: return atof(*ECS_CONST_CAST(const char**, ptr));
    case EcsOpEnum: return *(const ecs_i32_t*)ptr;
    case EcsOpBitmask: return *(const ecs_u32_t*)ptr;
    case EcsOpEntity:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from entity to float");
        break;
    case EcsOpId:
        ecs_throw(ECS_INVALID_PARAMETER,
            "invalid conversion from id to float");
        break;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpPrimitive:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for float");
        break;
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        break;
    }
error:
    return 0;
}

double ecs_meta_get_float(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    return flecs_meta_to_float(op->kind, ptr);
}

/* Handler to get string from opaque (see ecs_meta_get_string below) */
static int ecs_meta_get_string_value_from_opaque(
    const struct ecs_serializer_t *ser, ecs_entity_t type, const void *value)
{
    if(type != ecs_id(ecs_string_t)) {
         ecs_err("Expected value call for opaque type to be a string");
         return -1;
    }
    char*** ctx = (char ***) ser->ctx;
    *ctx = ECS_CONST_CAST(char**, value);
    return 0;
}

/* Handler to get string from opaque (see ecs_meta_get_string below) */
static int ecs_meta_get_string_member_from_opaque(
    const struct ecs_serializer_t* ser, const char* name)
{
    (void)ser;  // silence unused warning
    (void)name; // silence unused warning
    ecs_err("Unexpected member call when serializing string from opaque");
    return -1;
}

const char* ecs_meta_get_string(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpString: return *(const char**)ptr;
    case EcsOpOpaqueValue: {
        /* If opaque type happens to map to a string, retrieve it. 
         Otherwise, fallback to default case (error). */
        const EcsOpaque *opaque = ecs_get(cursor->world, op->type, EcsOpaque);
        if(opaque && opaque->as_type == ecs_id(ecs_string_t) && opaque->serialize) {
            char** str = NULL;
            ecs_serializer_t ser = {
                .world = cursor->world,
                .value = ecs_meta_get_string_value_from_opaque,
                .member = ecs_meta_get_string_member_from_opaque,
                .ctx = &str
            };
            opaque->serialize(&ser, ptr);
            if(str && *str)
                return *str;
            /* invalid string, so fall through */
        }
        /* Not a compatible opaque type, so fall through */
    }
    /* fall through */
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpChar:
    case EcsOpBool:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpEntity:
    case EcsOpId:
    default:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for string");
        break;
    }
error:
    return 0;
}

ecs_entity_t ecs_meta_get_entity(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpEntity: return *(ecs_entity_t*)ptr;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpChar:
    case EcsOpBool:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpString:
    case EcsOpId:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for entity");
        break;
    }
error:
    return 0;
}

ecs_entity_t ecs_meta_get_id(
    const ecs_meta_cursor_t *cursor)
{
    ecs_meta_scope_t *scope = flecs_cursor_get_scope(cursor);
    ecs_meta_op_t *op = flecs_cursor_get_op(scope);
    void *ptr = flecs_meta_cursor_get_ptr(cursor->world, cursor, scope);
    switch(op->kind) {
    case EcsOpEntity: return *(ecs_id_t*)ptr; /* Entities are valid ids */
    case EcsOpId: return *(ecs_id_t*)ptr;
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPop:
    case EcsOpForward:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpScope:
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpPrimitive:
    case EcsOpChar:
    case EcsOpBool:
    case EcsOpByte:
    case EcsOpU8:
    case EcsOpU16:
    case EcsOpU32:
    case EcsOpU64:
    case EcsOpI8:
    case EcsOpI16:
    case EcsOpI32:
    case EcsOpI64:
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpString:
        ecs_throw(ECS_INVALID_PARAMETER, "invalid element for entity");
        break;
    }
error:
    return 0;
}

double ecs_meta_ptr_to_float(
    ecs_primitive_kind_t type_kind,
    const void *ptr)
{
    ecs_meta_op_kind_t kind = flecs_meta_primitive_to_op_kind(type_kind);
    return flecs_meta_to_float(kind, ptr);
}

#endif
