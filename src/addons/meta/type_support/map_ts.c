/**
 * @file addons/meta/type_support/map_ts.c
 * @brief Map type support.
 */

#include "type_support.h"

#ifdef FLECS_META

/* Returns -1 if type cannot be used as key type. If keys of the type have
 * lifecycle hooks, owns_key_out is set to true. */
static int flecs_meta_map_key_type(
    const ecs_world_t *world,
    ecs_entity_t key_type,
    bool *owns_key_out)
{
    *owns_key_out = false;

    const EcsType *kt = ecs_get(world, key_type, EcsType);
    if (kt && (kt->kind == EcsEnumType || kt->kind == EcsBitmaskType)) {
        return 0;
    }

    const EcsPrimitive *p = ecs_get(world, key_type, EcsPrimitive);
    if (!p) {
        return -1;
    }

    switch(p->kind) {
    case EcsBool:
    case EcsChar:
    case EcsByte:
    case EcsU8:
    case EcsU16:
    case EcsU32:
    case EcsU64:
    case EcsI8:
    case EcsI16:
    case EcsI32:
    case EcsI64:
    case EcsUPtr:
    case EcsIPtr:
    case EcsEntity:
    case EcsId:
        return 0;
    case EcsString:
        *owns_key_out = true;
        return 0;
    case EcsF32:
    case EcsF64:
        return -1;
    default:
        return -1;
    }
}

bool flecs_meta_map_key_type_is_valid(
    const ecs_world_t *world,
    ecs_entity_t key_type)
{
    bool owns_key;
    return flecs_meta_map_key_type(world, key_type, &owns_key) == 0;
}

int flecs_meta_map_info_init(
    const ecs_world_t *world,
    ecs_entity_t key_type,
    ecs_entity_t value_type,
    ecs_meta_map_info_t *info)
{
    ecs_os_zeromem(info);

    if (!key_type || !value_type) {
        return -1;
    }

    if (flecs_meta_map_key_type(world, key_type, &info->owns_key)) {
        return -1;
    }

    info->key_ti = ecs_get_type_info(world, key_type);
    info->value_ti = ecs_get_type_info(world, value_type);
    if (!info->key_ti || !info->value_ti) {
        return -1;
    }

    ecs_assert(info->key_ti->size <= ECS_SIZEOF(ecs_map_key_t),
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!info->owns_key || info->key_ti->hooks.equals != NULL, 
        ECS_INTERNAL_ERROR, NULL);

    return 0;
}

int flecs_meta_map_info_from_type(
    const ecs_world_t *world,
    ecs_entity_t map_type,
    ecs_meta_map_info_t *info)
{
    const EcsMap *m = ecs_get(world, map_type, EcsMap);
    if (!m) {
        ecs_os_zeromem(info);
        return -1;
    }

    return flecs_meta_map_info_init(world, m->key_type, m->type, info);
}

static bool flecs_meta_map_alloc_value(
    const ecs_meta_map_info_t *info)
{
    return info->value_ti->size > ECS_SIZEOF(ecs_map_val_t);
}

static ecs_map_key_t flecs_meta_map_key(
    const ecs_meta_map_info_t *info,
    const void *key)
{
    ecs_map_key_t result = 0;
    ecs_os_memcpy(&result, key, info->key_ti->size);
    return result;
}

static void* flecs_meta_map_value_ptr(
    const ecs_meta_map_info_t *info,
    ecs_map_val_t *val)
{
    if (flecs_meta_map_alloc_value(info)) {
        return (void*)(uintptr_t)val[0];
    } else {
        return val;
    }
}

/* Find map key for key. For keys owned by the map, the map key is the key
 * value (e.g. the string pointer), so keys are looked up by comparing them
 * with the keys in the map. */
static bool flecs_meta_map_find_key(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map,
    const void *key,
    ecs_map_key_t *map_key_out)
{
    if (!info->owns_key) {
        *map_key_out = flecs_meta_map_key(info, key);
        return ecs_map_get(map, *map_key_out) != NULL;
    }

    ecs_map_iter_t it = ecs_map_iter(map);
    while (ecs_map_next(&it)) {
        if (flecs_type_info_equals(&it.res[0], key, info->key_ti)) {
            *map_key_out = it.res[0];
            return true;
        }
    }

    return false;
}

static ecs_map_key_t flecs_meta_map_key_copy(
    const ecs_meta_map_info_t *info,
    const void *key)
{
    if (!info->owns_key) {
        return flecs_meta_map_key(info, key);
    }

    ecs_map_key_t result = 0;
    flecs_type_info_ctor(&result, 1, info->key_ti);
    flecs_type_info_copy(&result, key, 1, info->key_ti);
    return result;
}

static ecs_map_val_t* flecs_meta_map_insert_elem(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    ecs_map_key_t map_key)
{
    ecs_map_val_t *val = ecs_map_ensure(map, map_key);
    if (flecs_meta_map_alloc_value(info)) {
        val[0] = (ecs_map_val_t)(uintptr_t)ecs_os_calloc(info->value_ti->size);
    } else {
        val[0] = 0;
    }

    flecs_type_info_ctor(flecs_meta_map_value_ptr(info, val), 1, 
        info->value_ti);
    return val;
}

/* Destruct and free element storage */
static void flecs_meta_map_free_elem(
    const ecs_meta_map_info_t *info,
    ecs_map_key_t map_key,
    ecs_map_val_t *val)
{
    void *ptr = flecs_meta_map_value_ptr(info, val);
    flecs_type_info_dtor(ptr, 1, info->value_ti);

    if (flecs_meta_map_alloc_value(info)) {
        ecs_os_free(ptr);
    }

    if (info->owns_key) {
        flecs_type_info_dtor(&map_key, 1, info->key_ti);
    }
}

void* flecs_meta_map_get(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map,
    const void *key)
{
    if (!ecs_map_is_init(map)) {
        return NULL;
    }

    ecs_map_key_t map_key;
    if (!flecs_meta_map_find_key(info, map, key, &map_key)) {
        return NULL;
    }

    return flecs_meta_map_value_ptr(info, ecs_map_get(map, map_key));
}

void* flecs_meta_map_ensure(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key,
    bool *is_new)
{
    ecs_map_init_if(map, NULL);

    ecs_map_key_t map_key;
    ecs_map_val_t *val;
    bool created = !flecs_meta_map_find_key(info, map, key, &map_key);
    if (created) {
        val = flecs_meta_map_insert_elem(info, map, 
            flecs_meta_map_key_copy(info, key));
    } else {
        val = ecs_map_get(map, map_key);
    }

    if (is_new) {
        *is_new = created;
    }

    return flecs_meta_map_value_ptr(info, val);
}

bool flecs_meta_map_remove(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key)
{
    if (!ecs_map_is_init(map)) {
        return false;
    }

    ecs_map_key_t map_key;
    if (!flecs_meta_map_find_key(info, map, key, &map_key)) {
        return false;
    }

    flecs_meta_map_free_elem(info, map_key, ecs_map_get(map, map_key));
    ecs_map_remove(map, map_key);
    return true;
}

void flecs_meta_map_clear(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map)
{
    if (!ecs_map_is_init(map) || !ecs_map_count(map)) {
        return;
    }

    ecs_map_iter_t it = ecs_map_iter(map);
    while (ecs_map_next(&it)) {
        flecs_meta_map_free_elem(info, it.res[0], &it.res[1]);
    }

    ecs_map_clear(map);
}

void flecs_meta_map_fini(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map)
{
    flecs_meta_map_clear(info, map);
    ecs_map_fini(map);
}

void flecs_meta_map_copy(
    const ecs_meta_map_info_t *info,
    ecs_map_t *dst,
    const ecs_map_t *src)
{
    if (dst == src) {
        return;
    }

    flecs_meta_map_clear(info, dst);
    ecs_map_init_if(dst, NULL);

    if (!ecs_map_is_init(src)) {
        return;
    }

    const ecs_type_info_t *value_ti = info->value_ti;
    ecs_map_iter_t it = ecs_map_iter(src);
    while (ecs_map_next(&it)) {
        ecs_map_val_t *val = flecs_meta_map_insert_elem(info, dst, 
            flecs_meta_map_key_copy(info, &it.res[0]));
        flecs_type_info_copy(flecs_meta_map_value_ptr(info, val), 
            flecs_meta_map_value_ptr(info, &it.res[1]), 1, value_ti);
    }
}

ecs_meta_map_iter_t flecs_meta_map_iter(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map)
{
    return (ecs_meta_map_iter_t){
        .info = *info,
        .it = ecs_map_iter(map)
    };
}

bool flecs_meta_map_next(
    ecs_meta_map_iter_t *it)
{
    return ecs_map_next(&it->it);
}

void* flecs_meta_map_iter_key(
    const ecs_meta_map_iter_t *it)
{
    return &it->it.res[0];
}

void* flecs_meta_map_iter_value(
    const ecs_meta_map_iter_t *it)
{
    return flecs_meta_map_value_ptr(&it->info, &it->it.res[1]);
}

static void flecs_set_map(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMap *map = ecs_field(it, EcsMap, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t key_type = map[i].key_type;
        ecs_entity_t value_type = map[i].type;

        if (!key_type) {
            ecs_err("map '%s' has no key type", ecs_get_name(world, e));
            continue;
        }

        if (!value_type) {
            ecs_err("map '%s' has no value type", ecs_get_name(world, e));
            continue;
        }

        if (!ecs_is_alive(world, key_type)) {
            ecs_err("map '%s' has invalid key type", ecs_get_name(world, e));
            continue;
        }

        if (!flecs_meta_map_key_type_is_valid(world, key_type)) {
            ecs_err("map '%s' key type must be a primitive (other than "
                "f32 or f64), enum or bitmask type",
                ecs_get_name(world, e));
            continue;
        }

        if (!ecs_is_alive(world, value_type)) {
            ecs_err("map '%s' has invalid value type", ecs_get_name(world, e));
            continue;
        }

        if (!ecs_get_type_info(world, value_type)) {
            ecs_err("map '%s' value type has no type info",
                ecs_get_name(world, e));
            continue;
        }

        if (flecs_init_type_t(it->stage, e, EcsMapType, ecs_map_t)) {
            continue;
        }
    }
}

ecs_entity_t ecs_map_type_init(
    ecs_world_t *world,
    const ecs_map_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsMap, {
        .key_type = desc->key_type,
        .type = desc->type
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

void flecs_meta_map_init(
    ecs_world_t *world)
{
    ecs_entity_t map_component = ecs_entity(world, { .id = ecs_id(EcsMap),
        .name = "map", .symbol = "EcsMap" });
    ecs_add_pair(world, map_component, EcsOnInstantiate, EcsDontInherit);
    ecs_component(world, {
        .entity = map_component,
        .type.size = sizeof(EcsMap),
        .type.alignment = ECS_ALIGNOF(EcsMap)
    });

    ecs_observer(world, {
        .query.terms[0] = { .id = ecs_id(EcsMap) },
        .events = {EcsOnSet},
        .callback = flecs_set_map,
        .global_observer = true
    });
}

#endif
