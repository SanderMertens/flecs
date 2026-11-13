/**
 * @file addons/script/modules/typecache.c
 * @brief Cache with anonymous types used by expressions (e.g. vector<i32>).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static ecs_entity_t flecs_script_typecache_get(
    ecs_world_t *world)
{
    ecs_entity_t result = ecs_lookup(world, "flecs.script.typecache");
    if (!result) {
        ecs_entity_t prev_scope = ecs_set_scope(world, 0);
        result = ecs_entity(world, { .name = "flecs.script.typecache" });
        ecs_add_id(world, result, EcsModule);
        ecs_set_scope(world, prev_scope);
    }

    return result;
}

ecs_entity_t flecs_script_kind_component(
    const ecs_world_t *world,
    ecs_entity_t kind)
{
    (void)world;
    if (kind == ecs_id(ecs_value_t) || kind == ecs_id(EcsValue)) {
        return ecs_id(EcsValue);
    }

    if (kind == ecs_id(EcsVector) || kind == ecs_id(EcsArray) ||
        kind == ecs_id(EcsMap))
    {
        return kind;
    }

    return 0;
}

static void flecs_script_typecache_arg(
    const ecs_world_t *world,
    ecs_entity_t type,
    bool use_id,
    ecs_strbuf_t *buf)
{
    const char *name = ecs_get_name(world, type);
    if (!use_id && name && !strchr(name, '.')) {
        ecs_strbuf_appendstr(buf, name);
    } else {
        ecs_strbuf_append(buf, "#%u_%u",
            (uint32_t)type, (uint32_t)(type >> 32));
    }
}

static char* flecs_script_typecache_name(
    const ecs_world_t *world,
    ecs_entity_t component,
    const void *value,
    bool use_id)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    if (component == ecs_id(EcsVector)) {
        ecs_strbuf_appendlit(&buf, "vector<");
        flecs_script_typecache_arg(world,
            ((const EcsVector*)value)->type, use_id, &buf);
    } else if (component == ecs_id(EcsArray)) {
        const EcsArray *a = value;
        ecs_strbuf_appendlit(&buf, "array<");
        flecs_script_typecache_arg(world, a->type, use_id, &buf);
        ecs_strbuf_append(&buf, ",%d", a->count);
    } else if (component == ecs_id(EcsMap)) {
        const EcsMap *m = value;
        ecs_strbuf_appendlit(&buf, "map<");
        flecs_script_typecache_arg(world, m->key_type, use_id, &buf);
        ecs_strbuf_appendch(&buf, ',');
        flecs_script_typecache_arg(world, m->type, use_id, &buf);
    } else {
        ecs_assert(component == ecs_id(EcsValue), ECS_INTERNAL_ERROR, NULL);
        ecs_strbuf_appendlit(&buf, "value<");
        flecs_script_typecache_arg(world,
            ((const EcsValue*)value)->type, use_id, &buf);
    }

    ecs_strbuf_appendch(&buf, '>');
    return ecs_strbuf_get(&buf);
}

static bool flecs_script_typecache_match(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t component,
    const void *value)
{
    const void *existing = ecs_get_id(world, type, component);
    if (!existing) {
        return false;
    }

    if (component == ecs_id(EcsVector)) {
        return ((const EcsVector*)existing)->type ==
            ((const EcsVector*)value)->type;
    } else if (component == ecs_id(EcsArray)) {
        const EcsArray *a = existing, *b = value;
        return a->type == b->type && a->count == b->count;
    } else if (component == ecs_id(EcsMap)) {
        const EcsMap *a = existing, *b = value;
        return a->key_type == b->key_type && a->type == b->type;
    } else {
        return ((const EcsValue*)existing)->type ==
            ((const EcsValue*)value)->type;
    }
}

static bool flecs_script_typecache_is_type(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    return type && ecs_is_alive(world, type) &&
        ecs_get_type_info(world, type) != NULL;
}

static bool flecs_script_typecache_valid_args(
    const ecs_world_t *world,
    ecs_entity_t component,
    const void *value)
{
    if (component == ecs_id(EcsVector)) {
        return flecs_script_typecache_is_type(
            world, ((const EcsVector*)value)->type);
    } else if (component == ecs_id(EcsArray)) {
        const EcsArray *a = value;
        return a->count > 0 && flecs_script_typecache_is_type(world, a->type);
    } else if (component == ecs_id(EcsMap)) {
        const EcsMap *m = value;
        return flecs_script_typecache_is_type(world, m->key_type) &&
            flecs_script_typecache_is_type(world, m->type);
    }

    return true;
}

ecs_entity_t flecs_script_kind_type(
    ecs_world_t *world,
    ecs_entity_t component,
    const void *value)
{
    ecs_assert(flecs_script_kind_component(world, component) == component,
        ECS_INTERNAL_ERROR, NULL);

    if (component == ecs_id(EcsValue) && !((const EcsValue*)value)->type) {
        return ecs_id(ecs_value_t);
    }

    if (!flecs_script_typecache_valid_args(world, component, value)) {
        return 0;
    }

    ecs_entity_t typecache = flecs_script_typecache_get(world);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    char *name = NULL;
    int32_t attempt;
    for (attempt = 0; attempt < 2; attempt ++) {
        name = flecs_script_typecache_name(world, component, value, attempt);
        ecs_entity_t existing = ecs_lookup_child(world, typecache, name);
        if (!existing) {
            break;
        }

        if (flecs_script_typecache_match(world, existing, component, value)) {
            ecs_os_free(name);
            return existing;
        }

        ecs_os_free(name);
        name = NULL;
    }

    ecs_entity_t entity = ecs_entity(world, {
        .name = name, .parent = typecache });
    ecs_os_free(name);

    ecs_set_id(world, entity, component, flecs_itosize(ti->size), value);
    if (!ecs_has(world, entity, EcsType)) {
        ecs_delete(world, entity);
        return 0;
    }

    return entity;
}

bool flecs_script_is_anonymous_type(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_entity_t typecache = ecs_lookup(world, "flecs.script.typecache");
    return typecache && ecs_get_parent(world, type) == typecache;
}

ecs_entity_t flecs_script_array_type(
    ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count)
{
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    EcsArray value;
    ecs_os_zeromem(&value);
    value.type = elem_type;
    value.count = count;
    return flecs_script_kind_type(world, ecs_id(EcsArray), &value);
}

ecs_entity_t flecs_script_vector_type(
    ecs_world_t *world,
    ecs_entity_t elem_type)
{
    EcsVector value = { .type = elem_type };
    return flecs_script_kind_type(world, ecs_id(EcsVector), &value);
}

ecs_entity_t flecs_script_map_type(
    ecs_world_t *world,
    ecs_entity_t key_type,
    ecs_entity_t elem_type)
{
    EcsMap value = { .key_type = key_type, .type = elem_type };
    return flecs_script_kind_type(world, ecs_id(EcsMap), &value);
}

#endif
