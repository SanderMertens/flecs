/**
 * @file addons/script/serialize_type.c
 * @brief Serialize type definitions to script.
 */

#include "flecs.h"
#include <inttypes.h>

#ifdef FLECS_SCRIPT
#include "script.h"

static bool flecs_type_to_script_is_builtin(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_entity_t cur = type;
    while (cur) {
        if (cur == EcsFlecs) {
            return true;
        }

        cur = ecs_get_parent(world, cur);
    }

    return false;
}

static void flecs_type_to_script_ref(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    if (ecs_get_parent(world, type) == FlecsMeta) {
        ecs_strbuf_appendstr(buf, ecs_get_name(world, type));
        return;
    }

    char *path = ecs_get_path(world, type);
    ecs_strbuf_appendstr(buf, path);
    ecs_os_free(path);
}

static bool flecs_type_to_script_is_unsigned(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    if (!p) {
        return false;
    }

    return p->kind == EcsU8 || p->kind == EcsU16 || p->kind == EcsU32 ||
        p->kind == EcsU64 || p->kind == EcsUPtr || p->kind == EcsByte ||
        p->kind == EcsChar;
}

static int flecs_type_to_script_struct(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_strbuf_appendlit(buf, "struct ");
    ecs_strbuf_appendstr(buf, ecs_get_name(world, type));

    int32_t first = 0;
    ecs_entity_t base = ecs_get_target(world, type, EcsIsA, 0);
    const EcsStruct *base_st = base ? ecs_get(world, base, EcsStruct) : NULL;
    if (base_st) {
        ecs_strbuf_appendlit(buf, " : ");
        flecs_type_to_script_ref(world, base, buf);
        first = ecs_vec_count(&base_st->members);
    }

    ecs_strbuf_appendch(buf, '(');

    const ecs_member_t *members = ecs_vec_first(&st->members);
    int32_t i, count = ecs_vec_count(&st->members);
    for (i = first; i < count; i ++) {
        const ecs_member_t *m = &members[i];
        if (i != first) {
            ecs_strbuf_appendlit(buf, ", ");
        }

        ecs_strbuf_appendstr(buf, m->name);
        ecs_strbuf_appendlit(buf, ": ");

        bool has_unit = m->unit && m->unit != m->type;
        if (m->count > 1 || has_unit) {
            ecs_strbuf_appendch(buf, '{');
            flecs_type_to_script_ref(world, m->type, buf);
            if (m->count > 1) {
                ecs_strbuf_append(buf, ", count: %d", m->count);
            }

            if (has_unit) {
                ecs_strbuf_appendlit(buf, ", unit: ");
                flecs_type_to_script_ref(world, m->unit, buf);
            }

            ecs_strbuf_appendch(buf, '}');
        } else {
            flecs_type_to_script_ref(world, m->type, buf);
        }
    }

    ecs_strbuf_appendch(buf, ')');
    return 0;
}

static int flecs_type_to_script_constants(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf,
    bool is_bitmask)
{
    const EcsConstants *c = ecs_get(world, type, EcsConstants);
    if (!c) {
        ecs_err("type '%s' has no constants",
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }

    ecs_entity_t underlying = 0;
    if (!is_bitmask) {
        const EcsEnum *e = ecs_get(world, type, EcsEnum);
        underlying = e ? e->underlying_type : 0;
    }

    bool is_unsigned = is_bitmask ||
        flecs_type_to_script_is_unsigned(world, underlying);

    ecs_strbuf_appendstr(buf, is_bitmask ? "bitmask " : "enum ");
    ecs_strbuf_appendstr(buf, ecs_get_name(world, type));
    ecs_strbuf_appendch(buf, '(');

    const ecs_enum_constant_t *constants = ecs_vec_first(&c->ordered_constants);
    int32_t i, count = ecs_vec_count(&c->ordered_constants);
    for (i = 0; i < count; i ++) {
        if (i) {
            ecs_strbuf_appendlit(buf, ", ");
        }

        ecs_strbuf_appendstr(buf, constants[i].name);
        if (is_bitmask) {
            const ecs_bitmask_constant_t *bc =
                (const ecs_bitmask_constant_t*)&constants[i];
            ecs_strbuf_append(buf, ": %" PRIu64, (uint64_t)bc->value);
        } else if (is_unsigned) {
            ecs_strbuf_append(buf, ": %" PRIu64, constants[i].value_unsigned);
        } else {
            ecs_strbuf_append(buf, ": %" PRId64, constants[i].value);
        }
    }

    if (underlying && underlying != ecs_id(ecs_i32_t)) {
        if (count) {
            ecs_strbuf_appendlit(buf, ", ");
        }

        ecs_strbuf_appendlit(buf, "{underlying_type: ");
        flecs_type_to_script_ref(world, underlying, buf);
        ecs_strbuf_appendch(buf, '}');
    }

    ecs_strbuf_appendch(buf, ')');
    return 0;
}

static int flecs_type_to_script_decl(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    const EcsType *t = ecs_get(world, type, EcsType);
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);

    switch(t->kind) {
    case EcsStructType:
        return flecs_type_to_script_struct(world, type, buf);
    case EcsEnumType:
        return flecs_type_to_script_constants(world, type, buf, false);
    case EcsBitmaskType:
        return flecs_type_to_script_constants(world, type, buf, true);
    case EcsVectorType: {
        const EcsVector *v = ecs_get(world, type, EcsVector);
        ecs_strbuf_append(buf, "vector %s(type: ", ecs_get_name(world, type));
        flecs_type_to_script_ref(world, v->type, buf);
        ecs_strbuf_appendch(buf, ')');
        return 0;
    }
    case EcsArrayType: {
        const EcsArray *a = ecs_get(world, type, EcsArray);
        ecs_strbuf_append(buf, "array %s(type: ", ecs_get_name(world, type));
        flecs_type_to_script_ref(world, a->type, buf);
        ecs_strbuf_append(buf, ", count: %d)", a->count);
        return 0;
    }
    case EcsMapType: {
        const EcsMap *m = ecs_get(world, type, EcsMap);
        ecs_strbuf_append(buf, "map %s(key_type: ", ecs_get_name(world, type));
        flecs_type_to_script_ref(world, m->key_type, buf);
        ecs_strbuf_appendlit(buf, ", type: ");
        flecs_type_to_script_ref(world, m->type, buf);
        ecs_strbuf_appendch(buf, ')');
        return 0;
    }
    case EcsPrimitiveType:
    case EcsOpaqueType:
    case EcsValueType:
    default: {
        ecs_err("type '%s' cannot be declared in a script",
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }
    }
}

static int flecs_type_to_script_emit(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf)
{
    ecs_entity_t parents[64];
    int32_t i, depth = 0;
    ecs_entity_t parent = ecs_get_parent(world, type);
    while (parent && depth < 64) {
        parents[depth ++] = parent;
        parent = ecs_get_parent(world, parent);
    }

    for (i = depth - 1; i >= 0; i --) {
        ecs_strbuf_append(buf, "%*s%s {\n", (depth - 1 - i) * 2, "",
            ecs_get_name(world, parents[i]));
    }

    ecs_strbuf_append(buf, "%*s", depth * 2, "");
    if (flecs_type_to_script_decl(world, type, buf)) {
        return -1;
    }

    ecs_strbuf_appendch(buf, '\n');

    for (i = 0; i < depth; i ++) {
        ecs_strbuf_append(buf, "%*s}\n", (depth - 1 - i) * 2, "");
    }

    return 0;
}

static int flecs_type_to_script_visit(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *visited,
    ecs_strbuf_t *buf);

static int flecs_type_to_script_dep(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *visited,
    ecs_strbuf_t *buf)
{
    if (!type || flecs_type_to_script_is_builtin(world, type)) {
        return 0;
    }

    const EcsType *t = ecs_get(world, type, EcsType);
    if (!t || t->kind == EcsPrimitiveType || t->kind == EcsOpaqueType) {
        return 0;
    }

    return flecs_type_to_script_visit(world, type, visited, buf);
}

static int flecs_type_to_script_visit(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *visited,
    ecs_strbuf_t *buf)
{
    const ecs_entity_t *v = ecs_vec_first(visited);
    int32_t i, count = ecs_vec_count(visited);
    for (i = 0; i < count; i ++) {
        if (v[i] == type) {
            return 0;
        }
    }

    ecs_vec_append_t(NULL, visited, ecs_entity_t)[0] = type;

    const EcsType *t = ecs_get(world, type, EcsType);
    if (!t) {
        ecs_err("entity '%s' is not a type",
            flecs_errstr(ecs_get_path(world, type)));
        return -1;
    }

    ecs_entity_t deps[2] = {0};
    switch(t->kind) {
    case EcsStructType: {
        ecs_entity_t base = ecs_get_target(world, type, EcsIsA, 0);
        if (base && ecs_has(world, base, EcsStruct) &&
            flecs_type_to_script_dep(world, base, visited, buf))
        {
            return -1;
        }

        const EcsStruct *st = ecs_get(world, type, EcsStruct);
        const ecs_member_t *members = ecs_vec_first(&st->members);
        count = ecs_vec_count(&st->members);
        for (i = 0; i < count; i ++) {
            if (flecs_type_to_script_dep(
                world, members[i].type, visited, buf))
            {
                return -1;
            }
        }
        break;
    }
    case EcsVectorType:
        deps[0] = ecs_get(world, type, EcsVector)->type;
        break;
    case EcsArrayType:
        deps[0] = ecs_get(world, type, EcsArray)->type;
        break;
    case EcsMapType:
        deps[0] = ecs_get(world, type, EcsMap)->key_type;
        deps[1] = ecs_get(world, type, EcsMap)->type;
        break;
    case EcsPrimitiveType:
    case EcsBitmaskType:
    case EcsEnumType:
    case EcsOpaqueType:
    case EcsValueType:
    default:
        break;
    }

    if (flecs_type_to_script_dep(world, deps[0], visited, buf) ||
        flecs_type_to_script_dep(world, deps[1], visited, buf))
    {
        return -1;
    }

    return flecs_type_to_script_emit(world, type, buf);
}

char* ecs_type_to_script(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(type != 0, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_vec_t visited = {0};
    ecs_vec_init_t(NULL, &visited, ecs_entity_t, 0);
    int result = flecs_type_to_script_visit(world, type, &visited, &buf);
    ecs_vec_fini_t(NULL, &visited, ecs_entity_t);
    if (result) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

#endif
