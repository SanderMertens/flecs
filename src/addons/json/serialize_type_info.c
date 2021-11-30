#include "json.h"

#ifdef FLECS_JSON

static
int json_typeinfo_ser_primitive(
    ecs_primitive_kind_t kind,
    ecs_strbuf_t *str) 
{
    switch(kind) {
    case EcsBool:
        json_string(str, "bool");
        break;
    case EcsChar:
    case EcsString:
        json_string(str, "text");
        break;
    case EcsByte:
        json_string(str, "byte");
        break;
    case EcsU8:
    case EcsU16:
    case EcsU32:
    case EcsU64:
    case EcsI8:
    case EcsI16:
    case EcsI32:
    case EcsI64:
    case EcsIPtr:
    case EcsUPtr:
        json_string(str, "int");
        break;
    case EcsF32:
    case EcsF64:
        json_string(str, "float");
        break;
    case EcsEntity:
        json_string(str, "entity");
        break;
    default:
        return -1;
    }

    return 0;
}

static
int json_typeinfo_ser_constants(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_pair(EcsChildOf, type)
    });

    json_array_push(str);

    while (ecs_term_next(&it)) {
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            json_next(str);
            json_string(str, ecs_get_name(world, it.entities[i]));
        }
    }

    json_array_pop(str);

    return 0;
}

/* Forward serialization to the different type kinds */
static
int json_typeinfo_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    ecs_strbuf_t *str) 
{
    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpEnum:
        json_object_push(str);
        json_member(str, "enum");
        if (json_typeinfo_ser_constants(world, op->type, str)) {
            goto error;
        }
        json_object_pop(str);
        break;
    case EcsOpBitmask:
        json_object_push(str);
        json_member(str, "bitmask");
        if (json_typeinfo_ser_constants(world, op->type, str)) {
            goto error;
        }
        json_object_pop(str);
        break;
    case EcsOpArray:
    case EcsOpVector:
        json_string(str, "array");
        break;
    default:
        if (json_typeinfo_ser_primitive( 
            json_op_to_primitive_kind(op->kind), str))
        {
            /* Unknown operation */
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
            return -1;
        }
        break;
    }

    return 0;
error:
    return -1;
}

/* Iterate over a slice of the type ops array */
static
int json_typeinfo_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    ecs_strbuf_t *str) 
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (op != ops) {
            if (op->name) {
                json_member(str, op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1 && op != ops) {
                json_string(str, "array");
                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            json_object_push(str);
            break;
        case EcsOpPop:
            json_object_pop(str);
            break;
        default:
            if (json_typeinfo_ser_type_op(world, op, str)) {
                goto error;
            }
            break;
        }
    }

    return 0;
error:
    return -1;
}

int ecs_type_info_to_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    if (!comp) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("cannot serialize to JSON, '%s' is not a component", path);
        ecs_os_free(path);
        return -1;
    }

    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    if (!ser) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
        ecs_os_free(path);
        return -1;
    }

    ecs_meta_type_op_t *ops = ecs_vector_first(ser->ops, ecs_meta_type_op_t);
    int32_t count = ecs_vector_count(ser->ops);

    return json_typeinfo_ser_type_ops(world, ops, count, buf);
}

char* ecs_type_info_to_json(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_type_info_to_buf(world, type, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

#endif
