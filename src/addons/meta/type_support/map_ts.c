/**
 * @file addons/meta/type_support/map_ts.c
 * @brief Map type support.
 */

#include "type_support.h"

#ifdef FLECS_META

static ecs_map_key_t flecs_meta_map_hash_string(
    const void *key)
{
    const char *str = *(const char* const*)key;
    if (!str) {
        return 0;
    }

    return flecs_hash(str, ecs_os_strlen(str));
}

/* Returns -1 if type cannot be used as key type. If keys of the type cannot be
 * stored as map key, hash_out is set to the hash function for the type. */
static int flecs_meta_map_key_type(
    const ecs_world_t *world,
    ecs_entity_t key_type,
    flecs_meta_map_hash_t *hash_out)
{
    *hash_out = NULL;

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
        *hash_out = flecs_meta_map_hash_string;
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
    flecs_meta_map_hash_t hash;
    return flecs_meta_map_key_type(world, key_type, &hash) == 0;
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

    if (flecs_meta_map_key_type(world, key_type, &info->hash)) {
        return -1;
    }

    info->key_ti = ecs_get_type_info(world, key_type);
    info->value_ti = ecs_get_type_info(world, value_type);
    if (!info->key_ti || !info->value_ti) {
        return -1;
    }

    if (info->hash) {
        ecs_assert(info->key_ti->hooks.equals != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        info->value_offset = ECS_ALIGN(
            info->key_ti->size, info->value_ti->alignment);
        info->entry_size = info->value_offset + info->value_ti->size;
    }

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
    if (info->hash) {
        return ECS_OFFSET((void*)(uintptr_t)val[0], info->value_offset);
    } else if (flecs_meta_map_alloc_value(info)) {
        return (void*)(uintptr_t)val[0];
    } else {
        return val;
    }
}

/* Find entry for key. If the key is not found, slot_out is set to the map key
 * at which the key should be inserted. */
static void* flecs_meta_map_find_entry(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map,
    const void *key,
    ecs_map_key_t *slot_out)
{
    ecs_map_key_t slot = info->hash(key);
    ecs_map_val_t *val;
    while ((val = ecs_map_get(map, slot))) {
        void *entry = (void*)(uintptr_t)val[0];
        if (flecs_type_info_equals(entry, key, info->key_ti)) {
            *slot_out = slot;
            return entry;
        }

        slot ++;
    }

    *slot_out = slot;
    return NULL;
}

/* Destruct and free element storage */
static void flecs_meta_map_free_elem(
    const ecs_meta_map_info_t *info,
    ecs_map_val_t *val)
{
    void *ptr = flecs_meta_map_value_ptr(info, val);
    flecs_type_info_dtor(ptr, 1, info->value_ti);

    if (info->hash) {
        void *entry = (void*)(uintptr_t)val[0];
        flecs_type_info_dtor(entry, 1, info->key_ti);
        ecs_os_free(entry);
    } else if (flecs_meta_map_alloc_value(info)) {
        ecs_os_free(ptr);
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

    if (info->hash) {
        ecs_map_key_t slot;
        void *entry = flecs_meta_map_find_entry(info, map, key, &slot);
        return entry ? ECS_OFFSET(entry, info->value_offset) : NULL;
    }

    ecs_map_val_t *val = ecs_map_get(map, flecs_meta_map_key(info, key));
    return val ? flecs_meta_map_value_ptr(info, val) : NULL;
}

void* flecs_meta_map_ensure(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key,
    bool *is_new)
{
    ecs_map_init_if(map, NULL);

    void *ptr;
    bool created;
    if (info->hash) {
        ecs_map_key_t slot;
        void *entry = flecs_meta_map_find_entry(info, map, key, &slot);
        created = entry == NULL;
        if (created) {
            entry = ecs_os_calloc(info->entry_size);
            flecs_type_info_ctor(entry, 1, info->key_ti);
            flecs_type_info_copy(entry, key, 1, info->key_ti);
            ecs_map_insert(map, slot, (ecs_map_val_t)(uintptr_t)entry);
        }

        ptr = ECS_OFFSET(entry, info->value_offset);
    } else {
        int32_t count = ecs_map_count(map);
        ecs_map_val_t *val = ecs_map_ensure(
            map, flecs_meta_map_key(info, key));
        created = count != ecs_map_count(map);
        if (created) {
            if (flecs_meta_map_alloc_value(info)) {
                val[0] = (ecs_map_val_t)(uintptr_t)
                    ecs_os_calloc(info->value_ti->size);
            } else {
                val[0] = 0;
            }
        }

        ptr = flecs_meta_map_value_ptr(info, val);
    }

    if (created) {
        flecs_type_info_ctor(ptr, 1, info->value_ti);
    }

    if (is_new) {
        *is_new = created;
    }

    return ptr;
}

bool flecs_meta_map_remove(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key)
{
    if (!ecs_map_is_init(map)) {
        return false;
    }

    if (!info->hash) {
        ecs_map_key_t map_key = flecs_meta_map_key(info, key);
        ecs_map_val_t *val = ecs_map_get(map, map_key);
        if (!val) {
            return false;
        }

        flecs_meta_map_free_elem(info, val);
        ecs_map_remove(map, map_key);
        return true;
    }

    ecs_map_key_t slot;
    if (!flecs_meta_map_find_entry(info, map, key, &slot)) {
        return false;
    }

    flecs_meta_map_free_elem(info, ecs_map_get(map, slot));
    ecs_map_remove(map, slot);

    /* Entries after the removed entry may have been probed past it. Reinsert
     * them so that they can still be found. */
    ecs_map_val_t *val;
    while ((val = ecs_map_get(map, ++ slot))) {
        void *entry = (void*)(uintptr_t)val[0];
        ecs_map_remove(map, slot);

        ecs_map_key_t new_slot;
        void *found = flecs_meta_map_find_entry(info, map, entry, &new_slot);
        ecs_assert(found == NULL, ECS_INTERNAL_ERROR, NULL);
        (void)found;
        ecs_map_insert(map, new_slot, (ecs_map_val_t)(uintptr_t)entry);
    }

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
        flecs_meta_map_free_elem(info, &it.res[1]);
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
        /* Insert elements at the same map keys as the source map, which also
         * preserves the probe sequences of keys stored in entries. */
        ecs_map_val_t *val = ecs_map_ensure(dst, ecs_map_key(&it));
        if (info->hash) {
            const void *src_entry = (void*)(uintptr_t)it.res[1];
            void *entry = ecs_os_calloc(info->entry_size);
            flecs_type_info_ctor(entry, 1, info->key_ti);
            flecs_type_info_copy(entry, src_entry, 1, info->key_ti);
            val[0] = (ecs_map_val_t)(uintptr_t)entry;
        } else if (flecs_meta_map_alloc_value(info)) {
            val[0] = (ecs_map_val_t)(uintptr_t)ecs_os_calloc(value_ti->size);
        } else {
            val[0] = 0;
        }

        void *dst_value = flecs_meta_map_value_ptr(info, val);
        flecs_type_info_ctor(dst_value, 1, value_ti);
        flecs_type_info_copy(dst_value, 
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
    if (it->info.hash) {
        return (void*)(uintptr_t)it->it.res[1];
    }

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
