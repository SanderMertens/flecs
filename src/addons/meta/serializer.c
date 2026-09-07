/**
 * @file addons/meta/serializer.c
 * @brief Build instruction list for serializing/deserializing types.
 */

#include "meta.h"

#ifdef FLECS_META

static int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops,
    bool root);

static int flecs_meta_serialize_scope(
    ecs_world_t *world,
    ecs_meta_op_t op,
    ecs_entity_t element,
    int32_t array_count,
    ecs_vec_t *ops)
{
    int32_t first = ecs_vec_count(ops);
    *ecs_vec_append_t(NULL, ops, ecs_meta_op_t) = op;
    if (op.kind == EcsOpPushStruct) {
        const EcsStruct *st = ecs_get(world, op.type, EcsStruct);
        ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_member_t *members = ecs_vec_first(&st->members);
        int32_t count = ecs_vec_count(&st->members);
        ecs_hashmap_t *index = count ?
            flecs_name_index_new(&world->allocator) : NULL;
        ecs_vec_get_t(ops, ecs_meta_op_t, first)->is.members = index;
        for (int32_t i = 0; i < count; i ++) {
            ecs_member_t *m = &members[i];
            int32_t cur = ecs_vec_count(ops);
            int ret;
            if (m->count >= 1) {
                ecs_meta_op_t array = {
                    .kind = EcsOpPushArray,
                    .type = m->type,
                    .offset = m->offset,
                    .elem_size = flecs_type_size(world, m->type)
                };
                ret = flecs_meta_serialize_scope(
                    world, array, m->type, m->count, ops);
            } else {
                ret = flecs_meta_serialize_type(
                    world, m->type, m->offset, ops, false);
            }
            if (ret) {
                continue;
            }
            ecs_meta_op_t *member_op = ecs_vec_get_t(ops, ecs_meta_op_t, cur);
            if (!member_op->type) {
                member_op->type = m->type;
            }
            member_op->op_count = flecs_ito(int16_t, ecs_vec_count(ops) - cur);
            member_op->name = ecs_os_strdup(m->name);
            member_op->member_index = flecs_ito(int16_t, i);
            flecs_name_index_ensure(index, flecs_ito(uint64_t, cur - first - 1),
                member_op->name, 0, 0);
        }
    } else if (element && flecs_meta_serialize_type(
        world, element, 0, ops, false))
    {
        return -1;
    }
    ecs_meta_op_t *pop = ecs_vec_append_t(NULL, ops, ecs_meta_op_t);
    *pop = (ecs_meta_op_t){
        .kind = EcsOpPop,
        .op_count = 1,
        .type = op.type,
        .type_info = op.kind == EcsOpPushStruct ? NULL : op.type_info,
        .elem_size = array_count
    };
    ecs_vec_get_t(ops, ecs_meta_op_t, first)->op_count =
        flecs_ito(int16_t, ecs_vec_count(ops) - first);
    return 0;
}

static int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops,
    bool root)
{
    const EcsType *t = ecs_get(world, type, EcsType);
    if (!t) {
        ecs_err("missing reflection data for type '%s'",
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }
    ecs_meta_op_t op = {
        .kind = EcsOpForward,
        .op_count = 1,
        .type = type,
        .type_info = ecs_get_type_info(world, type),
        .offset = offset
    };
    ecs_assert(op.type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t element = 0;
    int32_t array_count = 0;
    switch (t->kind) {
    case EcsPrimitiveType: {
        const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
        if (!p) {
            ecs_err("entity '%s' is not a primitive type",
                flecs_errstr(ecs_get_path(world, type)));
            return -1;
        }
        op.kind = flecs_meta_primitive_to_op_kind(p->kind);
        break;
    }
    case EcsEnumType:
    case EcsBitmaskType: {
        const EcsConstants *c = ecs_get(world, type, EcsConstants);
        ecs_assert(c != NULL, ECS_INVALID_PARAMETER, NULL);
        op.is.constants = c->constants;
        ecs_assert(op.is.constants != NULL, ECS_INTERNAL_ERROR, NULL);
        op.kind = EcsOpBitmask;
        if (t->kind == EcsEnumType) {
            const EcsEnum *e = ecs_get(world, type, EcsEnum);
            ecs_assert(e != NULL, ECS_INVALID_PARAMETER, NULL);
            const EcsPrimitive *p = ecs_get(
                world, e->underlying_type, EcsPrimitive);
            ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert((p->kind >= EcsU8 && p->kind <= EcsI64) ||
                p->kind == EcsUPtr || p->kind == EcsIPtr,
                ECS_INTERNAL_ERROR, "invalid underlying enum type");
            op.kind = EcsOpEnum;
            op.underlying_kind = flecs_meta_primitive_to_op_kind(p->kind);
        }
        break;
    }
    case EcsStructType:
        op.kind = EcsOpPushStruct;
        break;
    case EcsArrayType:
        if (root) {
            const EcsArray *a = ecs_get(world, type, EcsArray);
            if (!a) {
                return -1;
            }
            op.kind = EcsOpPushArray;
            op.type = element = a->type;
            op.type_info = NULL;
            array_count = a->count;
        }
        break;
    case EcsVectorType:
        if (root) {
            const EcsVector *v = ecs_get(world, type, EcsVector);
            if (!v) {
                return -1;
            }
            op.kind = EcsOpPushVector;
            element = v->type;
        }
        break;
    case EcsMapType:
        if (root) {
            const EcsMap *m = ecs_get(world, type, EcsMap);
            if (!m) {
                return -1;
            }
            const EcsTypeSerializer *key = ecs_get(
                world, m->key_type, EcsTypeSerializer);
            if (!key || !ecs_vec_count(&key->ops)) {
                return -1;
            }
            const ecs_meta_op_t *key_op = ecs_vec_first(&key->ops);
            op.kind = EcsOpPushMap;
            op.underlying_kind = key_op->kind;
            op.is.constants = key_op->is.constants;
            element = m->type;
        }
        break;
    case EcsValueType:
        if (root) {
            op.kind = EcsOpPushValue;
        }
        break;
    case EcsOpaqueType: {
        const EcsOpaque *o = ecs_get(world, type, EcsOpaque);
        ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
        const EcsType *as = ecs_get(world, o->as_type, EcsType);
        ecs_assert(as != NULL, ECS_INTERNAL_ERROR,
            "missing reflection for Opaque::as_type");
        op.kind = EcsOpOpaqueValue;
        if (as->kind == EcsArrayType) {
            op.kind = EcsOpOpaqueArray;
        } else if (as->kind == EcsVectorType) {
            op.kind = EcsOpOpaqueVector;
        } else if (as->kind == EcsStructType) {
            op.kind = EcsOpOpaqueStruct;
        }
        op.is.opaque = o->serialize;
        break;
    }
    }
    if (op.kind == EcsOpPushStruct || op.kind == EcsOpPushArray ||
        op.kind == EcsOpPushVector || op.kind == EcsOpPushMap ||
        op.kind == EcsOpPushValue)
    {
        if (element) {
            op.elem_size = flecs_type_size(world, element);
        }
        return flecs_meta_serialize_scope(world, op, element, array_count, ops);
    }
    *ecs_vec_append_t(NULL, ops, ecs_meta_op_t) = op;
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

        int ret = flecs_meta_serialize_type(world, type, 0, &ops, true);

        if (ret != 0) {
            ecs_vec_fini_t(NULL, &ops, ecs_meta_op_t);
            continue;
        }

        EcsTypeSerializer *ptr = ecs_ensure(world, type, EcsTypeSerializer);
        if (ptr->ops.array) {
            flecs_type_serializer_dtor(ptr);
        }

        ptr->kind = type_ptr->kind;
        ptr->ops = ops;

        ecs_modified(world, type, EcsTypeSerializer);
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
