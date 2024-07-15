/**
 * @file addons/json/serialize_value.c
 * @brief Serialize value to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

static
int flecs_json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str,
    int32_t in_array);

static
int flecs_json_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

/* Serialize enumeration */
static
int flecs_json_ser_enum(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
    ecs_check(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t value = *(const int32_t*)base;
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    ecs_enum_constant_t *constant = ecs_map_get_deref(&enum_type->constants,
        ecs_enum_constant_t, (ecs_map_key_t)value);
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
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    uint32_t value = *(const uint32_t*)ptr;
    if (!value) {
        ecs_strbuf_appendch(str, '0');
        return 0;
    }

    ecs_strbuf_list_push(str, "\"", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
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

/* Serialize elements of a contiguous array */
static
int flecs_json_ser_elements(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops, 
    int32_t op_count,
    const void *base, 
    int32_t elem_count, 
    int32_t elem_size,
    ecs_strbuf_t *str,
    bool is_array)
{
    flecs_json_array_push(str);

    const void *ptr = base;

    int i;
    for (i = 0; i < elem_count; i ++) {
        ecs_strbuf_list_next(str);
        if (flecs_json_ser_type_ops(world, ops, op_count, ptr, str, is_array)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    flecs_json_array_pop(str);

    return 0;
}

static
int flecs_json_ser_type_elements(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void *base, 
    int32_t elem_count, 
    ecs_strbuf_t *str,
    bool is_array)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
    int32_t op_count = ecs_vec_count(&ser->ops);

    return flecs_json_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, str, is_array);
}

/* Serialize array */
static
int json_ser_array(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsArray *a = ecs_get(world, op->type, EcsArray);
    ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_json_ser_type_elements(
        world, a->type, ptr, a->count, str, true);
}

/* Serialize vector */
static
int json_ser_vector(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const ecs_vec_t *value = base;
    const EcsVector *v = ecs_get(world, op->type, EcsVector);
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vec_count(value);
    void *array = ecs_vec_first(value);

    /* Serialize contiguous buffer of vector */
    return flecs_json_ser_type_elements(world, v->type, array, count, str, false);
}

typedef struct json_serializer_ctx_t {
    ecs_strbuf_t *str;
    bool is_collection;
    bool is_struct;
} json_serializer_ctx_t;

static
int json_ser_custom_value(
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
int json_ser_custom_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    json_serializer_ctx_t *json_ser = ser->ctx;
    if (!json_ser->is_struct) {
        ecs_err("serializer::member can only be called for structs");
        return -1;
    }
    flecs_json_member(json_ser->str, name);
    return 0;
}

static
int json_ser_custom_type(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str)
{
    const EcsOpaque *ct = ecs_get(world, op->type, EcsOpaque);
    ecs_assert(ct != NULL, ECS_INVALID_OPERATION, 
        "entity %s in opaque type serializer instruction is not an opaque type",
            ecs_get_name(world, op->type));
    ecs_assert(ct->as_type != 0, ECS_INVALID_OPERATION, 
        "opaque type %s has not populated as_type field",
            ecs_get_name(world, op->type));
    ecs_assert(ct->serialize != NULL, ECS_INVALID_OPERATION,
        "opaque type %s does not have serialize interface", 
            ecs_get_name(world, op->type));

    const EcsType *pt = ecs_get(world, ct->as_type, EcsType);
    ecs_assert(pt != NULL, ECS_INVALID_OPERATION, 
        "opaque type %s is missing flecs.meta.Type component",
            ecs_get_name(world, op->type));

    ecs_type_kind_t kind = pt->kind;
    bool is_collection = false;
    bool is_struct = false;

    if (kind == EcsStructType) {
        flecs_json_object_push(str);
        is_struct = true;
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        flecs_json_array_push(str);
        is_collection = true;
    }

    json_serializer_ctx_t json_ser = {
        .str = str,
        .is_struct = is_struct,
        .is_collection = is_collection
    };

    ecs_serializer_t ser = {
        .world = world,
        .value = json_ser_custom_value,
        .member = json_ser_custom_member,
        .ctx = &json_ser
    };

    if (ct->serialize(&ser, base)) {
        return -1;
    }

    if (kind == EcsStructType) {
        flecs_json_object_pop(str);
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        flecs_json_array_pop(str);
    }

    return 0;
}

/* Forward serialization to the different type kinds */
static
int flecs_json_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr,
    ecs_strbuf_t *str) 
{
    void *vptr = ECS_OFFSET(ptr, op->offset);
    bool large_int = false;
    if (op->kind == EcsOpI64) {
        if (*(int64_t*)vptr >= 2147483648) {
            large_int = true;
        }
    } else if (op->kind == EcsOpU64) {
        if (*(uint64_t*)vptr >= 2147483648) {
            large_int = true;
        }
    }

    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }

    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpF32:
        ecs_strbuf_appendflt(str, 
            (ecs_f64_t)*(const ecs_f32_t*)vptr, '"');
        break;
    case EcsOpF64:
        ecs_strbuf_appendflt(str, 
            *(ecs_f64_t*)vptr, '"');
        break;
    case EcsOpEnum:
        if (flecs_json_ser_enum(world, op, vptr, str)) {
            goto error;
        }
        break;
    case EcsOpBitmask:
        if (flecs_json_ser_bitmask(world, op, vptr, str)) {
            goto error;
        }
        break;
    case EcsOpArray:
        if (json_ser_array(world, op, vptr, str)) {
            goto error;
        }
        break;
    case EcsOpVector:
        if (json_ser_vector(world, op, vptr, str)) {
            goto error;
        }
        break;
    case EcsOpOpaque:
        if (json_ser_custom_type(world, op, vptr, str)) {
            goto error;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = *(const ecs_entity_t*)vptr;
        if (!e) {
            ecs_strbuf_appendlit(str, "\"#0\"");
        } else {
            flecs_json_path(str, world, e);
        }
        break;
    }
    case EcsOpId: {
        ecs_id_t id = *(const ecs_id_t*)vptr;
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
            flecs_json_op_to_primitive_kind(op->kind), 
            ECS_OFFSET(ptr, op->offset), str, true)) 
        {
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
        }
        break;

    case EcsOpPrimitive:
    case EcsOpScope:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }

    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }

    return 0;
error:
    return -1;
}

/* Iterate over a slice of the type ops array */
static
int flecs_json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str,
    int32_t in_array)
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (in_array <= 0) {
            if (op->name) {
                flecs_json_member(str, op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1) {
                /* Serialize inline array */
                if (flecs_json_ser_elements(world, op, op->op_count, base,
                    elem_count, op->size, str, true))
                {
                    return -1;
                }

                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            flecs_json_object_push(str);
            in_array --;
            break;
        case EcsOpPop:
            flecs_json_object_pop(str);
            in_array ++;
            break;
        case EcsOpArray:
        case EcsOpVector:
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
        case EcsOpString:
        case EcsOpOpaque:
            if (flecs_json_ser_type_op(world, op, base, str)) {
                goto error;
            }
            break;
        default:
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
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
    ecs_meta_type_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return flecs_json_ser_type_ops(world, ops, count, base, str, 0);
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
