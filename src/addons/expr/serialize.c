
#include "../../private_api.h"

#ifdef FLECS_EXPR

static
int expr_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
int expr_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str);

static
int expr_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

/* Serialize a primitive value */
static
int expr_ser_primitive(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const char *bool_str[] = { "false", "true" };

    switch(op->kind) {
    case EcsOpBool:
        ecs_strbuf_appendstr(str, bool_str[(int)*(bool*)base]);
        break;
    case EcsOpChar: {
        char chbuf[3];
        char ch = *(char*)base;
        if (ch) {
            ecs_chresc(chbuf, *(char*)base, '"');
            ecs_strbuf_appendstrn(str, "\"", 1);
            ecs_strbuf_appendstr(str, chbuf);
            ecs_strbuf_appendstrn(str, "\"", 1);
        } else {
            ecs_strbuf_appendstr(str, "0");
        }
        break;
    }
    case EcsOpByte:
        ecs_strbuf_append(str, "0x%x", *(uint8_t*)base);
        break;
    case EcsOpU8:
        ecs_strbuf_append(str, "%u", *(uint8_t*)base);
        break;
    case EcsOpU16:
        ecs_strbuf_append(str, "%u", *(uint16_t*)base);
        break;
    case EcsOpU32:
        ecs_strbuf_append(str, "%u", *(uint32_t*)base);
        break;
    case EcsOpU64:
        ecs_strbuf_append(str, "%llu", *(uint64_t*)base);
        break;
    case EcsOpI8:
        ecs_strbuf_append(str, "%d", *(int8_t*)base);
        break;
    case EcsOpI16:
        ecs_strbuf_append(str, "%d", *(int16_t*)base);
        break;
    case EcsOpI32:
        ecs_strbuf_append(str, "%d", *(int32_t*)base);
        break;
    case EcsOpI64:
        ecs_strbuf_append(str, "%lld", *(int64_t*)base);
        break;
    case EcsOpF32:
        ecs_strbuf_append(str, "%f", (double)*(float*)base);
        break;
    case EcsOpF64:
        ecs_strbuf_append(str, "%f", *(double*)base);
        break;
    case EcsOpIPtr:
        ecs_strbuf_append(str, "%i", *(intptr_t*)base);
        break;
    case EcsOpUPtr:
        ecs_strbuf_append(str, "%u", *(uintptr_t*)base);
        break;
    case EcsOpString: {
        char *value = *(char**)base;
        if (value) {
            ecs_size_t length = ecs_stresc(NULL, 0, '"', value);
            if (length == ecs_os_strlen(value)) {
                ecs_strbuf_appendstrn(str, "\"", 1);
                ecs_strbuf_appendstr(str, value);
                ecs_strbuf_appendstrn(str, "\"", 1);
            } else {
                char *out = ecs_os_malloc(length + 3);
                ecs_stresc(out + 1, length, '"', value);
                out[0] = '"';
                out[length + 1] = '"';
                out[length + 2] = '\0';
                ecs_strbuf_appendstr_zerocpy(str, out);
            }
        } else {
            ecs_strbuf_appendstr(str, "null");
        }
        break;
    }
    case EcsOpEntity: {
        ecs_entity_t e = *(ecs_entity_t*)base;
        const char *name;
        if (e && (name = ecs_get_name(world, e))) {
            ecs_strbuf_appendstr(str, name);
        } else {
            ecs_strbuf_append(str, "%u", e);
        }

        break;
    }
    default:
        return -1;
    }

    return 0;
}

/* Serialize enumeration */
static
int expr_ser_enum(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
    ecs_assert(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t value = *(int32_t*)base;
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    ecs_entity_t *constant = ecs_map_get(
        enum_type->constants, ecs_entity_t, value);
    if (!constant) {
        return -1;
    }

    ecs_strbuf_appendstr(str, ecs_get_name(world, *constant));

    return 0;
}

/* Serialize bitmask */
static
int expr_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_assert(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    uint32_t value = *(uint32_t*)ptr;
    ecs_map_key_t key;
    ecs_entity_t *constant;
    int count = 0;

    ecs_strbuf_list_push(str, "", " | ");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(bitmask_type->constants);
    while ((constant = ecs_map_next(&it, ecs_entity_t, &key))) {
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, ecs_get_name(world, *constant));
            count ++;
        }
    }

    if (!count) {
        ecs_strbuf_list_appendstr(str, "0");
    }

    ecs_strbuf_list_pop(str, "");

    return 0;
}

/* Serialize elements of a contiguous array */
static
int expr_ser_elements(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops, 
    int32_t op_count,
    const void *base, 
    int32_t elem_count, 
    int32_t elem_size,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_push(str, "[", ", ");

    const void *ptr = base;

    int i;
    for (i = 0; i < elem_count; i ++) {
        ecs_strbuf_list_next(str);
        if (expr_ser_type_ops(world, ops, op_count, ptr, str)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    ecs_strbuf_list_pop(str, "]");

    return 0;
}

static
int expr_ser_type_elements(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void *base, 
    int32_t elem_count, 
    ecs_strbuf_t *str)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *ops = ecs_vector_first(ser->ops, ecs_meta_type_op_t);
    int32_t op_count = ecs_vector_count(ser->ops);

    return expr_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, str);
}

/* Serialize array */
static
int expr_ser_array(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsArray *a = ecs_get(world, op->type, EcsArray);
    ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

    return expr_ser_type_elements(
        world, a->type, ptr, op->count, str);
}

/* Serialize vector */
static
int expr_ser_vector(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_vector_t *value = *(ecs_vector_t**)base;
    if (!value) {
        ecs_strbuf_appendstr(str, "null");
        return 0;
    }

    const EcsVector *v = ecs_get(world, op->type, EcsVector);
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, v->type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vector_count(value);
    void *array = ecs_vector_first_t(value, comp->size, comp->alignment);

    /* Serialize contiguous buffer of vector */
    return expr_ser_type_elements(world, v->type, array, count, str);
}

/* Forward serialization to the different type kinds */
static
int expr_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr,
    ecs_strbuf_t *str) 
{
    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpEnum:
        if (expr_ser_enum(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpBitmask:
        if (expr_ser_bitmask(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpArray:
        if (expr_ser_array(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpVector:
        if (expr_ser_vector(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    default:
        if (expr_ser_primitive(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            /* Unknown operation */
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
            return -1;
        }
        break;
    }

    return 0;
}

/* Iterate over a slice of the type ops array */
static
int expr_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str) 
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (op != ops) {
            if (op->name) {
                ecs_strbuf_list_next(str);
                ecs_strbuf_append(str, "%s = ", op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1 && op != ops) {
                /* Serialize inline array */
                if (expr_ser_elements(world, op, op->op_count, base,
                    elem_count, op->size, str))
                {
                    return -1;
                }

                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            ecs_strbuf_list_push(str, "{", ", ");
            break;
        case EcsOpPop:
            ecs_strbuf_list_pop(str, "}");
            break;
        default:
            if (expr_ser_type_op(world, op, base, str)) {
                goto error;
            }
            break;
        }
    }

    return 0;
error:
    return -1;
}

/* Iterate over the type ops of a type */
static
int expr_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_type_op_t *ops = ecs_vector_first(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vector_count(v_ops);
    return expr_ser_type_ops(world, ops, count, base, str);
}

char* ecs_ptr_to_expr(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);

    if (expr_ser_type(world, ser->ops, ptr, &str)) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

#endif

