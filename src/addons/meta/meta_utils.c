/**
 * @file addons/meta/serializer.c
 * @brief Build instruction list for serializing/deserializing types.
 */

#include "meta.h"

#ifdef FLECS_META

static
int flecs_meta_ser_scalar(
    const ecs_world_t *world,
    ecs_meta_op_kind_t op_kind,
    const void *ptr, 
    ecs_strbuf_t *str,
    bool is_expr)
{
    switch(op_kind) {
    case EcsOpBool:
        if (*(const bool*)ptr) {
            ecs_strbuf_appendlit(str, "true");
        } else {
            ecs_strbuf_appendlit(str, "false");
        }
        break;
    case EcsOpChar: {
        char chbuf[3];
        char ch = *(const char*)ptr;
        if (ch) {
            flecs_chresc(chbuf, *(const char*)ptr, '"');
            if (is_expr) ecs_strbuf_appendch(str, '"');
            ecs_strbuf_appendstr(str, chbuf);
            if (is_expr) ecs_strbuf_appendch(str, '"');
        } else {
            ecs_strbuf_appendch(str, '0');
        }
        break;
    }
    case EcsOpByte:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t*)ptr));
        break;
    case EcsOpU8:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t*)ptr));
        break;
    case EcsOpU16:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint16_t*)ptr));
        break;
    case EcsOpU32:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint32_t*)ptr));
        break;
    case EcsOpU64:
        ecs_strbuf_append(str, "%llu", *(const uint64_t*)ptr);
        break;
    case EcsOpI8:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int8_t*)ptr));
        break;
    case EcsOpI16:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int16_t*)ptr));
        break;
    case EcsOpI32:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int32_t*)ptr));
        break;
    case EcsOpI64:
        ecs_strbuf_appendint(str, *(const int64_t*)ptr);
        break;
    case EcsOpF32:
        ecs_strbuf_appendflt(str, (double)*(const float*)ptr, 0);
        break;
    case EcsOpF64:
        ecs_strbuf_appendflt(str, *(const double*)ptr, 0);
        break;
    case EcsOpIPtr:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const intptr_t*)ptr));
        break;
    case EcsOpUPtr:
        ecs_strbuf_append(str, "%llu",
            (unsigned long long)*(const uintptr_t*)ptr);
        break;
    case EcsOpString: {
        const char *value = *ECS_CONST_CAST(const char**, ptr);
        if (value) {
            if (!is_expr) {
                ecs_strbuf_appendstr(str, value);
            } else {
                ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
                if (length == ecs_os_strlen(value)) {
                    ecs_strbuf_appendch(str, '"');
                    ecs_strbuf_appendstrn(str, value, length);
                    ecs_strbuf_appendch(str, '"');
                } else {
                    char *out = ecs_os_malloc(length + 3);
                    flecs_stresc(out + 1, length, '"', value);
                    out[0] = '"';
                    out[length + 1] = '"';
                    out[length + 2] = '\0';
                    ecs_strbuf_appendstr(str, out);
                    ecs_os_free(out);
                }
            }
        } else {
            ecs_strbuf_appendlit(str, "null");
        }
        break;
    }
    case EcsOpEntity: {
        ecs_entity_t e = *(const ecs_entity_t*)ptr;
        if (!e) {
            ecs_strbuf_appendlit(str, "#0");
        } else {
            ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
        }
        break;
    }
    case EcsOpId: {
        ecs_id_t id = *(const ecs_id_t*)ptr;
        if (!id) {
            ecs_strbuf_appendlit(str, "#0");
        } else {
            ecs_id_str_buf(world, id, str);
        }
        break;
    }
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpForward:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPushMap:
    case EcsOpPushValue:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpPrimitive:
    case EcsOpScope:
        ecs_err("invalid primitive kind");
        return -1;
    }

    return 0;
}

ecs_meta_op_kind_t flecs_meta_primitive_to_op_kind(
    ecs_primitive_kind_t kind)
{
    switch(kind) {
    case EcsBool: return EcsOpBool;
    case EcsChar: return EcsOpChar;
    case EcsByte: return EcsOpByte;
    case EcsU8: return EcsOpU8;
    case EcsU16: return EcsOpU16;
    case EcsU32: return EcsOpU32;
    case EcsU64: return EcsOpU64;
    case EcsI8: return EcsOpI8;
    case EcsI16: return EcsOpI16;
    case EcsI32: return EcsOpI32;
    case EcsI64: return EcsOpI64;
    case EcsF32: return EcsOpF32;
    case EcsF64: return EcsOpF64;
    case EcsUPtr: return EcsOpUPtr;
    case EcsIPtr: return EcsOpIPtr;
    case EcsString: return EcsOpString;
    case EcsEntity: return EcsOpEntity;
    case EcsId: return EcsOpId;
    default: ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

ecs_entity_t flecs_meta_op_kind_to_type(
    ecs_meta_op_kind_t kind)
{
    switch(kind) {
    case EcsOpBool: return ecs_id(ecs_bool_t);
    case EcsOpChar: return ecs_id(ecs_char_t);
    case EcsOpByte:  return ecs_id(ecs_byte_t);
    case EcsOpU8: return ecs_id(ecs_u8_t);
    case EcsOpU16: return ecs_id(ecs_u16_t);
    case EcsOpU32: return ecs_id(ecs_u32_t);
    case EcsOpU64: return ecs_id(ecs_u64_t);
    case EcsOpI8: return ecs_id(ecs_i8_t);
    case EcsOpI16: return ecs_id(ecs_i16_t);
    case EcsOpI32: return ecs_id(ecs_u32_t);
    case EcsOpI64: return ecs_id(ecs_u64_t);
    case EcsOpF32: return ecs_id(ecs_f32_t);
    case EcsOpF64: return ecs_id(ecs_f64_t);
    case EcsOpIPtr: return ecs_id(ecs_iptr_t);
    case EcsOpUPtr: return ecs_id(ecs_uptr_t);
    case EcsOpString: return ecs_id(ecs_string_t);
    case EcsOpEntity: return ecs_id(ecs_entity_t);
    case EcsOpId: return ecs_id(ecs_id_t);
    case EcsOpEnum:
    case EcsOpBitmask:
    case EcsOpForward:
    case EcsOpPushStruct:
    case EcsOpPushArray:
    case EcsOpPushVector:
    case EcsOpPushMap:
    case EcsOpPushValue:
    case EcsOpOpaqueValue:
    case EcsOpOpaqueStruct:
    case EcsOpOpaqueArray:
    case EcsOpOpaqueVector:
    case EcsOpPop:
    case EcsOpPrimitive:
    case EcsOpScope:
        ecs_err("cannot convert kind to type");
    }

    return 0;
}

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
    case EcsOpPushMap: return "PushMap";
    case EcsOpPushValue: return "PushValue";
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

bool flecs_meta_valid_digit(
    const char *str)
{
    return str[0] == '-' || isdigit(str[0]);
}

int flecs_value_blit_u64(
    const ecs_world_t *world,
    const ecs_value_t *value,
    ecs_map_key_t *out)
{
    const EcsComponent *comp = ecs_get(world, value->type, EcsComponent);
    if (!comp) {
        ecs_err("entity '%s' is not a type", 
            flecs_errstr(ecs_get_path(world, value->type)));
        return -1;
    }

    if (comp->size > ECS_SIZEOF(*out)) {
        ecs_err("type '%s' is too large for a u64",
            flecs_errstr(ecs_get_path(world, value->type)));
        return -1;
    }

    *out = 0;
    ecs_os_memcpy(out, value->ptr, comp->size);

    return 0;
}

int flecs_meta_value_type_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);

    const char *symbol = ecs_get_symbol(world, type);
    if (symbol) {
        ecs_strbuf_appendstr(str, symbol);
        return 0;
    }

    char *path = ecs_get_path(world, type);
    if (!path) {
        ecs_err("cannot serialize value type without name");
        return -1;
    }

    ecs_strbuf_appendstr(str, path);
    ecs_os_free(path);

    return 0;
}

int flecs_meta_ser_enum(
    const ecs_world_t *world,
    ecs_entity_t enum_type,
    ecs_meta_op_kind_t op_kind,
    ecs_map_t *constants,
    const void *ptr,
    ecs_strbuf_t *str)
{
    ecs_map_key_t value;

    if (op_kind == EcsOpU8) {
        value = *(const uint8_t*)ptr;
    } else if (op_kind == EcsOpI8) {
        value = (ecs_map_key_t)(int64_t)*(const int8_t*)ptr;
    } else if (op_kind == EcsOpU16) {
        value = *(const uint16_t*)ptr;
    } else if (op_kind == EcsOpI16) {
        value = (ecs_map_key_t)(int64_t)*(const int16_t*)ptr;
    } else if (op_kind == EcsOpU32) {
        value = *(const uint32_t*)ptr;
    } else if (op_kind == EcsOpI32) {
        value = (ecs_map_key_t)(int64_t)*(const int32_t*)ptr;
    } else if (op_kind == EcsOpUPtr) {
        value = *(const uintptr_t*)ptr;
    } else if (op_kind == EcsOpIPtr) {
        value = (ecs_map_key_t)(int64_t)*(const intptr_t*)ptr;
    } else if (op_kind == EcsOpU64 || op_kind == EcsOpI64) {
        value = *(const uint64_t*)ptr;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, "invalid underlying type");
    }
    
    ecs_enum_constant_t *constant = ecs_map_get_deref(constants,
        ecs_enum_constant_t, value);
    if (!constant) {
        char *name = enum_type ? ecs_get_path(world, enum_type) : NULL;
        ecs_err("enumeration value '%d' of type '%s' is not a valid constant", 
            value, name ? name : "unknown");
        ecs_os_free(name);
        return -1;
    }

    ecs_strbuf_appendstr(str, ecs_get_name(world, constant->constant));
    return 0;
}

int flecs_meta_ser_bitmask(
    const ecs_world_t *world,
    ecs_entity_t bitmask_type,
    ecs_map_t *constants,
    const void *ptr,
    const char *delim,
    ecs_strbuf_t *str)
{
    uint32_t value = *(const uint32_t*)ptr;

    if (!value) {
        ecs_strbuf_appendstr(str, "0");
        return 0;
    }

    ecs_strbuf_list_push(str, delim, "|");

    ecs_map_iter_t it = ecs_map_iter(constants);
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, ecs_get_name(world, c->constant));
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        char *name = bitmask_type ? ecs_get_path(world, bitmask_type) : NULL;
        ecs_err("value for bitmask %s contains bits (%u) that cannot be mapped"
            " to a constant", name ? name : "unknown", value);
        ecs_os_free(name);
        goto error;
    }

    ecs_strbuf_list_pop(str, delim);

    return 0;
error:
    return -1;
}

int flecs_meta_ser_map_key(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    ecs_map_key_t key,
    ecs_strbuf_t *str)
{
    if (op->underlying_kind == EcsOpEnum) {
        if (flecs_meta_ser_enum(
            world, 0, EcsOpU64, op->is.constants, &key, str)) 
        {
            return -1;
        }
    } else if (op->underlying_kind == EcsOpBitmask) {
        uint32_t value = (uint32_t)key;
        if (!value) {
            ecs_strbuf_appendch(str, '0');
            return 0;
        }

        ecs_strbuf_list_push(str, "", "|");
        ecs_map_iter_t it = ecs_map_iter(op->is.constants);
        while (ecs_map_next(&it)) {
            ecs_bitmask_constant_t *constant = ecs_map_ptr(&it);
            ecs_map_key_t constant_key = ecs_map_key(&it);
            if ((value & constant_key) == constant_key) {
                ecs_strbuf_list_appendstr(str,
                    ecs_get_name(world, constant->constant));
                value -= (uint32_t)constant_key;
            }
        }

        ecs_strbuf_list_pop(str, "");

        if (value != 0) {
            char *name = ecs_get_path(world, op->type);
            ecs_err("map key '%llu' of type '%s' contains invalid/unknown "
                "bits", (unsigned long long)key, name);
            ecs_os_free(name);
            return -1;
        }
    } else {
        return flecs_meta_ser_scalar(
            world, op->underlying_kind, &key, str, false);
    }

    return 0;
}

int flecs_meta_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *ptr, 
    ecs_strbuf_t *str,
    bool is_expr) 
{
    return flecs_meta_ser_scalar(
        world, flecs_meta_primitive_to_op_kind(kind), ptr, str, is_expr);
}

int flecs_meta_find_constant(
    const ecs_world_t *world,
    const ecs_map_t *constants,
    const char *name,
    uint64_t *value_out)
{
    ecs_map_iter_t it = ecs_map_iter(constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *constant = ecs_map_ptr(&it);
        if (!ecs_os_strcmp(
            ecs_get_name(world, constant->constant), name))
        {
            *value_out = ecs_map_key(&it);
            return 0;
        }
    }
    return -1;
}

static
int flecs_meta_add_bitmask_constant(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_map_t *constants,
    void *out,
    const char *value)
{
    if (!ecs_os_strcmp(value, "0")) {
        return 0;
    }
    
    uint32_t v;

    if (type) {
        ecs_entity_t c = ecs_lookup_child(world, type, value);
        if (!c) {
            char *path = ecs_get_path(world, type);
            ecs_err("unresolved bitmask constant '%s' for type '%s'", 
                value, path);
            ecs_os_free(path);
            return -1;
        }

        const uint32_t *vptr = ecs_get_pair_second(
            world, c, EcsConstant, ecs_u32_t);
        if (vptr == NULL) {
            char *path = ecs_get_path(world, type);
            ecs_err("'%s' is not a bitmask constant for type '%s'", 
                value, path);
            ecs_os_free(path);
            return -1;
        }

        v = *vptr;
    } else {
        uint64_t constant = 0;
        flecs_meta_find_constant(world, constants, value, &constant);
        v = (uint32_t)constant;
    }

    *(ecs_u32_t*)out |= v;

    return 0;
}

int flecs_meta_parse_bitmask(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_map_t *constants,
    void *out,
    const char *value)
{
    char token[ECS_MAX_TOKEN_SIZE];

    const char *prev = value, *ptr = value;

    *(ecs_u32_t*)out = 0;

    while ((ptr = strchr(ptr, '|'))) {
        ecs_os_memcpy(token, prev, ptr - prev);
        token[ptr - prev] = '\0';
        if (flecs_meta_add_bitmask_constant(world, type, constants, out, token)) {
            goto error;
        }

        ptr ++;
        prev = ptr;
    }

    if (flecs_meta_add_bitmask_constant(world, type, constants, out, prev)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_meta_parse_enum(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_map_t *constants,
    void *out,
    const char *value)
{
    ecs_value_t v;

    if (!type) {
        ecs_assert(constants != NULL, ECS_INTERNAL_ERROR, NULL);
        uint64_t constant = 0;
        if (flecs_meta_find_constant(world, constants, value, &constant)) {
            goto error;
        }
        *(uint64_t*)out = constant;
        return 0;
    }

    {
        ecs_entity_t c = ecs_lookup_child(world, type, value);

        if (!c) {
            char *path = ecs_get_path(world, type);
            ecs_err("unresolved enum constant '%s' for type '%s'", value, path);
            ecs_os_free(path);
            goto error;
        }

        ecs_entity_t underlying = ecs_get_target(world, c, EcsConstant, 0);
        if (!underlying) {
            char *path = ecs_get_path(world, type);
            ecs_err("'%s' is not an enum constant for type '%s'", value, path);
            ecs_os_free(path);
            goto error;
        }

        const void *cptr = ecs_get_id(world, c, ecs_pair(EcsConstant, underlying));
        if (!cptr) {
            char *path = ecs_get_path(world, type);
            ecs_err("constant '%s' enum '%s' is of an invalid underlying type", 
                value, path);
            ecs_os_free(path);
            goto error;
        }

        v = (ecs_value_t) {
            .ptr = ECS_CONST_CAST(void*, cptr),
            .type = underlying
        };
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, out);
    ecs_meta_set_value(&cur, &v);

    return 0;
error:
    return -1;
}

#endif
