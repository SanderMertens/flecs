/**
 * @file addons/script/serialize.c
 * @brief Serialize values to string.
 */

#include "flecs.h"
#include "../meta/meta.h"
#include <inttypes.h>

#ifdef FLECS_SCRIPT
#include "script.h"

static
int flecs_expr_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *ser, 
    const void *base, 
    ecs_strbuf_t *str,
    bool is_expr);

static
int flecs_expr_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str,
    bool is_expr);

static
ecs_primitive_kind_t flecs_expr_op_to_primitive_kind(ecs_meta_op_kind_t kind) {
    return kind - EcsOpPrimitive;
}

/* Serialize enumeration */
static
int flecs_expr_ser_enum(
    const ecs_world_t *world,
    ecs_meta_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_map_key_t value;
    ecs_meta_op_kind_t kind = op->underlying_kind;

    if (kind == EcsOpU8 || kind == EcsOpI8) {
        value = *(const uint8_t*)base;
    } else if (kind == EcsOpU16 || kind == EcsOpI16) {
        value = *(const uint16_t*)base;
    } else if (kind == EcsOpU32 || kind == EcsOpI32) {
        value = *(const uint32_t*)base;
    } else if (kind == EcsOpUPtr || kind == EcsOpIPtr) {
        value = *(const uintptr_t*)base;
    } else if (kind == EcsOpU64 || kind == EcsOpI64) {
        value = *(const uint64_t*)base;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, "invalid underlying type");
    }
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    ecs_enum_constant_t *c = ecs_map_get_deref(op->is.constants, 
        ecs_enum_constant_t, value);
    if (!c) {
        char *path = ecs_get_path(world, op->type);
        ecs_err("value %" PRIu64 " is not valid for enum type '%s'", 
            value, path);
        ecs_os_free(path);
        goto error;
    }

    ecs_strbuf_appendstr(str, ecs_get_name(world, c->constant));

    return 0;
error:
    return -1;
}

/* Serialize bitmask */
static
int flecs_expr_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    uint32_t value = *(const uint32_t*)ptr;

    ecs_strbuf_list_push(str, "", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(op->is.constants);
    int count = 0;
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, ecs_get_name(world, c->constant));
            count ++;
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        /* All bits must have been matched by a constant */
        char *path = ecs_get_path(world, op->type);
        ecs_err(
            "value for bitmask %s contains bits (%u) that cannot be mapped to constant", 
            path, value);
        ecs_os_free(path);
        goto error;
    }

    if (!count) {
        ecs_strbuf_list_appendstr(str, "0");
    }

    ecs_strbuf_list_pop(str, "");

    return 0;
error:
    return -1;
}

static
int flecs_expr_ser_scope(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *base,
    ecs_strbuf_t *str,
    bool is_expr)
{
    if (flecs_expr_ser_type_ops(
        world, ops + 1, ops->op_count - 2, base, str, is_expr)) 
    {
        return -1;
    }
    return 0;
}

static
int flecs_expr_ser_array(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *array,
    int32_t count,
    ecs_strbuf_t *str,
    bool is_expr)
{
    ecs_strbuf_list_push(str, "[", ", ");

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(str);
        void *ptr = ECS_ELEM(array, ops->elem_size, i);
        if (flecs_expr_ser_scope(world, ops, ptr, str, is_expr)) {
            goto error;
        }
    }

    ecs_strbuf_list_pop(str, "]");

    return 0;
error:
    return -1;
}

static
int flecs_expr_ser_struct(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *base,
    ecs_strbuf_t *str,
    bool is_expr)
{
    ecs_strbuf_list_push(str, "{", ", ");

    if (flecs_expr_ser_scope(world, ops, base, str, is_expr)) {
        return -1;
    }

    ecs_strbuf_list_pop(str, "}");

    return 0;
}

static
int flecs_expr_ser_forward(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *base,
    ecs_strbuf_t *str,
    bool is_expr)
{
    const EcsTypeSerializer *ts = ecs_get(world, type, EcsTypeSerializer);
    if (!ts) {
        ecs_err("missing type serializer for '%s'", 
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }

    return flecs_expr_ser_type_ops(world, ecs_vec_first(&ts->ops), 
        ecs_vec_count(&ts->ops), base, str, is_expr);
}

typedef struct flecs_expr_serializer_ctx_t {
    ecs_strbuf_t *str;
    bool is_collection;
} flecs_expr_serializer_ctx_t;

static
int flecs_expr_ser_opaque_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value)
{
    flecs_expr_serializer_ctx_t *expr_ser = ser->ctx;
    if (expr_ser->is_collection) {
        ecs_strbuf_list_next(expr_ser->str);
    }
    return ecs_ptr_to_expr_buf(ser->world, type, value, expr_ser->str);
}

static
int flecs_expr_ser_opaque_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    flecs_expr_serializer_ctx_t *expr_ser = ser->ctx;
    ecs_strbuf_list_next(expr_ser->str);
    ecs_strbuf_append(expr_ser->str, "%s: ", name);
    return 0;
}

static
int flecs_expr_ser_opaque(
    const ecs_world_t *world,
    ecs_meta_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str,
    ecs_meta_op_kind_t kind)
{
    bool is_struct = kind == EcsOpOpaqueStruct;
    bool is_collection = kind == EcsOpOpaqueVector || kind == EcsOpOpaqueArray;

    if (is_struct) {
        ecs_strbuf_list_push(str, "{", ", ");
    } else if (is_collection) {
        ecs_strbuf_list_push(str, "[", ", ");
    }

    flecs_expr_serializer_ctx_t expr_ser = { 
        .str = str, .is_collection = is_collection
    };

    ecs_serializer_t ser = {
        .world = world,
        .value = flecs_expr_ser_opaque_value,
        .member = is_struct ? flecs_expr_ser_opaque_member : NULL,
        .ctx = &expr_ser
    };

    ecs_assert(op->is.opaque != NULL, ECS_INTERNAL_ERROR, NULL);
    if (op->is.opaque(&ser, base)) {
        return -1;
    }

    if (is_struct) {
        ecs_strbuf_list_pop(str, "}");
    } else if (is_collection) {
        ecs_strbuf_list_pop(str, "]");
    }

    return 0;
}

/* Iterate over a slice of the type ops array */
static
int flecs_expr_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str,
    bool is_expr) 
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        
        const void *ptr = ECS_OFFSET(base, op->offset);

        if (op->name) {
            ecs_strbuf_list_next(str);
            ecs_strbuf_append(str, "%s: ", op->name);
        }

        switch(op->kind) {
        case EcsOpPushStruct: {
            if (flecs_expr_ser_struct(world, op, ptr, str, is_expr)) {
                goto error;
            }
            break;
        }
        case EcsOpPushArray: {
            if (flecs_expr_ser_array(world, op, ptr, 
                ecs_meta_op_get_elem_count(op, ptr), str, is_expr))
            {
                goto error;
            }
            break;
        }
        case EcsOpPushVector: {
            ecs_vec_t *vec = ECS_OFFSET(base, op->offset);

            if (flecs_expr_ser_array(world, op, 
                vec->array, vec->count, str, is_expr))
            {
                goto error;
            }
            break;
        }
        case EcsOpForward: {
            if (flecs_expr_ser_forward(world, op->type, ptr, str, is_expr)) {
                goto error;
            }
            break;
        }
        case EcsOpOpaqueStruct:
        case EcsOpOpaqueArray:
        case EcsOpOpaqueVector:
        case EcsOpOpaqueValue:
            if (flecs_expr_ser_opaque(world, op, ptr, str, op->kind)) {
                goto error;
            }
            break;
        case EcsOpEnum:
            if (flecs_expr_ser_enum(world, op, ptr, str)) {
                goto error;
            }
            break;
        case EcsOpBitmask:
            if (flecs_expr_ser_bitmask(world, op, ptr, str)) {
                goto error;
            }
            break;
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
        case EcsOpString:
            if (flecs_expr_ser_primitive(world, 
                flecs_expr_op_to_primitive_kind(op->kind), ptr, str, is_expr))
            {
                /* Unknown operation */
                ecs_err("unknown serializer operation kind (%d)", op->kind);
                goto error;
            }
            break;
        case EcsOpScope:
        case EcsOpPrimitive:
        case EcsOpPop:
        default:
            ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
        }

        i += op->op_count - 1; // Skip over already processed instructions
    }

    return 0;
error:
    return -1;
}

/* Iterate over the type ops of a type */
static
int flecs_expr_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str,
    bool is_expr) 
{
    ecs_meta_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return flecs_expr_ser_type_ops(world, ops, count, base, str, is_expr);
}

int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }

    if (flecs_expr_ser_type(world, &ser->ops, ptr, buf_out, true)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

char* ecs_ptr_to_expr(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_ptr_to_expr_buf(world, type, ptr, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

int ecs_ptr_to_str_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }

    if (flecs_expr_ser_type(world, &ser->ops, ptr, buf_out, false)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

char* ecs_ptr_to_str(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_ptr_to_str_buf(world, type, ptr, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

#endif
