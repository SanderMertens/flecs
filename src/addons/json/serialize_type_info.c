/**
 * @file addons/json/serialize_type_info.c
 * @brief Serialize type (reflection) information to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

static int flecs_json_typeinfo_ser_type(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf);

static int flecs_json_typeinfo_ser_primitive(
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

    ecs_strbuf_list_next(str);

    return 0;
}

static void flecs_json_typeinfo_ser_constants(
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

/* Serialize unit information */
static int flecs_json_typeinfo_ser_unit(
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

static void flecs_json_typeinfo_ser_range(
    ecs_strbuf_t *str,
    const char *kind,
    const ecs_member_value_range_t *range)
{
    flecs_json_member(str, kind);
    flecs_json_array_push(str);
    flecs_json_next(str);
    flecs_json_number(str, range->min);
    flecs_json_next(str);
    flecs_json_number(str, range->max);
    flecs_json_array_pop(str);
}

static int flecs_json_typeinfo_ser_op(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    const ecs_member_t *member,
    ecs_strbuf_t *str);

static int flecs_json_typeinfo_ser_scope(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    ecs_strbuf_t *str)
{
    const EcsStruct *st = op->kind == EcsOpPushStruct
        ? ecs_get(world, op->type, EcsStruct) : NULL;
    for (int32_t i = 1; i < op->op_count - 1; ) {
        const ecs_meta_op_t *child = &op[i];
        const ecs_member_t *member = NULL;
        if (child->name) {
            flecs_json_member(str, child->name);
            member = ecs_vec_get_t(&st->members, ecs_member_t, child->member_index);
        }
        if (flecs_json_typeinfo_ser_op(world, child, member, str)) {
            return -1;
        }
        i += child->op_count;
    }
    ecs_entity_t unit = ecs_get_target_for(world, op->type, EcsIsA, EcsUnit);
    if (unit) {
        flecs_json_member(str, "@self");
        flecs_json_array_push(str);
        flecs_json_object_push(str);
        flecs_json_typeinfo_ser_unit(world, str, unit);
        flecs_json_object_pop(str);
        flecs_json_array_pop(str);
    }
    return 0;
}

static int flecs_json_typeinfo_ser_op(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    const ecs_member_t *member,
    ecs_strbuf_t *str)
{
    if (op->kind == EcsOpForward) {
        return flecs_json_typeinfo_ser_type(world, op->type, str);
    }
    if (op->kind == EcsOpOpaqueStruct || op->kind == EcsOpOpaqueArray ||
        op->kind == EcsOpOpaqueVector || op->kind == EcsOpOpaqueValue)
    {
        const EcsOpaque *ct = ecs_get(world, op->type, EcsOpaque);
        return flecs_json_typeinfo_ser_type(world, ct->as_type, str);
    }
    if (op->kind == EcsOpPushStruct) {
        flecs_json_object_push(str);
        if (flecs_json_typeinfo_ser_scope(world, op, str)) {
            return -1;
        }
        flecs_json_object_pop(str);
        return 0;
    }

    flecs_json_array_push(str);
    switch (op->kind) {
    case EcsOpPushMap: {
        ecs_strbuf_list_appendstr(str, "\"map\"");
        flecs_json_next(str);
        ecs_meta_op_t key = { .kind = op->underlying_kind };
        if (key.kind == EcsOpEnum || key.kind == EcsOpBitmask) {
            key.type = ecs_get(world, op->type, EcsMap)->key_type;
        }
        if (flecs_json_typeinfo_ser_op(world, &key, NULL, str)) {
            return -1;
        }
        flecs_json_next(str);
        if (flecs_json_typeinfo_ser_scope(world, op, str)) {
            return -1;
        }
        break;
    }
    case EcsOpPushArray:
    case EcsOpPushVector:
        ecs_strbuf_list_appendstr(str,
            op->kind == EcsOpPushArray ? "\"array\"" : "\"vector\"");
        flecs_json_next(str);
        if (flecs_json_typeinfo_ser_scope(world, op, str)) {
            return -1;
        }
        if (op->kind == EcsOpPushArray) {
            ecs_strbuf_list_append(str, "%u", ecs_meta_op_get_elem_count(op, NULL));
        }
        break;
    case EcsOpPushValue:
        ecs_strbuf_list_appendstr(str, "\"value\"");
        break;
    case EcsOpEnum:
    case EcsOpBitmask:
        ecs_strbuf_list_appendstr(str,
            op->kind == EcsOpEnum ? "\"enum\"" : "\"bitmask\"");
        flecs_json_typeinfo_ser_constants(world, op->type, str);
        break;
    default:
        if (flecs_json_typeinfo_ser_primitive(
            flecs_json_op_to_primitive_kind(op->kind), str))
        {
            return -1;
        }
        break;
    }

    if (member) {
        bool value_range = ECS_NEQ(member->range.min, member->range.max);
        bool error_range = ECS_NEQ(member->error_range.min, member->error_range.max);
        bool warning_range = ECS_NEQ(member->warning_range.min, member->warning_range.max);
        if (member->unit || value_range || error_range || warning_range) {
            flecs_json_next(str);
            flecs_json_object_push(str);
            if (member->unit) {
                flecs_json_typeinfo_ser_unit(world, str, member->unit);
            }
            if (value_range) {
                flecs_json_typeinfo_ser_range(str, "range", &member->range);
            }
            if (error_range) {
                flecs_json_typeinfo_ser_range(str, "error_range", &member->error_range);
            }
            if (warning_range) {
                flecs_json_typeinfo_ser_range(str, "warning_range", &member->warning_range);
            }
            flecs_json_object_pop(str);
        }
    }
    flecs_json_array_pop(str);
    return 0;
}

static int flecs_json_typeinfo_ser_type(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    const EcsTypeSerializer *ser = ecs_get(world, type, EcsTypeSerializer);
    if (!ecs_has(world, type, EcsComponent) || !ser) {
        ecs_strbuf_appendch(buf, '0');
        return 0;
    }
    return flecs_json_typeinfo_ser_op(world, ecs_vec_first(&ser->ops), NULL, buf);
}

int ecs_type_info_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    return flecs_json_typeinfo_ser_type(world, type, buf);
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
