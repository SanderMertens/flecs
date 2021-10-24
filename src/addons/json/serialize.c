
#include "../../private_api.h"

#ifdef FLECS_JSON


static
int json_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
int json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str);

static
int json_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

static
ecs_primitive_kind_t json_op_to_primitive_kind(ecs_meta_type_op_kind_t kind) {
    return kind - EcsOpPrimitive;
}

/* Serialize enumeration */
static
int json_ser_enum(
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
    ecs_enum_constant_t *constant = ecs_map_get(
        enum_type->constants, ecs_enum_constant_t, value);
    if (!constant) {
        return -1;
    }

    ecs_strbuf_appendstr(str, "\"");
    ecs_strbuf_appendstr(str, ecs_get_name(world, constant->constant));
    ecs_strbuf_appendstr(str, "\"");

    return 0;
}

/* Serialize bitmask */
static
int json_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_assert(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    uint32_t value = *(uint32_t*)ptr;
    ecs_map_key_t key;
    ecs_bitmask_constant_t *constant;

    if (!value) {
        ecs_strbuf_appendstr(str, "0");
        return 0;
    }

    ecs_strbuf_list_push(str, "\"", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(bitmask_type->constants);
    while ((constant = ecs_map_next(&it, ecs_bitmask_constant_t, &key))) {
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, 
                ecs_get_name(world, constant->constant));
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        /* All bits must have been matched by a constant */
        return -1;
    }

    ecs_strbuf_list_pop(str, "\"");

    return 0;
}

/* Serialize elements of a contiguous array */
static
int json_ser_elements(
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
        if (json_ser_type_ops(world, ops, op_count, ptr, str)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    ecs_strbuf_list_pop(str, "]");

    return 0;
}

static
int json_ser_type_elements(
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

    return json_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, str);
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

    return json_ser_type_elements(
        world, a->type, ptr, a->count, str);
}

/* Serialize vector */
static
int json_ser_vector(
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
    return json_ser_type_elements(world, v->type, array, count, str);
}

/* Forward serialization to the different type kinds */
static
int json_ser_type_op(
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
        if (json_ser_enum(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpBitmask:
        if (json_ser_bitmask(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpArray:
        if (json_ser_array(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpVector:
        if (json_ser_vector(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            return -1;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = *(ecs_entity_t*)ptr;
        if (!e) {
            ecs_strbuf_appendstr(str, "0");
        } else {
            char *path = ecs_get_fullpath(world, e);
            ecs_assert(path != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_strbuf_append(str, "\"%s\"", path);
            ecs_os_free(path);
        }
        break;
    }

    default:
        if (ecs_primitive_to_expr_buf(world, json_op_to_primitive_kind(op->kind), 
            ECS_OFFSET(ptr, op->offset), str)) 
        {
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
int json_ser_type_ops(
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
                ecs_strbuf_append(str, "\"%s\": ", op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1 && op != ops) {
                /* Serialize inline array */
                if (json_ser_elements(world, op, op->op_count, base,
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
            if (json_ser_type_op(world, op, base, str)) {
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
int json_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_type_op_t *ops = ecs_vector_first(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vector_count(v_ops);
    return json_ser_type_ops(world, ops, count, base, str);
}

int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);

    if (json_ser_type(world, ser->ops, ptr, buf_out)) {
        return -1;
    }

    return 0;
}

char* ecs_ptr_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_ptr_to_json_buf(world, type, ptr, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

static
int append_type(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf_out, 
    ecs_entity_t ent, 
    ecs_entity_t inst) 
{
    ecs_type_t type = ecs_get_type(world, ent);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i, count = ecs_vector_count(type);

    ecs_strbuf_list_appendstr(buf_out, "\"type\": ");
    ecs_strbuf_list_push(buf_out, "[", ", ");

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        ecs_entity_t pred = 0, obj = 0, role = 0;

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* Skip IsA as they are already added in "inherits" */
            continue;
        }

        if (ent != inst) {
            /* If not serializing the top level entity, skip components that are
             * never inherited from a base entity */
            if (id == ecs_pair(ecs_id(EcsIdentifier), EcsName) ||
                ECS_PAIR_RELATION(id) == EcsChildOf ||
                id == EcsPrefab)
            {
                continue;
            }
        }

        if (ECS_HAS_ROLE(id, PAIR)) {
            pred = ecs_pair_relation(world, id);
            obj = ecs_pair_object(world, id);
        } else {
            pred = id & ECS_COMPONENT_MASK;
            if (id & ECS_ROLE_MASK) {
                role = id & ECS_ROLE_MASK;
            }
        }

        ecs_strbuf_list_next(buf_out);
        ecs_strbuf_list_push(buf_out, "{", ", ");

        if (pred) {
            char *str = ecs_get_fullpath(world, pred);
            ecs_strbuf_list_append(buf_out, "\"pred\":\"%s\"", str);
            ecs_os_free(str);
        }
        if (obj) {
            char *str = ecs_get_fullpath(world, obj);
            ecs_strbuf_list_append(buf_out, "\"obj\":\"%s\"", str);
            ecs_os_free(str);
        }
        if (role) {
            ecs_strbuf_list_append(buf_out, "\"obj\":\"%s\"", 
                ecs_role_str(role));
        }

        bool hidden = false;

        if (ent != inst) {
            if (ecs_get_object_for_id(world, inst, EcsIsA, id) != ent) {
                hidden = true;
                ecs_strbuf_list_append(buf_out, "\"hidden\": true", 
                    ecs_role_str(role));
            }
        }

        if (!hidden) {
            ecs_entity_t typeid = ecs_get_typeid(world, id);
            if (typeid) {
                const EcsMetaTypeSerialized *ser = ecs_get(
                    world, typeid, EcsMetaTypeSerialized);
                if (ser) {
                    const void *ptr = ecs_get_id(world, ent, id);
                    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_strbuf_list_appendstr(buf_out, "\"data\": ");
                    if (json_ser_type(world, ser->ops, ptr, buf_out) != 0) {
                        /* Entity contains invalid value */
                        return -1;
                    }
                }
            }
        }

        ecs_strbuf_list_pop(buf_out, "}");
    }

    ecs_strbuf_list_pop(buf_out, "]");
    
    return 0;
}

static
int append_base(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf_out, 
    ecs_entity_t ent, 
    ecs_entity_t inst) 
{
    ecs_type_t type = ecs_get_type(world, ent);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            if (append_base(world, buf_out, ecs_pair_object(world, id), inst)) {
                return -1;
            }
        }
    }

    char *path = ecs_get_fullpath(world, ent);
    ecs_strbuf_list_append(buf_out, "\"%s\": ", path);
    ecs_os_free(path);

    ecs_strbuf_list_push(buf_out, "{", ", ");
    if (append_type(world, buf_out, ent, inst)) {
        return -1;
    }
    ecs_strbuf_list_pop(buf_out, "}");

    return 0;
}

int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf_out)
{
    ecs_strbuf_list_push(buf_out, "{", ", ");

    if (!entity || !ecs_is_valid(world, entity)) {
        ecs_strbuf_list_appendstr(buf_out, "\"valid\": false");
    } else {
        ecs_strbuf_list_appendstr(buf_out, "\"valid\": true");

        char *path = ecs_get_fullpath(world, entity);
        ecs_strbuf_list_append(buf_out, "\"path\": \"%s\"", path);
        ecs_os_free(path);

        ecs_type_t type = ecs_get_type(world, entity);
        ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
        int32_t i, count = ecs_vector_count(type);

        if (ecs_has_pair(world, entity, EcsIsA, EcsWildcard)) {
            ecs_strbuf_list_appendstr(buf_out, "\"is_a\": ");
            ecs_strbuf_list_push(buf_out, "{", ", ");

            for (i = 0; i < count; i ++) {
                ecs_id_t id = ids[i];
                if (ECS_HAS_RELATION(id, EcsIsA)) {
                    if (append_base(
                        world, buf_out, ecs_pair_object(world, id), entity)) 
                    {
                        return -1;
                    }
                }
            }

            ecs_strbuf_list_pop(buf_out, "}");
        }

        if (append_type(world, buf_out, entity, entity)) {
            goto error;
        }
    }

    ecs_strbuf_list_pop(buf_out, "}");

    return 0;
error:
    return -1;
}

char* ecs_entity_to_json(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_entity_to_json_buf(world, entity, &buf) != 0) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
