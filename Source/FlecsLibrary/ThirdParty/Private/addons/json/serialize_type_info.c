/**
 * @file addons/json/serialize_type_info.c
 * @brief Serialize type (reflection) information to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

static
int json_typeinfo_ser_type(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf);

static
int json_typeinfo_ser_primitive(
    ecs_primitive_kind_t kind,
    ecs_strbuf_t *str) 
{
    switch(kind) {
    case EcsBool:
        flecs_json_string(str, "bool");
        break;
    case EcsChar:
    case EcsString:
        flecs_json_string(str, "text");
        break;
    case EcsByte:
        flecs_json_string(str, "byte");
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
        flecs_json_string(str, "int");
        break;
    case EcsF32:
    case EcsF64:
        flecs_json_string(str, "float");
        break;
    case EcsEntity:
        flecs_json_string(str, "entity");
        break;
    case EcsId:
        flecs_json_string(str, "id");
        break;
    default:
        return -1;
    }

    return 0;
}

static
void json_typeinfo_ser_constants(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    ecs_iter_t it = ecs_each_id(world, ecs_pair(EcsChildOf, type));
    while (ecs_each_next(&it)) {
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            flecs_json_next(str);
            flecs_json_string(str, ecs_get_name(world, it.entities[i]));
        }
    }
}

static
void json_typeinfo_ser_enum(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_appendstr(str, "\"enum\"");
    json_typeinfo_ser_constants(world, type, str);
}

static
void json_typeinfo_ser_bitmask(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_appendstr(str, "\"bitmask\"");
    json_typeinfo_ser_constants(world, type, str);
}

static
int json_typeinfo_ser_array(
    const ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_appendstr(str, "\"array\"");

    flecs_json_next(str);
    if (json_typeinfo_ser_type(world, elem_type, str)) {
        goto error;
    }

    ecs_strbuf_list_append(str, "%u", count);
    return 0;
error:
    return -1;
}

static
int json_typeinfo_ser_array_type(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    const EcsArray *arr = ecs_get(world, type, EcsArray);
    ecs_assert(arr != NULL, ECS_INTERNAL_ERROR, NULL);
    if (json_typeinfo_ser_array(world, arr->type, arr->count, str)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int json_typeinfo_ser_vector(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str)
{
    const EcsVector *arr = ecs_get(world, type, EcsVector);
    ecs_assert(arr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_strbuf_list_appendstr(str, "\"vector\"");

    flecs_json_next(str);
    if (json_typeinfo_ser_type(world, arr->type, str)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

/* Serialize unit information */
static
int json_typeinfo_ser_unit(
    const ecs_world_t *world,
    ecs_strbuf_t *str,
    ecs_entity_t unit) 
{
    flecs_json_memberl(str, "unit");
    flecs_json_path(str, world, unit);

    const EcsUnit *uptr = ecs_get(world, unit, EcsUnit);
    if (uptr) {
        if (uptr->symbol) {
            flecs_json_memberl(str, "symbol");
            flecs_json_string(str, uptr->symbol);
        }
        ecs_entity_t quantity = ecs_get_target(world, unit, EcsQuantity, 0);
        if (quantity) {
            flecs_json_memberl(str, "quantity");
            flecs_json_path(str, world, quantity);
        }
    }

    return 0;
}

static
void json_typeinfo_ser_range(
    ecs_strbuf_t *str,
    const char *kind,
    ecs_member_value_range_t *range)
{
    flecs_json_member(str, kind);
    flecs_json_array_push(str);
    flecs_json_next(str);
    flecs_json_number(str, range->min);
    flecs_json_next(str);
    flecs_json_number(str, range->max);
    flecs_json_array_pop(str);
}

/* Forward serialization to the different type kinds */
static
int json_typeinfo_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    ecs_strbuf_t *str,
    const EcsStruct *st) 
{
    if (op->kind == EcsOpOpaque) {
        const EcsOpaque *ct = ecs_get(world, op->type, 
            EcsOpaque);
        ecs_assert(ct != NULL, ECS_INTERNAL_ERROR, NULL);
        return json_typeinfo_ser_type(world, ct->as_type, str);
    }

    flecs_json_array_push(str);

    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, 
            "unexpected push/pop serializer instruction");
        break;
    case EcsOpEnum:
        json_typeinfo_ser_enum(world, op->type, str);
        break;
    case EcsOpBitmask:
        json_typeinfo_ser_bitmask(world, op->type, str);
        break;
    case EcsOpArray:
        json_typeinfo_ser_array_type(world, op->type, str);
        break;
    case EcsOpVector:
        json_typeinfo_ser_vector(world, op->type, str);
        break;
    case EcsOpOpaque:
        /* Can't happen, already handled above */
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
        break;
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
        if (json_typeinfo_ser_primitive( 
            flecs_json_op_to_primitive_kind(op->kind), str))
        {
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
        }
        break;
    case EcsOpScope:
    case EcsOpPrimitive:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }

    if (st) {
        ecs_member_t *m = ecs_vec_get_t(
            &st->members, ecs_member_t, op->member_index);
        ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

        bool value_range = ECS_NEQ(m->range.min, m->range.max);
        bool error_range = ECS_NEQ(m->error_range.min, m->error_range.max);
        bool warning_range = ECS_NEQ(m->warning_range.min, m->warning_range.max);

        ecs_entity_t unit = m->unit;
        if (unit || error_range || warning_range || value_range) {
            flecs_json_next(str);
            flecs_json_next(str);
            flecs_json_object_push(str);

            if (unit) {
                json_typeinfo_ser_unit(world, str, unit);
            }
            if (value_range) {
                json_typeinfo_ser_range(str, "range", &m->range);
            }
            if (error_range) {
                json_typeinfo_ser_range(str, "error_range", &m->error_range);
            }
            if (warning_range) {
                json_typeinfo_ser_range(str, "warning_range", &m->warning_range);
            }

            flecs_json_object_pop(str);
        }
    }

    flecs_json_array_pop(str);

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
    ecs_strbuf_t *str,
    const EcsStruct *st) 
{
    const EcsStruct *stack[64] = {st};
    int32_t sp = 1;

    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (op != ops) {
            if (op->name) {
                flecs_json_member(str, op->name);
            }
        }

        int32_t elem_count = op->count;
        if (elem_count > 1) {
            flecs_json_array_push(str);
            json_typeinfo_ser_array(world, op->type, op->count, str);
            flecs_json_array_pop(str);
            i += op->op_count - 1;
            continue;
        }

        switch(op->kind) {
        case EcsOpPush:
            flecs_json_object_push(str);
            ecs_assert(sp < 63, ECS_INVALID_OPERATION, "type nesting too deep");
            stack[sp ++] = ecs_get(world, op->type, EcsStruct);
            break;
        case EcsOpPop: {
            ecs_entity_t unit = ecs_get_target_for(world, op->type, EcsIsA, EcsUnit);
            if (unit) {
                flecs_json_member(str, "@self");
                flecs_json_array_push(str);
                flecs_json_object_push(str);
                json_typeinfo_ser_unit(world, str, unit);
                flecs_json_object_pop(str);
                flecs_json_array_pop(str);
            }

            flecs_json_object_pop(str);
            sp --;
            break;
        }
        case EcsOpArray:
        case EcsOpVector:
        case EcsOpEnum:
        case EcsOpBitmask:
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
            if (json_typeinfo_ser_type_op(world, op, str, stack[sp - 1])) {
                goto error;
            }
            break;
        case EcsOpPrimitive:
        case EcsOpScope:
        default:
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
        }
    }

    return 0;
error:
    return -1;
}

static
int json_typeinfo_ser_type(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    if (!comp) {
        ecs_strbuf_appendch(buf, '0');
        return 0;
    }

    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (!ser) {
        ecs_strbuf_appendch(buf, '0');
        return 0;
    }

    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
    int32_t count = ecs_vec_count(&ser->ops);

    if (json_typeinfo_ser_type_ops(world, ops, count, buf, st)) {
        return -1;
    }

    return 0;
}

int ecs_type_info_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    return json_typeinfo_ser_type(world, type, buf);
}

char* ecs_type_info_to_json(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_type_info_to_json_buf(world, type, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

#endif
