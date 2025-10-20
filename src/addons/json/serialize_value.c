/**
 * @file addons/json/serialize_value.c
 * @brief Serialize value to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

static
int flecs_json_ser_type_slice(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str);

/* Serialize enumeration */
static
int flecs_json_ser_enum(
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
    ecs_enum_constant_t *constant = ecs_map_get_deref(op->is.constants,
        ecs_enum_constant_t, value);
    if (!constant) {
        /* If the value is not found, it is not a valid enumeration constant */
        char *name = ecs_get_path(world, op->type);
        ecs_err("enumeration value '%d' of type '%s' is not a valid constant", 
            value, name);
        ecs_os_free(name);
        goto error;
    }

    ecs_strbuf_appendch(str, '"');
    ecs_strbuf_appendstr(str, ecs_get_name(world, constant->constant));
    ecs_strbuf_appendch(str, '"');

    return 0;
error:
    return -1;
}

/* Serialize bitmask */
static
int flecs_json_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    uint32_t value = *(const uint32_t*)ptr;
    if (!value) {
        ecs_strbuf_appendch(str, '0');
        return 0;
    }

    ecs_strbuf_list_push(str, "\"", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(op->is.constants);
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *constant = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, 
                ecs_get_name(world, constant->constant));
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        /* All bits must have been matched by a constant */
        char *name = ecs_get_path(world, op->type);
        ecs_err("bitmask value '%u' of type '%s' contains invalid/unknown bits", 
            value, name);
        ecs_os_free(name);
        goto error;
    }

    ecs_strbuf_list_pop(str, "\"");

    return 0;
error:
    return -1;
}

typedef struct json_serializer_ctx_t {
    ecs_strbuf_t *str;
    bool is_collection;
} json_serializer_ctx_t;

static
int flecs_json_ser_opaque_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value)
{
    json_serializer_ctx_t *json_ser = ser->ctx;
    if (json_ser->is_collection) {
        ecs_strbuf_list_next(json_ser->str);
    }
    return ecs_ptr_to_json_buf(ser->world, type, value, json_ser->str);
}

static
int flecs_json_ser_opaque_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    json_serializer_ctx_t *json_ser = ser->ctx;
    flecs_json_member(json_ser->str, name);
    return 0;
}

static
int flecs_json_ser_opaque(
    const ecs_world_t *world,
    ecs_meta_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str,
    ecs_meta_op_kind_t kind)
{
    bool is_struct = kind == EcsOpOpaqueStruct;
    bool is_collection = kind == EcsOpOpaqueVector || kind == EcsOpOpaqueArray;

    if (is_struct) {
        flecs_json_object_push(str);
    } else if (is_collection) {
        flecs_json_array_push(str);
    }

    json_serializer_ctx_t json_ser = { 
        .str = str, .is_collection = is_collection
    };

    ecs_serializer_t ser = {
        .world = world,
        .value = flecs_json_ser_opaque_value,
        .member = is_struct ? flecs_json_ser_opaque_member : NULL,
        .ctx = &json_ser
    };

    ecs_assert(op->is.opaque != NULL, ECS_INTERNAL_ERROR, NULL);
    if (op->is.opaque(&ser, base)) {
        return -1;
    }

    if (is_struct) {
        flecs_json_object_pop(str);
    } else if (is_collection) {
        flecs_json_array_pop(str);
    }

    return 0;
}

static
int flecs_json_ser_scope(
    const ecs_world_t *world,
    ecs_meta_op_t *op,
    const void *base,
    ecs_strbuf_t *str)
{
    if (flecs_json_ser_type_slice(world, &op[1], op->op_count - 2, base, str)) {
        return -1;
    }

    return 0;
}

static
int flecs_json_ser_array(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *array,
    int32_t elem_size,
    int32_t count,
    ecs_strbuf_t *str)
{
    flecs_json_array_push(str);

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(str);
        void *ptr = ECS_ELEM(array, elem_size, i);
        if (flecs_json_ser_scope(world, ops, ptr, str)) {
            return -1;
        }
    }

    flecs_json_array_pop(str);
    return 0;
}

static
int flecs_json_ser_forward(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *base,
    ecs_strbuf_t *str)
{
    const EcsTypeSerializer *ts = ecs_get(world, type, EcsTypeSerializer);
    if (!ts) {
        ecs_err("missing type serializer for '%s'", 
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }

    return flecs_json_ser_type_slice(world, ecs_vec_first(&ts->ops), 
        ecs_vec_count(&ts->ops), base, str);
}

/* Iterate over a slice of the type ops array */
static
int flecs_json_ser_type_slice(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str)
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        const void *ptr = ECS_OFFSET(base, op->offset);

        if (op->name) {
            flecs_json_member(str, op->name);
        }

        bool large_int = false;
        if (op->kind == EcsOpI64) {
            if (*(const int64_t*)ptr >= 2147483648) {
                large_int = true;
            }
        } else if (op->kind == EcsOpU64) {
            if (*(const uint64_t*)ptr >= 2147483648) {
                large_int = true;
            }
        }

        if (large_int) {
            ecs_strbuf_appendch(str, '"');
        }

        switch(op->kind) {
        case EcsOpPushStruct: {
            flecs_json_object_push(str);
            if (flecs_json_ser_scope(world, op, ptr, str)) {
                return -1;
            }
            flecs_json_object_pop(str);

            i += op->op_count - 1;
            break;
        }
        case EcsOpPushVector: {
            const ecs_vec_t *vec = ptr;
            if (flecs_json_ser_array(world, op, 
                vec->array, op->elem_size, vec->count, str)) 
            {
                goto error;
            }

            i += op->op_count - 1;
            break;
        }
        case EcsOpPushArray: {
            if (flecs_json_ser_array(world, op, ptr, 
                op->elem_size, ecs_meta_op_get_elem_count(op, ptr), str)) 
            {
                goto error;
            }

            i += op->op_count - 1;
            break;
        }
        case EcsOpForward: {
            if (flecs_json_ser_forward(world, op->type, 
                ECS_OFFSET(base, op->offset), str))
            {
                goto error;
            }
            break;
        }
        case EcsOpF32:
            ecs_strbuf_appendflt(str, 
                (ecs_f64_t)*(const ecs_f32_t*)ptr, '"');
            break;
        case EcsOpF64:
            ecs_strbuf_appendflt(str, *(const ecs_f64_t*)ptr, '"');
            break;
        case EcsOpEnum:
            if (flecs_json_ser_enum(world, op, ptr, str)) {
                goto error;
            }
            break;
        case EcsOpBitmask:
            if (flecs_json_ser_bitmask(world, op, ptr, str)) {
                goto error;
            }
            break;
        case EcsOpOpaqueStruct:
        case EcsOpOpaqueArray:
        case EcsOpOpaqueVector:
        case EcsOpOpaqueValue:
            if (flecs_json_ser_opaque(world, op, ptr, str, op->kind)) {
                goto error;
            }
            break;
        case EcsOpEntity: {
            ecs_entity_t e = *(const ecs_entity_t*)ptr;
            if (!e) {
                ecs_strbuf_appendlit(str, "\"#0\"");
            } else {
                flecs_json_path(str, world, e);
            }
            break;
        }
        case EcsOpId: {
            ecs_id_t id = *(const ecs_id_t*)ptr;
            if (!id) {
                ecs_strbuf_appendlit(str, "\"#0\"");
            } else {
                flecs_json_id(str, world, id);
            }
            break;
        }
        case EcsOpU64:
        case EcsOpI64:
        case EcsOpBool:
        case EcsOpChar:
        case EcsOpByte:
        case EcsOpU8:
        case EcsOpU16:
        case EcsOpU32:
        case EcsOpI8:
        case EcsOpI16:
        case EcsOpI32:
        case EcsOpUPtr:
        case EcsOpIPtr:
        case EcsOpString:
            if (flecs_expr_ser_primitive(world, 
                flecs_json_op_to_primitive_kind(op->kind), ptr, str, true)) 
            {
                ecs_throw(ECS_INTERNAL_ERROR, NULL);
            }
            break;
        case EcsOpPrimitive:
        case EcsOpScope:
        case EcsOpPop:
        default:
            ecs_throw(ECS_INTERNAL_ERROR, 
                "unexpected serializer operation");
        }

        if (large_int) {
            ecs_strbuf_appendch(str, '"');
        }
    }

    return 0;
error:
    return -1;
}

/* Iterate over the type ops of a type */
int flecs_json_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return flecs_json_ser_type_slice(world, ops, count, base, str);
}

static
int flecs_array_to_json_buf_w_type_data(
    const ecs_world_t *world,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf,
    const EcsComponent *comp,
    const EcsTypeSerializer *ser)
{
    if (count) {
        ecs_size_t size = comp->size;

        flecs_json_array_push(buf);

        do {
            ecs_strbuf_list_next(buf);
            if (flecs_json_ser_type(world, &ser->ops, ptr, buf)) {
                return -1;
            }

            ptr = ECS_OFFSET(ptr, size);
        } while (-- count);

        flecs_json_array_pop(buf);
    } else {
        if (flecs_json_ser_type(world, &ser->ops, ptr, buf)) {
            return -1;
        }
    }

    return 0;
}

int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf)
{
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    if (!comp) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize to JSON, '%s' is not a component", path);
        ecs_os_free(path);
        return -1;
    }

    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (!ser) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
        ecs_os_free(path);
        return -1;
    }

    return flecs_array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser);
}

char* ecs_array_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr,
    int32_t count)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_array_to_json_buf(world, type, ptr, count, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf)
{
    return ecs_array_to_json_buf(world, type, ptr, 0, buf);
}

char* ecs_ptr_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    return ecs_array_to_json(world, type, ptr, 0);
}

#endif
