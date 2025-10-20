/**
 * @file addons/meta/serializer.c
 * @brief Build instruction list for serializing/deserializing types.
 */

#include "meta.h"

#ifdef FLECS_META

static
int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops);

/* Serialize a primitive value */
int flecs_expr_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base, 
    ecs_strbuf_t *str,
    bool is_expr) 
{
    switch(kind) {
    case EcsBool:
        if (*(const bool*)base) {
            ecs_strbuf_appendlit(str, "true");
        } else {
            ecs_strbuf_appendlit(str, "false");
        }
        break;
    case EcsChar: {
        char chbuf[3];
        char ch = *(const char*)base;
        if (ch) {
            flecs_chresc(chbuf, *(const char*)base, '"');
            if (is_expr) ecs_strbuf_appendch(str, '"');
            ecs_strbuf_appendstr(str, chbuf);
            if (is_expr) ecs_strbuf_appendch(str, '"');
        } else {
            ecs_strbuf_appendch(str, '0');
        }
        break;
    }
    case EcsByte:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t*)base));
        break;
    case EcsU8:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t*)base));
        break;
    case EcsU16:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint16_t*)base));
        break;
    case EcsU32:
        ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint32_t*)base));
        break;
    case EcsU64:
        ecs_strbuf_append(str, "%llu", *(const uint64_t*)base);
        break;
    case EcsI8:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int8_t*)base));
        break;
    case EcsI16:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int16_t*)base));
        break;
    case EcsI32:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int32_t*)base));
        break;
    case EcsI64:
        ecs_strbuf_appendint(str, *(const int64_t*)base);
        break;
    case EcsF32:
        ecs_strbuf_appendflt(str, (double)*(const float*)base, 0);
        break;
    case EcsF64:
        ecs_strbuf_appendflt(str, *(const double*)base, 0);
        break;
    case EcsIPtr:
        ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const intptr_t*)base));
        break;
    case EcsUPtr:
        ecs_strbuf_append(str, "%u", *(const uintptr_t*)base);
        break;
    case EcsString: {
        const char *value = *ECS_CONST_CAST(const char**, base);
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
    case EcsEntity: {
        ecs_entity_t e = *(const ecs_entity_t*)base;
        if (!e) {
            ecs_strbuf_appendlit(str, "#0");
        } else {
            ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
        }
        break;
    }
    case EcsId: {
        ecs_id_t id = *(const ecs_id_t*)base;
        if (!id) {
            ecs_strbuf_appendlit(str, "#0");
        } else {
            ecs_id_str_buf(world, id, str);
        }
        break;
    }
    default:
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

static
ecs_size_t flecs_meta_type_size(ecs_world_t *world, ecs_entity_t type) {
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);
    return comp->size;
}

static
ecs_meta_op_t* flecs_meta_ops_add(ecs_vec_t *ops, ecs_meta_op_kind_t kind) {
    ecs_meta_op_t *op = ecs_vec_append_t(NULL, ops, ecs_meta_op_t);
    op->kind = kind;
    op->offset = 0;
    op->op_count = 1;
    op->type_info = NULL;
    op->name = NULL;
    op->is.members = NULL;
    op->type = 0;
    op->member_index = 0;
    return op;
}

static
ecs_meta_op_t* flecs_meta_ops_get(ecs_vec_t *ops, int32_t index) {
    ecs_meta_op_t* op = ecs_vec_get_t(ops, ecs_meta_op_t, index);
    ecs_assert(op != NULL, ECS_INTERNAL_ERROR, NULL);
    return op;
}

static
int flecs_meta_serialize_primitive(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsPrimitive *ptr = ecs_get(world, type, EcsPrimitive);
    if (!ptr) {
        char *name = ecs_get_path(world, type);
        ecs_err("entity '%s' is not a primitive type", name);
        ecs_os_free(name);
        return -1;
    }

    ecs_meta_op_t *op = flecs_meta_ops_add(ops, 
        flecs_meta_primitive_to_op_kind(ptr->kind));
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    return 0;
}

static
int flecs_meta_serialize_enum(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    
    ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpEnum);
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsConstants *constants = ecs_get(world, type, EcsConstants);
    ecs_assert(constants != NULL, ECS_INVALID_PARAMETER, NULL);
    op->is.constants = constants->constants;
    ecs_assert(op->is.constants != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsEnum *enum_type = ecs_get(world, type, EcsEnum);
    ecs_assert(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t underlying = enum_type->underlying_type;
    ecs_assert(underlying != 0, ECS_INTERNAL_ERROR, NULL);

    const EcsPrimitive *prim_type = ecs_get(world, underlying, EcsPrimitive);
    ecs_assert(prim_type != NULL, ECS_INTERNAL_ERROR, NULL);

    switch(prim_type->kind) {
    case EcsU8: 
        op->underlying_kind = EcsOpU8; 
        break;
    case EcsU16:
        op->underlying_kind = EcsOpU16;
        break;
    case EcsU32:
        op->underlying_kind = EcsOpU32;
        break;
    case EcsUPtr:
        op->underlying_kind = EcsOpUPtr;
        break;
    case EcsU64:
        op->underlying_kind = EcsOpU64;
        break;
    case EcsI8:
        op->underlying_kind = EcsOpI8;
        break;
    case EcsI16:
        op->underlying_kind = EcsOpI16;
        break;
    case EcsI32:
        op->underlying_kind = EcsOpI32;
        break;
    case EcsIPtr:
        op->underlying_kind = EcsOpIPtr;
        break;
    case EcsI64:
        op->underlying_kind = EcsOpI64;
        break;
    case EcsBool:
    case EcsChar:
    case EcsByte:
    case EcsF32:
    case EcsF64:
    case EcsString:
    case EcsEntity:
    case EcsId:
        ecs_abort(ECS_INTERNAL_ERROR, 
            "invalid primitive type kind for underlying enum type");
    }

    return 0;
}

static
int flecs_meta_serialize_bitmask(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    
    ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpBitmask);
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsConstants *bitmask_type = ecs_get(world, type, EcsConstants);
    ecs_assert(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);
    op->is.constants = bitmask_type->constants;
    ecs_assert(op->is.constants != NULL, ECS_INTERNAL_ERROR, NULL);

    return 0;
}

static
int flecs_meta_serialize_array_inline(
    ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    int32_t first = ecs_vec_count(ops);

    {
        ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpPushArray);
        op->type = elem_type;
        op->type_info = NULL;
        op->elem_size = flecs_meta_type_size(world, elem_type);
        op->offset = offset;
    }

    if (flecs_meta_serialize_type(world, elem_type, 0, ops) != 0) {
        return -1;
    }

    {
        ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpPop);
        op->type = elem_type;
        op->type_info = NULL;
        op->offset = 0;
        op->elem_size = count;
    }

    flecs_meta_ops_get(ops, first)->op_count = 
        flecs_ito(int16_t, ecs_vec_count(ops) - first);

    return 0;
}

static
int flecs_meta_serialize_array_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *ops)
{
    const EcsArray *ptr = ecs_get(world, type, EcsArray);
    if (!ptr) {
        return -1;
    }

    return flecs_meta_serialize_array_inline(
        world, ptr->type, ptr->count, 0, ops);
}

static
int flecs_meta_serialize_vector_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *ops)
{
    const EcsVector *ptr = ecs_get(world, type, EcsVector);
    if (!ptr) {
        return -1; /* Should never happen, will trigger internal error */
    }

    int32_t first = ecs_vec_count(ops);

    {
        ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpPushVector);
        op->offset = 0;
        op->type = type;
        op->type_info = ecs_get_type_info(world, type);
        ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
        op->elem_size = flecs_meta_type_size(world, ptr->type);
    }

    if (flecs_meta_serialize_type(world, ptr->type, 0, ops) != 0) {
        return -1;
    }

    {
        ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpPop);
        op->offset = 0;
        op->type = type;
        op->type_info = ecs_get_type_info(world, type);
        ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_meta_ops_get(ops, first)->op_count = 
        flecs_ito(int16_t, ecs_vec_count(ops) - first);

    return 0;
}

static
int flecs_meta_serialize_forward(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpForward);
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    return 0;
}

static
int flecs_meta_serialize_opaque_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    const EcsOpaque *o = ecs_get(world, type, EcsOpaque);
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
    const EcsType *t = ecs_get(world, o->as_type, EcsType);
    ecs_assert(o != NULL, ECS_INTERNAL_ERROR, 
        "missing reflection for Opaque::as_type");

    ecs_meta_op_kind_t kind = EcsOpOpaqueValue;
    if (t->kind == EcsArrayType) {
        kind = EcsOpOpaqueArray;
    } else if (t->kind == EcsVectorType) {
        kind = EcsOpOpaqueVector;
    } else if (t->kind == EcsStructType) {
        kind = EcsOpOpaqueStruct;
    }

    ecs_meta_op_t *op = flecs_meta_ops_add(ops, kind);
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    op->is.opaque = o->serialize;
    return 0;
}

static
int flecs_meta_serialize_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsStruct *ptr = ecs_get(world, type, EcsStruct);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur, first = ecs_vec_count(ops);
    ecs_meta_op_t *op = flecs_meta_ops_add(ops, EcsOpPushStruct);
    op->offset = offset;
    op->type = type;
    op->type_info = ecs_get_type_info(world, type);
    ecs_assert(op->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_member_t *members = ecs_vec_first(&ptr->members);
    int32_t i, count = ecs_vec_count(&ptr->members);

    ecs_hashmap_t *member_index = NULL;
    if (count) {        
        op->is.members = member_index = flecs_name_index_new(&world->allocator);
    }

    for (i = 0; i < count; i ++) {
        ecs_member_t *member = &members[i];

        cur = ecs_vec_count(ops);

        if (member->count >= 1) {
            if (flecs_meta_serialize_array_inline(
                world, member->type, member->count, member->offset, ops))
            {
                continue;
            }

            op = flecs_meta_ops_get(ops, cur);
        } else {
            if (flecs_meta_serialize_type(
                world, member->type, member->offset, ops)) 
            {
                continue;
            }

            op = flecs_meta_ops_get(ops, cur);
            if (!op->type) {
                op->type = member->type;
            }

            op->op_count = flecs_ito(int16_t, ecs_vec_count(ops) - cur);
        }

        const char *member_name = member->name;
        op->name = member_name;
        op->member_index = flecs_ito(int16_t, i);

        flecs_name_index_ensure(
            member_index, flecs_ito(uint64_t, cur - first - 1), 
                member_name, 0, 0);
    }

    ecs_meta_op_t *pop = flecs_meta_ops_add(ops, EcsOpPop);
    pop->type = type;
    flecs_meta_ops_get(ops, first)->op_count = 
        flecs_ito(int16_t, ecs_vec_count(ops) - first);

    return 0;
}

static
int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsType *ptr = ecs_get(world, type, EcsType);
    if (!ptr) {
        char *path = ecs_get_path(world, type);
        ecs_err("missing reflection data for type '%s'", path);
        ecs_os_free(path);
        return -1;
    }

    switch(ptr->kind) {
    case EcsPrimitiveType: return flecs_meta_serialize_primitive(world, type, offset, ops);
    case EcsEnumType: return flecs_meta_serialize_enum(world, type, offset, ops);
    case EcsBitmaskType: return flecs_meta_serialize_bitmask(world, type, offset, ops);
    case EcsStructType: return flecs_meta_serialize_struct(world, type, offset, ops);
    case EcsArrayType: return flecs_meta_serialize_forward(world, type, offset, ops);
    case EcsVectorType: return flecs_meta_serialize_forward(world, type, offset, ops);
    case EcsOpaqueType: return flecs_meta_serialize_opaque_type(world, type, offset, ops);
    }

    return 0;
}

void flecs_meta_type_serializer_init(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t type = it->entities[i];
        ecs_vec_t ops;
        ecs_vec_init_t(NULL, &ops, ecs_meta_op_t, 0);

        const EcsType *type_ptr = ecs_get(world, type, EcsType);
        if (!type_ptr) {
            char *path = ecs_get_path(world, type);
            ecs_err("missing reflection data for type '%s'", path);
            ecs_os_free(path);
            continue;
        }

        if (type_ptr->kind == EcsArrayType) {
            flecs_meta_serialize_array_type(world, type, &ops);
        } else if (type_ptr->kind == EcsVectorType) {
            flecs_meta_serialize_vector_type(world, type, &ops);
        } else {
            flecs_meta_serialize_type(world, type, 0, &ops);
        }

        EcsTypeSerializer *ptr = ecs_ensure(world, type, EcsTypeSerializer);
        if (ptr->ops.array) {
            flecs_type_serializer_dtor(ptr);
        }

        ptr->kind = type_ptr->kind;
        ptr->ops = ops;
    }
}

char* ecs_meta_serializer_to_str(
    ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsTypeSerializer *ptr = ecs_get(
        world, type, EcsTypeSerializer);
    if (!ptr) {
        return NULL;
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_meta_op_t *ops = ecs_vec_first(&ptr->ops);
    int32_t i, count = ecs_vec_count(&ptr->ops);
    for (i = 0; i < count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        const char *kind_str = flecs_meta_op_kind_str(op->kind);
        ecs_strbuf_append(&buf, "%s", kind_str);
        ecs_strbuf_append(&buf, "%*s", 10 - ecs_os_strlen(kind_str), " ");

        if (op->type) {
            char *type_str = ecs_id_str(world, op->type);
            ecs_strbuf_append(&buf, "%s ", type_str);
            ecs_os_free(type_str);
        }

        if (op->name) {
            ecs_strbuf_append(&buf, "%s: ", op->name);
        }

        ecs_strbuf_appendstr(&buf, "{ ");

        ecs_strbuf_append(&buf, "offset: %d, size: %d ", 
            op->offset, op->type_info ? op->type_info->size : 0);

        ecs_strbuf_append(&buf, ", op_count = %d ", 
            op->op_count);

        ecs_strbuf_appendstr(&buf, "}");

        ecs_strbuf_appendlit(&buf, "\n");
    }

    return ecs_strbuf_get(&buf);
}

#endif
