/**
 * @file addons/serialize/serialize.c
 * @brief Serialize values.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "serialize.h"

static
int flecs_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_visitor_desc_t *visitor_desc,
    int32_t in_array);

static
int flecs_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_visitor_desc_t *visitor_desc);

extern
int flecs_ser_custom_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value);

static
int flecs_ser_custom_member(
    const ecs_serializer_t *ser,
    const char *name);

static
int flecs_ser_custom_type(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_visitor_desc_t *visitor_desc);


/* Serialize enumeration */
static
int flecs_ser_enum(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    ecs_visitor_desc_t *visitor_desc,
    const void *base) 
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

    visitor_desc->visit_enum(value, ecs_get_name(world, constant->constant), visitor_desc->user_data);

    return 0;
error:
    return -1;
}

static
int flecs_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    ecs_visitor_desc_t *visitor_desc,
    const void *ptr
    ) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    uint32_t value = *(const uint32_t*)ptr;

    if (visitor_desc->visit_bitmask.enter && !visitor_desc->visit_bitmask.enter(value, visitor_desc->user_data)) {
        // Visit Enter Bitmask can decide to skip visiting by returning 0
        return 0;
    }

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
    uint32_t visited = 0;
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *constant = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);
        if ((value & key) == key) {
            if (visitor_desc && visitor_desc->visit_bitmask.value) {
                visitor_desc->visit_bitmask.value(
                    (uint32_t)key,
                    ecs_get_name(world, constant->constant),
                    visitor_desc->user_data
                );
            }
            ++visited;
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

    if (visitor_desc->visit_bitmask.exit) {
        visitor_desc->visit_bitmask.exit(visited, visitor_desc->user_data);
    }

    return 0;
error:
    return -1;
}


/* Serialize elements of a contiguous array */
static
int flecs_ser_elements(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops, 
    int32_t op_count,
    const void *base, 
    int32_t elem_count, 
    int32_t elem_size,
    ecs_visitor_desc_t *visitor_desc,
    bool is_array)
{
    if (visitor_desc->visit_array.enter)
        visitor_desc->visit_array.enter((size_t)elem_count, visitor_desc->user_data);

    const void *ptr = base;

    size_t i;
    for (i = 0; i < (size_t)elem_count; i ++) {
        if (visitor_desc->visit_array.next_value)
            visitor_desc->visit_array.next_value(i, visitor_desc->user_data);
        if (flecs_ser_type_ops(world, ops, op_count, ptr, visitor_desc, is_array)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    if (visitor_desc->visit_array.exit)
        visitor_desc->visit_array.exit(visitor_desc->user_data);

    return 0;
}

static
int flecs_ser_type_elements(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void *base, 
    int32_t elem_count, 
    ecs_visitor_desc_t *visitor_desc,
    bool is_array)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
    int32_t op_count = ecs_vec_count(&ser->ops);

    return flecs_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, visitor_desc, is_array);
}


/* Serialize array */
static
int flecs_ser_array(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_visitor_desc_t *visitor_desc) 
{
    const EcsArray *a = ecs_get(world, op->type, EcsArray);
    ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_ser_type_elements(
        world, a->type, ptr, a->count, visitor_desc, true);
}

/* Serialize vector */
static
int flecs_ser_vector(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_visitor_desc_t *visitor_desc) 
{
    const ecs_vec_t *value = base;
    const EcsVector *v = ecs_get(world, op->type, EcsVector);
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vec_count(value);
    void *array = ecs_vec_first(value);

    /* Serialize contiguous buffer of vector */
    return flecs_ser_type_elements(world, v->type, array, count, visitor_desc, false);
}

typedef struct flecs_custom_type_serializer_ctx_t {
    ecs_visitor_desc_t *visitor_desc;
    bool is_collection;
    bool is_struct;
    size_t current_index; // Used for array/vector
} flecs_custom_type_serializer_ctx_t;

extern
int flecs_ser_custom_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value)
{
    flecs_custom_type_serializer_ctx_t *ser_ctx = ser->ctx;
    ecs_visitor_desc_t* visitor_desc = ser_ctx->visitor_desc;
    if (ser_ctx->is_collection && visitor_desc->visit_array.next_value) {
        visitor_desc->visit_array.next_value(ser_ctx->current_index++, visitor_desc->user_data);
    }
    return ecs_ser_ptr(ser->world, type, value, visitor_desc);
}

int flecs_ser_custom_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    flecs_custom_type_serializer_ctx_t *ser_ctx = ser->ctx;
    ecs_visitor_desc_t* visitor_desc = ser_ctx->visitor_desc;
    if (!ser_ctx->is_struct) {
        ecs_err("serializer::member can only be called for structs");
        return -1;
    }

    if (visitor_desc->visit_member)
        visitor_desc->visit_member(name, visitor_desc->user_data);
    return 0;
}

static
int flecs_ser_custom_type(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_visitor_desc_t *visitor_desc)
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

    flecs_custom_type_serializer_ctx_t ser_ctx = {
        .visitor_desc = visitor_desc,
        .is_struct = false,
        .is_collection = false,
        .current_index = 0,
    };

    if (kind == EcsStructType) {
        if (visitor_desc->visit_struct.enter) {
            visitor_desc->visit_struct.enter(visitor_desc->user_data);
        } 
        ser_ctx.is_struct = true;
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        // If a user doesn't supply a count fn,
        // they will have to figure it out themselves.
        size_t count = ct->count? ct->count(base): 0u;
        if (visitor_desc->visit_array.enter) {
            visitor_desc->visit_array.enter(count, visitor_desc->user_data);
        } 
        ser_ctx.is_collection = true;
    }

    ecs_serializer_t ser = {
        .world = world,
        .value = flecs_ser_custom_value,
        .member = flecs_ser_custom_member,
        .ctx = &ser_ctx
    };

    if (ct->serialize(&ser, base)) {
        return -1;
    }

    if (kind == EcsStructType) {
        if (visitor_desc->visit_struct.exit) {
            visitor_desc->visit_struct.exit(visitor_desc->user_data);
        } 
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        if (visitor_desc->visit_array.exit) {
            visitor_desc->visit_array.exit(visitor_desc->user_data);
        } 
    }

    return 0;
}


/* Serialize a primitive value */
static
int flecs_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    ecs_visitor_desc_t *visitor_desc,
    const void *base) 
{
    switch(kind) {
    case EcsBool:
        if (visitor_desc->visit_bool)
            visitor_desc->visit_bool(*(const bool*)base, visitor_desc->user_data);
        break;
    case EcsChar:
        if (visitor_desc->visit_char)
            visitor_desc->visit_char(*(const char*)base, visitor_desc->user_data);
        break;
    case EcsByte:
        if (visitor_desc->visit_byte)
            visitor_desc->visit_byte(*(const uint8_t*)base, visitor_desc->user_data);
        break;
    case EcsU8:
        if (visitor_desc->visit_u8)
            visitor_desc->visit_u8(*(const uint8_t*)base, visitor_desc->user_data);
        break;
    case EcsU16:
        if (visitor_desc->visit_u16)
            visitor_desc->visit_u16(*(const uint16_t*)base, visitor_desc->user_data);
        break;
    case EcsU32:
        if (visitor_desc->visit_u32)
            visitor_desc->visit_u32(*(const uint32_t*)base, visitor_desc->user_data);
        break;
    case EcsU64:
        if (visitor_desc->visit_u64)
            visitor_desc->visit_u64(*(const uint64_t*)base, visitor_desc->user_data);
        break;
    case EcsI8:
        if (visitor_desc->visit_i8)
            visitor_desc->visit_i8(*(const int8_t*)base, visitor_desc->user_data);
        break;
    case EcsI16:
        if (visitor_desc->visit_i16)
            visitor_desc->visit_i16(*(const int16_t*)base, visitor_desc->user_data);
        break;
    case EcsI32:
        if (visitor_desc->visit_i32)
            visitor_desc->visit_i32(*(const int32_t*)base, visitor_desc->user_data);
        break;
    case EcsI64:
        if (visitor_desc->visit_i64)
            visitor_desc->visit_i64(*(const int64_t*)base, visitor_desc->user_data);
        break;
    case EcsF32:
        if (visitor_desc->visit_f32)
            visitor_desc->visit_f32(*(const float*)base, visitor_desc->user_data);
        break;
    case EcsF64:
        if (visitor_desc->visit_f64)
            visitor_desc->visit_f64(*(const double*)base, visitor_desc->user_data);
        break;
    case EcsIPtr:
        if (visitor_desc->visit_iptr)
            visitor_desc->visit_iptr(*(const intptr_t*)base, visitor_desc->user_data);
        break;
    case EcsUPtr:
        if (visitor_desc->visit_uptr)
            visitor_desc->visit_uptr(*(const uintptr_t*)base, visitor_desc->user_data);
        break;
    case EcsString:
        if (visitor_desc->visit_string)
            visitor_desc->visit_string(*ECS_CONST_CAST(const char**, base), visitor_desc->user_data);
        break;
    case EcsEntity:
        if (visitor_desc->visit_entity)
            visitor_desc->visit_entity(world, *(const ecs_entity_t*)base, visitor_desc->user_data);
        break;
    case EcsId:
        if (visitor_desc->visit_id)
            visitor_desc->visit_id(world, *(const ecs_id_t*)base, visitor_desc->user_data);
        break;
    default:
        ecs_err("invalid primitive kind");
        return -1;
    }

    return 0;
}

/* Forward serialization to the different type kinds */
static
int flecs_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr,
    ecs_visitor_desc_t *visitor_desc) 
{

    void *vptr = ECS_OFFSET(ptr, op->offset);

    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpEnum:
        if (visitor_desc->visit_enum && flecs_ser_enum(world, op, visitor_desc, vptr)) {
            goto error;
        }
        break;
    case EcsOpBitmask:
        if ((visitor_desc->visit_bitmask.enter || visitor_desc->visit_bitmask.value)
            && flecs_ser_bitmask(world, op, visitor_desc, vptr)) {
            goto error;
        }
        break;
    case EcsOpArray:
        if (flecs_ser_array(world, op, vptr, visitor_desc)) {
            goto error;
        }
        break;
    case EcsOpVector:
        if (flecs_ser_vector(world, op, vptr, visitor_desc)) {
            goto error;
        }
        break;
    case EcsOpOpaque:
        if (flecs_ser_custom_type(world, op, vptr, visitor_desc)) {
            goto error;
        }
        break;
        /*
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
    */
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
    case EcsOpF32:
    case EcsOpF64:
    case EcsOpUPtr:
    case EcsOpIPtr:
    case EcsOpString:
    case EcsOpEntity:
    case EcsOpId:
        if (flecs_ser_primitive(world, 
            flecs_op_to_primitive_kind(op->kind), 
            visitor_desc,
            ECS_OFFSET(ptr, op->offset))) 
        {
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
        }
        break;
    case EcsOpPrimitive:
    case EcsOpScope:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }

    return 0;
error:
    return -1;
}

/* Iterate over a slice of the type ops array */
static
int flecs_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_visitor_desc_t *visitor_desc,
    int32_t in_array)
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (in_array <= 0) {
            if (op->name && visitor_desc->visit_member) {
                visitor_desc->visit_member(op->name, visitor_desc->user_data);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1) {
                /* Serialize inline array */
                if (flecs_ser_elements(world, op, op->op_count, base,
                    elem_count, op->size, visitor_desc, true))
                {
                    return -1;
                }

                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            if (visitor_desc->visit_struct.enter)
                visitor_desc->visit_struct.enter(visitor_desc->user_data);
            in_array --;
            break;
        case EcsOpPop:
            if (visitor_desc->visit_struct.exit)
                visitor_desc->visit_struct.exit(visitor_desc->user_data);
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
            if (flecs_ser_type_op(world, op, base, visitor_desc)) {
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
int flecs_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *v_ops,
    const void *base, 
    ecs_visitor_desc_t *visitor_desc) 
{
    ecs_meta_type_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return flecs_ser_type_ops(world, ops, count, base, visitor_desc, 0);
}

int flecs_ser_array_w_type_data(
    const ecs_world_t *world,
    const void *ptr,
    int32_t count,
    ecs_visitor_desc_t *visitor_desc,
    const EcsComponent *comp,
    const EcsTypeSerializer *ser)
{
    if (count) {
        ecs_size_t size = comp->size;

        if (visitor_desc->visit_array.enter) {
            visitor_desc->visit_array.enter((size_t)count, visitor_desc->user_data);
        }

        do {
            if (visitor_desc->visit_array.next_value) {
                visitor_desc->visit_array.next_value((size_t)count, visitor_desc->user_data);
            }
            if (flecs_ser_type(world, &ser->ops, ptr, visitor_desc)) {
                return -1;
            }

            ptr = ECS_OFFSET(ptr, size);
        } while (-- count);

        if (visitor_desc->visit_array.exit) {
            visitor_desc->visit_array.exit(visitor_desc->user_data);
        }
    } else {
        if (flecs_ser_type(world, &ser->ops, ptr, visitor_desc)) {
            return -1;
        }
    }

    return 0;
}

ecs_primitive_kind_t flecs_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind) 
{
    return kind - EcsOpPrimitive;
}

int ecs_ser_array(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_visitor_desc_t *visitor_desc)
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

    return flecs_ser_array_w_type_data(world, ptr, count, visitor_desc, comp, ser);
}

/*
void* ecs_ser_ptr(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void* ptr,
    ecs_visitor_desc_t *visitor_desc)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        visitor_desc->_error = true;
        goto error;
    }

    if (flecs_ser_type(world, &ser->ops, ptr, &visitor_desc)) {
        visitor_desc->_error = true;
        goto error;
    }

    if (visitor_desc->exit)
        return visitor_desc->exit(visitor_desc->user_data);
    return visitor_desc->user_data;
error:
    if (visitor_desc->error)
        return visitor_desc->error(visitor_desc->user_data);
    return NULL;
}
*/

int ecs_ser_ptr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_visitor_desc_t *visitor_desc)
{
    return ecs_ser_array(world, type, ptr, 0, visitor_desc);
}

#endif
