#include "meta.h"

#ifdef FLECS_META
#if defined(FLECS_JSON) || defined(FLECS_SCRIPT)
#ifdef FLECS_JSON
#include "../json/json.h"
#endif

static int flecs_meta_write_type_ops(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format);

static int flecs_meta_write_forward(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format);

static void flecs_meta_flt_to_str(
    char *buf,
    int32_t size,
    double value,
    bool is_f32)
{
    if (value == 0) {
        ecs_os_strcpy(buf, "0");
        return;
    }

    int32_t precision = is_f32 ? 6 : 15;
    int32_t max_precision = is_f32 ? 9 : 17;

    for (; precision < max_precision; precision ++) {
        ecs_os_snprintf(buf, size, "%.*g", precision, value);
        if (is_f32) {
            if ((float)strtod(buf, NULL) == (float)value) {
                return;
            }
        } else {
            if (strtod(buf, NULL) == value) {
                return;
            }
        }
    }

    ecs_os_snprintf(buf, size, "%.*g", max_precision, value);
}

static void flecs_meta_write_member(
    ecs_strbuf_t *str,
    const char *name,
    flecs_meta_format_t format)
{
#ifdef FLECS_JSON
    if (format == EcsMetaJson) {
        flecs_json_member(str, name);
        return;
    }
#endif
    ecs_strbuf_list_next(str);
    if (format != EcsMetaExprPositional &&
        format != EcsMetaExprPrecisePositional)
    {
        ecs_strbuf_append(str, "%s: ", name);
    }
}

static int flecs_meta_write_scope(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    if (flecs_meta_write_type_ops(
        world, ops + 1, ops->op_count - 2, base, str, format))
    {
        return -1;
    }
    return 0;
}

static int flecs_meta_write_array(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *array,
    int32_t count,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    ecs_strbuf_list_push(str, "[", ", ");

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(str);
        void *ptr = ECS_ELEM(array, ops->elem_size, i);
        if (flecs_meta_write_scope(world, ops, ptr, str, format)) {
            goto error;
        }
    }

    ecs_strbuf_list_pop(str, "]");

    return 0;
error:
    return -1;
}

static int flecs_meta_write_map(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    const ecs_map_t *map = base;

    bool json = format == EcsMetaJson;
    ecs_strbuf_list_push(str, json ? "{" : "[", ", ");

    ecs_map_iter_t it = ecs_map_iter(map);
    while (ecs_map_next(&it)) {
        ecs_strbuf_t key_buf = ECS_STRBUF_INIT;
        if (!json) {
            ecs_strbuf_list_next(str);
        }
        if (flecs_meta_ser_map_key(
            world, ops, ecs_map_key(&it), json ? &key_buf : str))
        {
            ecs_strbuf_reset(&key_buf);
            return -1;
        }
        if (json) {
            char *key = ecs_strbuf_get(&key_buf);
            flecs_meta_write_member(str, key, format);
            ecs_os_free(key);
        } else {
            ecs_strbuf_appendlit(str, ": ");
        }

        const void *ptr;
        if (ops->elem_size > ECS_SIZEOF(ecs_map_val_t)) {
            ptr = ecs_map_ptr(&it);
        } else {
            ptr = &it.res[1];
        }

        if (flecs_meta_write_scope(world, ops, ptr, str, format)) {
            goto error;
        }
    }

    ecs_strbuf_list_pop(str, json ? "}" : "]");

    return 0;
error:
    return -1;
}

static int flecs_meta_write_value(
    const ecs_world_t *world,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    const ecs_value_t *value = base;

    if (!value->type || !value->ptr) {
        ecs_assert(false, ECS_INVALID_PARAMETER,
            "cannot serialize value without value");
        ecs_err("cannot serialize value without value");
        return -1;
    }

    bool json = format == EcsMetaJson;
    ecs_strbuf_t type_buf = ECS_STRBUF_INIT;
    ecs_strbuf_list_push(str, "{", ", ");
    if (!json) {
        ecs_strbuf_list_next(str);
    }
    if (flecs_meta_value_type_str(world, value->type, json ? &type_buf : str)) {
        ecs_strbuf_reset(&type_buf);
        return -1;
    }
    if (json) {
        char *name = ecs_strbuf_get(&type_buf);
        flecs_meta_write_member(str, name, format);
        ecs_os_free(name);
    } else {
        ecs_strbuf_appendlit(str, ": ");
    }

    if (flecs_meta_write_forward(
        world, value->type, value->ptr, str, format))
    {
        return -1;
    }

    ecs_strbuf_list_pop(str, "}");

    return 0;
}

static int flecs_meta_write_struct(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    ecs_strbuf_list_push(str, "{", ", ");

    if (flecs_meta_write_scope(world, ops, base, str, format)) {
        return -1;
    }

    ecs_strbuf_list_pop(str, "}");

    return 0;
}

static int flecs_meta_write_forward(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    const EcsTypeSerializer *ts = ecs_get(world, type, EcsTypeSerializer);
    if (!ts) {
        ecs_err("missing type serializer for '%s'",
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }

    return flecs_meta_write_type_ops(world, ecs_vec_first(&ts->ops),
        ecs_vec_count(&ts->ops), base, str, format);
}

typedef struct flecs_meta_writer_t {
    ecs_strbuf_t *str;
    bool is_collection;
    flecs_meta_format_t format;
} flecs_meta_writer_t;

static int flecs_meta_write_opaque_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value)
{
    flecs_meta_writer_t *writer = ser->ctx;
    if (writer->is_collection) {
        ecs_strbuf_list_next(writer->str);
    }
    return flecs_meta_write_forward(
        ser->world, type, value, writer->str, writer->format);
}

static int flecs_meta_write_opaque_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    flecs_meta_writer_t *writer = ser->ctx;
    flecs_meta_write_member(writer->str, name, writer->format);
    return 0;
}

static int flecs_meta_write_opaque(
    const ecs_world_t *world,
    ecs_meta_op_t *op,
    const void *base,
    ecs_strbuf_t *str,
    ecs_meta_op_kind_t kind,
    flecs_meta_format_t format)
{
    bool is_struct = kind == EcsOpOpaqueStruct;
    bool is_collection = kind == EcsOpOpaqueVector || kind == EcsOpOpaqueArray;

    if (is_struct) {
        ecs_strbuf_list_push(str, "{", ", ");
    } else if (is_collection) {
        ecs_strbuf_list_push(str, "[", ", ");
    }

    flecs_meta_writer_t writer = {
        .str = str, .is_collection = is_collection,
        .format = format == EcsMetaJson ? EcsMetaJson :
            format >= EcsMetaExprPositional ? EcsMetaExprPositional : EcsMetaExpr
    };

    ecs_serializer_t ser = {
        .world = world,
        .value = flecs_meta_write_opaque_value,
        .member = is_struct ? flecs_meta_write_opaque_member : NULL,
        .ctx = &writer
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

static int flecs_meta_write_primitive(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    const void *ptr,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    ecs_meta_op_kind_t kind = op->kind;
#ifdef FLECS_JSON
    if (format == EcsMetaJson) {
        switch (kind) {
        case EcsOpEntity:
        case EcsOpId: {
            ecs_id_t id = *(const ecs_id_t*)ptr;
            if (!id) {
                ecs_strbuf_appendlit(str, "\"#0\"");
            } else if (kind == EcsOpEntity) {
                flecs_json_path(str, world, id);
            } else {
                flecs_json_id(str, world, id);
            }
            return 0;
        }
        case EcsOpString:
            flecs_json_string_escape_ctrl(str, *(char* const*)ptr);
            return 0;
        case EcsOpF32:
        case EcsOpF64:
            ecs_strbuf_appendflt(str, kind == EcsOpF32
                ? (double)*(const float*)ptr : *(const double*)ptr, '"');
            return 0;
        default:
            break;
        }
    }
#endif
    if ((format == EcsMetaExprPrecise || format == EcsMetaExprPrecisePositional)
        && (kind == EcsOpF32 || kind == EcsOpF64))
    {
        char buf[32];
        bool is_f32 = kind == EcsOpF32;
        flecs_meta_flt_to_str(buf, 32, is_f32
            ? (double)*(const float*)ptr : *(const double*)ptr, is_f32);
        ecs_strbuf_appendstr(str, buf);
        return 0;
    }
    if (kind <= EcsOpPrimitive || kind > EcsMetaTypeOpKindLast) {
        ecs_throw(ECS_INVALID_PARAMETER, "invalid serializer operation");
    }
    bool quote = format == EcsMetaJson &&
        ((kind == EcsOpI64 && *(const int64_t*)ptr >= 2147483648) ||
         (kind == EcsOpU64 && *(const uint64_t*)ptr >= 2147483648));
    if (quote) {
        ecs_strbuf_appendch(str, '"');
    }
    if (flecs_meta_ser_primitive(world, kind - EcsOpPrimitive,
        ptr, str, format != EcsMetaStr))
    {
        return -1;
    }
    if (quote) {
        ecs_strbuf_appendch(str, '"');
    }
    return 0;
error:
    return -1;
}

static int flecs_meta_write_type_ops(
    const ecs_world_t *world,
    ecs_meta_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_op_t *op = &ops[i];

        const void *ptr = ECS_OFFSET(base, op->offset);

        if (op->name) {
            flecs_meta_write_member(str, op->name, format);
        }

        switch(op->kind) {
        case EcsOpPushStruct: {
            if (flecs_meta_write_struct(world, op, ptr, str, format)) {
                goto error;
            }
            break;
        }
        case EcsOpPushArray: {
            if (flecs_meta_write_array(world, op, ptr,
                ecs_meta_op_get_elem_count(op, ptr), str, format))
            {
                goto error;
            }
            break;
        }
        case EcsOpPushVector: {
            ecs_vec_t *vec = ECS_OFFSET(base, op->offset);

            if (flecs_meta_write_array(world, op,
                vec->array, vec->count, str, format))
            {
                goto error;
            }
            break;
        }
        case EcsOpPushMap: {
            if (flecs_meta_write_map(world, op, ptr, str, format)) {
                goto error;
            }
            break;
        }
        case EcsOpPushValue: {
            if (flecs_meta_write_value(world, ptr, str, format)) {
                goto error;
            }
            break;
        }
        case EcsOpForward: {
            if (flecs_meta_write_forward(world, op->type, ptr, str, format)) {
                goto error;
            }
            break;
        }
        case EcsOpOpaqueStruct:
        case EcsOpOpaqueArray:
        case EcsOpOpaqueVector:
        case EcsOpOpaqueValue:
            if (flecs_meta_write_opaque(world, op, ptr, str, op->kind, format)) {
                goto error;
            }
            break;
        case EcsOpEnum:
            if (format == EcsMetaJson) {
                ecs_strbuf_appendch(str, '"');
            }
            if (flecs_meta_ser_enum(world, op->type,
                op->underlying_kind, op->is.constants, ptr, str))
            {
                return -1;
            }
            if (format == EcsMetaJson) {
                ecs_strbuf_appendch(str, '"');
            }
            break;
        case EcsOpBitmask:
            if (flecs_meta_ser_bitmask(world, op->type, op->is.constants,
                ptr, format == EcsMetaJson ? "\"" : "", str))
            {
                return -1;
            }
            break;
        default:
            if (flecs_meta_write_primitive(world, op, ptr, str, format)) {
                return -1;
            }
            break;
        }

        i += op->op_count - 1;
    }

    return 0;
error:
    return -1;
}

int flecs_meta_serialize(
    const ecs_world_t *world,
    const ecs_vec_t *v_ops,
    const void *base,
    ecs_strbuf_t *str,
    flecs_meta_format_t format)
{
    ecs_meta_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return flecs_meta_write_type_ops(
        world, ops, count, base, str, format);
}

#endif

#endif
