/**
 * @file expr/serialize.c
 * @brief Serialize (component) values to flecs string format.
 */

#include "../../private_api.h"

#ifdef FLECS_EXPR

static
int flecs_expr_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
int flecs_expr_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str,
    int32_t in_array);

static
int flecs_expr_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

static
ecs_primitive_kind_t flecs_expr_op_to_primitive_kind(ecs_meta_type_op_kind_t kind) {
    return kind - EcsOpPrimitive;
}

/* Serialize a primitive value */
static
int flecs_expr_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base, 
    ecs_strbuf_t *str) 
{
    switch(kind) {
    case EcsBool:
        if (*(bool*)base) {
            ecs_strbuf_appendlit(str, "true");
        } else {
            ecs_strbuf_appendlit(str, "false");
        }
        break;
    case EcsChar: {
        char chbuf[3];
        char ch = *(char*)base;
        if (ch) {
            ecs_chresc(chbuf, *(char*)base, '"');
            ecs_strbuf_appendch(str, '"');
            ecs_strbuf_appendstr(str, chbuf);
            ecs_strbuf_appendch(str, '"');
        } else {
            ecs_strbuf_appendch(str, '0');
        }
        break;
    }
    case EcsByte:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(uint8_t*)base));
        break;
    case EcsU8:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(uint8_t*)base));
        break;
    case EcsU16:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(uint16_t*)base));
        break;
    case EcsU32:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(uint32_t*)base));
        break;
    case EcsU64:
        ecs_strbuf_append(str, "%llu", *(uint64_t*)base);
        break;
    case EcsI8:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(int8_t*)base));
        break;
    case EcsI16:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(int16_t*)base));
        break;
    case EcsI32:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(int32_t*)base));
        break;
    case EcsI64:
        ecs_strbuf_appendint(str, *(int64_t*)base);
        break;
    case EcsF32:
        ecs_strbuf_appendflt(str, (double)*(float*)base, 0);
        break;
    case EcsF64:
        ecs_strbuf_appendflt(str, *(double*)base, 0);
        break;
    case EcsIPtr:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(intptr_t*)base));
        break;
    case EcsUPtr:
        ecs_strbuf_append(str, "%u", *(uintptr_t*)base);
        break;
    case EcsString: {
        char *value = *(char**)base;
        if (value) {
            ecs_size_t length = ecs_stresc(NULL, 0, '"', value);
            if (length == ecs_os_strlen(value)) {
                ecs_strbuf_appendch(str, '"');
                ecs_strbuf_appendstrn(str, value, length);
                ecs_strbuf_appendch(str, '"');
            } else {
                char *out = ecs_os_malloc(length + 3);
                ecs_stresc(out + 1, length, '"', value);
                out[0] = '"';
                out[length + 1] = '"';
                out[length + 2] = '\0';
                ecs_strbuf_appendstr_zerocpy(str, out);
            }
        } else {
            ecs_strbuf_appendlit(str, "null");
        }
        break;
    }
    case EcsEntity: {
        ecs_entity_t e = *(ecs_entity_t*)base;
        if (!e) {
            ecs_strbuf_appendch(str, '0');
        } else {
            ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str);
        }
        break;
    }
    default:
        ecs_err("invalid primitive kind");
        return -1;
    }

    return 0;
}

/* Serialize enumeration */
static
int flecs_expr_ser_enum(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
    ecs_check(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t val = *(int32_t*)base;
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    ecs_enum_constant_t *c = ecs_map_get_deref(&enum_type->constants, 
        ecs_enum_constant_t, (ecs_map_key_t)val);
    if (!c) {
        char *path = ecs_get_fullpath(world, op->type);
        ecs_err("value %d is not valid for enum type '%s'", val, path);
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
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);
    uint32_t value = *(uint32_t*)ptr;

    ecs_strbuf_list_push(str, "", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
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
        char *path = ecs_get_fullpath(world, op->type);
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
        if (flecs_expr_ser_type_ops(world, ops, op_count, ptr, str, 1)) {
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
        world, a->type, ptr, a->count, str);
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
        ecs_strbuf_appendlit(str, "null");
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
int flecs_expr_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr,
    ecs_strbuf_t *str) 
{
    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpEnum:
        if (flecs_expr_ser_enum(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpBitmask:
        if (flecs_expr_ser_bitmask(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpArray:
        if (expr_ser_array(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpVector:
        if (expr_ser_vector(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    default:
        if (flecs_expr_ser_primitive(world, flecs_expr_op_to_primitive_kind(op->kind), 
            ECS_OFFSET(ptr, op->offset), str)) 
        {
            /* Unknown operation */
            ecs_err("unknown serializer operation kind (%d)", op->kind);
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

/* Iterate over a slice of the type ops array */
static
int flecs_expr_ser_type_ops(
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
                ecs_strbuf_list_next(str);
                ecs_strbuf_append(str, "%s: ", op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1) {
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
            in_array --;
            break;
        case EcsOpPop:
            ecs_strbuf_list_pop(str, "}");
            in_array ++;
            break;
        default:
            if (flecs_expr_ser_type_op(world, op, base, str)) {
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
int flecs_expr_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_type_op_t *ops = ecs_vector_first(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vector_count(v_ops);
    return flecs_expr_ser_type_ops(world, ops, count, base, str, 0);
}

int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    if (ser == NULL) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }

    if (flecs_expr_ser_type(world, ser->ops, ptr, buf_out)) {
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

int ecs_primitive_to_expr_buf(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base, 
    ecs_strbuf_t *str)
{
    return flecs_expr_ser_primitive(world, kind, base, str);
}

#endif
