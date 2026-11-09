#include "../private_api.h"
#include "world_guard.h"

#if defined(FLECS_REST) || defined(FLECS_SCRIPT)

#define FLECS_GUARD_MAX_DEPTH (1024)
#define FLECS_GUARD_MAX_SIZE (1 << 24)

static const char* flecs_guard_path(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    return flecs_errstr(ecs_get_path(world, e));
}

static const char* flecs_guard_id_str(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return flecs_errstr_1(ecs_id_str(world, id));
}

static bool flecs_guard_is_builtin(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    if (e == EcsFlecs || e == EcsFlecsCore) {
        return true;
    }

    ecs_table_t *table = ecs_get_table(world, e);
    return table && (table->flags & EcsTableHasBuiltins);
}

static bool flecs_guard_in_use(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    return ecs_id_in_use(world, e) ||
        ecs_id_in_use(world, ecs_pair(e, EcsWildcard));
}

static char* flecs_guard_entity(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    if (!e || (e & ECS_ID_FLAGS_MASK) || !ecs_is_alive(world, e)) {
        return flecs_asprintf("entity #%u is not alive", (uint32_t)e);
    }

    return NULL;
}

static bool flecs_guard_element_alive(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    if (e == EcsWildcard || e == EcsAny) {
        return true;
    }

    return ecs_get_alive(world, e) != 0;
}

static ecs_id_t flecs_guard_strip_flags(
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        return id;
    }

    return id & ~(ECS_AUTO_OVERRIDE | ECS_TOGGLE);
}

static char* flecs_guard_id(
    const ecs_world_t *world,
    ecs_id_t id,
    bool allow_wildcard)
{
    ecs_id_t base = flecs_guard_strip_flags(id);
    if (!base) {
        return flecs_asprintf("invalid component '#0'");
    }

    if (ECS_IS_PAIR(base)) {
        ecs_entity_t first = ECS_PAIR_FIRST(base);
        ecs_entity_t second = ECS_PAIR_SECOND(base);
        if (!first || (!second && !ECS_IS_VALUE_PAIR(base))) {
            return flecs_asprintf("invalid pair: pair elements cannot be 0");
        }

        if (!flecs_guard_element_alive(world, first)) {
            return flecs_asprintf("invalid pair: relationship #%u is not alive",
                (uint32_t)first);
        }

        if (!ECS_IS_VALUE_PAIR(base) &&
            !flecs_guard_element_alive(world, second))
        {
            return flecs_asprintf("invalid pair: target #%u is not alive",
                (uint32_t)second);
        }
    } else {
        if (base & ECS_ID_FLAGS_MASK) {
            return flecs_asprintf("invalid component: unsupported id flags");
        }

        if (!flecs_guard_element_alive(world, base)) {
            return flecs_asprintf("invalid component: #%u is not alive",
                (uint32_t)base);
        }
    }

    if (!allow_wildcard && ecs_id_is_wildcard(base)) {
        return flecs_asprintf("invalid component '%s': cannot use wildcards",
            flecs_guard_id_str(world, base));
    }

    return NULL;
}

static bool flecs_guard_reachable_intern(
    const ecs_world_t *world,
    ecs_entity_t rel,
    ecs_entity_t from,
    ecs_entity_t to,
    ecs_map_t *visited,
    int32_t depth)
{
    if (from == to) {
        return true;
    }

    if (depth > FLECS_GUARD_MAX_DEPTH) {
        return true;
    }

    if (ecs_map_get(visited, from)) {
        return false;
    }

    ecs_map_insert(visited, from, 1);

    if (!ecs_is_alive(world, from)) {
        return false;
    }

    if (rel == EcsChildOf) {
        ecs_entity_t parent = ecs_get_parent(world, from);
        return parent && flecs_guard_reachable_intern(
            world, rel, parent, to, visited, depth + 1);
    }

    int32_t i = 0;
    ecs_entity_t tgt;
    while ((tgt = ecs_get_target(world, from, rel, i ++))) {
        if (flecs_guard_reachable_intern(
            world, rel, tgt, to, visited, depth + 1))
        {
            return true;
        }
    }

    return false;
}

static bool flecs_guard_reachable(
    const ecs_world_t *world,
    ecs_entity_t rel,
    ecs_entity_t from,
    ecs_entity_t to)
{
    ecs_map_t visited;
    ecs_map_init(&visited, NULL);
    bool result = flecs_guard_reachable_intern(
        world, rel, from, to, &visited, 0);
    ecs_map_fini(&visited);
    return result;
}

static bool flecs_guard_is_traversable(
    const ecs_world_t *world,
    ecs_entity_t rel)
{
    return rel == EcsChildOf || rel == EcsIsA ||
        ecs_has_id(world, rel, EcsTraversable);
}

static char* flecs_guard_parent(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t parent)
{
    if (e == EcsFlecs || e == EcsFlecsCore) {
        return flecs_asprintf("cannot reparent builtin module '%s'",
            flecs_guard_path(world, e));
    }

    if (flecs_guard_is_builtin(world, e)) {
        return flecs_asprintf("cannot reparent builtin entity '%s'",
            flecs_guard_path(world, e));
    }

    if (!parent || !ecs_is_alive(world, parent)) {
        return flecs_asprintf("parent #%u is not alive", (uint32_t)parent);
    }

    if (parent == EcsFlecsCore) {
        return flecs_asprintf("cannot add entity '%s' to builtin module '%s'",
            flecs_guard_path(world, e), flecs_guard_id_str(world, parent));
    }

    if (flecs_guard_reachable(world, EcsChildOf, parent, e)) {
        return flecs_asprintf("cannot make '%s' a child of '%s': "
            "cycle detected in hierarchy",
                flecs_guard_path(world, e), flecs_guard_id_str(world, parent));
    }

    return NULL;
}

#ifdef FLECS_CONSTRAINT_TRAITS
static char* flecs_guard_constraints(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ecs_pair_first(world, id);
        ecs_entity_t tgt = 0;
        if (!ECS_IS_VALUE_PAIR(id)) {
            tgt = ecs_pair_second(world, id);
        }

        if (tgt && !ecs_id_is_wildcard(tgt)) {
            if (ecs_has_id(world, tgt, EcsRelationship) &&
                !ecs_has_id(world, rel, EcsTrait))
            {
                return flecs_asprintf("cannot use '%s' as target in pair "
                    "'%s': it has the Relationship trait",
                        flecs_guard_path(world, tgt),
                        flecs_guard_id_str(world, id));
            }
        }

        if (ecs_has_id(world, rel, EcsTarget)) {
            return flecs_asprintf("cannot use '%s' as relationship in pair "
                "'%s': it has the Target trait",
                    flecs_guard_path(world, rel),
                    flecs_guard_id_str(world, id));
        }

        if (tgt && !ecs_id_is_wildcard(tgt)) {
            ecs_entity_t oneof = ecs_has_id(world, rel, EcsOneOf) ? rel :
                ecs_get_target(world, rel, EcsOneOf, 0);
            if (oneof && !ecs_has_pair(world, tgt, EcsChildOf, oneof)) {
                return flecs_asprintf("cannot use '%s' as target in pair "
                    "'%s': target must be a child of '%s'",
                        flecs_guard_path(world, tgt),
                        flecs_guard_id_str(world, id),
                        flecs_errstr_2(ecs_get_path(world, oneof)));
            }

            if (rel == EcsIsA && ecs_has_id(world, tgt, EcsFinal)) {
                return flecs_asprintf("cannot inherit from '%s': "
                    "it has the Final trait", flecs_guard_path(world, tgt));
            }
        }

        if (e && ecs_has_id(world, rel, EcsSingleton) &&
            ((uint32_t)rel != (uint32_t)e))
        {
            return flecs_asprintf("cannot add singleton pair '%s' to '%s'",
                flecs_guard_id_str(world, id), flecs_guard_path(world, e));
        }
    } else {
        if (ecs_has_id(world, id, EcsTarget)) {
            return flecs_asprintf("cannot use '%s' by itself: "
                "it has the Target trait", flecs_guard_path(world, id));
        }

        if (ecs_has_id(world, id, EcsRelationship)) {
            return flecs_asprintf("cannot use '%s' by itself: "
                "it has the Relationship trait", flecs_guard_path(world, id));
        }

        if (e && ecs_has_id(world, id, EcsSingleton) &&
            ((uint32_t)id != (uint32_t)e))
        {
            return flecs_asprintf("cannot add singleton '%s' to '%s'",
                flecs_guard_id_str(world, id), flecs_guard_path(world, e));
        }
    }

    return NULL;
}
#endif

static bool flecs_guard_is_rest(
    ecs_id_t id)
{
#ifdef FLECS_REST
    return ecs_id(EcsRest) && (id == ecs_id(EcsRest));
#else
    (void)id;
    return false;
#endif
}

static bool flecs_guard_is_rest_module(
    const ecs_world_t *world,
    ecs_entity_t e)
{
#ifdef FLECS_REST
    return ecs_id(EcsRest) && e &&
        (e == ecs_get_parent(world, ecs_id(EcsRest)));
#else
    (void)world;
    (void)e;
    return false;
#endif
}

static bool flecs_guard_has_rest(
    const ecs_world_t *world,
    ecs_entity_t e)
{
#ifdef FLECS_REST
    return ecs_id(EcsRest) && ecs_owns_id(world, e, ecs_id(EcsRest));
#else
    (void)world;
    (void)e;
    return false;
#endif
}

static bool flecs_guard_is_flag_trait(
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        return rel == EcsOnDelete || rel == EcsOnDeleteTarget ||
            rel == EcsWith || rel == EcsOnInstantiate;
    }

    return id == EcsPairIsTag || id == EcsInheritable ||
        id == EcsTraversable || id == EcsExclusive || id == EcsCanToggle ||
        id == EcsSparse || id == EcsDontFragment || id == EcsSingleton ||
        id == EcsSymmetric || id == EcsFinal || id == ecs_id(EcsComponent);
}

static char* flecs_guard_trait(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id,
    bool add)
{
    if (!flecs_guard_is_flag_trait(id)) {
        return NULL;
    }

    if (add && !(ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsWith) &&
        flecs_component_is_trait_locked(world, e))
    {
        return flecs_asprintf("cannot change trait '%s' for '%s': "
            "component is already queried for",
                flecs_guard_id_str(world, id), flecs_guard_path(world, e));
    }

    if (id == EcsFinal) {
        if (flecs_components_get(world, ecs_pair(EcsIsA, e))) {
            return flecs_asprintf("cannot change trait 'Final' for '%s': "
                "already inherited from", flecs_guard_path(world, e));
        }

        return NULL;
    }

    if (flecs_guard_in_use(world, e)) {
        return flecs_asprintf("cannot change trait '%s' for '%s': "
            "component is already in use",
                flecs_guard_id_str(world, id), flecs_guard_path(world, e));
    }

    if (add && ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsWith) {
        ecs_entity_t tgt = ecs_pair_second(world, id);
        if (ecs_owns_id(world, e, EcsDontFragment) &&
            !ecs_owns_id(world, tgt, EcsDontFragment))
        {
            return flecs_asprintf("cannot add '%s' to non-fragmenting "
                "component '%s': target must also be non-fragmenting",
                    flecs_guard_id_str(world, id), flecs_guard_path(world, e));
        }
    }

    if (add && id == EcsDontFragment) {
        const ecs_type_t *type = ecs_get_type(world, e);
        int32_t i;
        for (i = 0; type && i < type->count; i ++) {
            ecs_id_t with = type->array[i];
            if (!ECS_HAS_RELATION(with, EcsWith)) {
                continue;
            }

            if (!ecs_owns_id(world, ecs_pair_second(world, with),
                EcsDontFragment))
            {
                return flecs_asprintf("cannot add 'DontFragment' to '%s': "
                    "targets of '%s' must also be non-fragmenting",
                        flecs_guard_path(world, e),
                        flecs_guard_id_str(world, with));
            }
        }
    }

    return NULL;
}

#ifdef FLECS_META
static bool flecs_guard_type_depends_intern(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t target,
    ecs_map_t *visited,
    int32_t depth)
{
    if (!type) {
        return false;
    }

    if (type == target || depth > FLECS_GUARD_MAX_DEPTH) {
        return true;
    }

    if (ecs_map_get(visited, type)) {
        return false;
    }

    ecs_map_insert(visited, type, 1);

    if (!ecs_is_alive(world, type)) {
        return false;
    }

    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    if (st) {
        ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
        int32_t i, count = ecs_vec_count(&st->members);
        for (i = 0; i < count; i ++) {
            if (flecs_guard_type_depends_intern(
                world, members[i].type, target, visited, depth + 1))
            {
                return true;
            }
        }
    }

    const EcsArray *arr = ecs_get(world, type, EcsArray);
    if (arr) {
        if (flecs_guard_type_depends_intern(
            world, arr->type, target, visited, depth + 1))
        {
            return true;
        }
    }

    return false;
}

static bool flecs_guard_type_depends(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t target)
{
    ecs_map_t visited;
    ecs_map_init(&visited, NULL);
    bool result = flecs_guard_type_depends_intern(
        world, type, target, &visited, 0);
    ecs_map_fini(&visited);
    return result;
}

static bool flecs_guard_type_locked(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    if (!ti || !(ti->hooks.flags & ECS_TYPE_HOOK_IN_USE)) {
        return false;
    }

    const EcsType *t = ecs_get(world, type, EcsType);
    return t && !t->existing;
}

static bool flecs_guard_value_changed(
    const ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id,
    const void *value)
{
    const void *cur = ecs_get_id(world, e, id);
    if (!cur) {
        return true;
    }

    if (!value) {
        return false;
    }

#ifdef FLECS_JSON
    ecs_entity_t type = ecs_get_typeid(world, id);
    char *cur_json = ecs_ptr_to_json(world, type, cur);
    char *new_json = ecs_ptr_to_json(world, type, value);
    bool changed = !cur_json || !new_json || ecs_os_strcmp(cur_json, new_json);
    ecs_os_free(cur_json);
    ecs_os_free(new_json);
    return changed;
#else
    return true;
#endif
}

static bool flecs_guard_is_type_kind(
    ecs_id_t id)
{
    return id == ecs_id(EcsPrimitive) || id == ecs_id(EcsStruct) ||
        id == ecs_id(EcsEnum) || id == ecs_id(EcsBitmask) ||
        id == ecs_id(EcsArray) || id == ecs_id(EcsVector) ||
        id == ecs_id(EcsOpaque);
}

static char* flecs_guard_element_size(
    const ecs_world_t *world,
    ecs_entity_t type,
    int32_t count,
    ecs_entity_t e)
{
    if (count < 0) {
        return flecs_asprintf("invalid element count for '%s'",
            flecs_guard_path(world, e));
    }

    if (!type || !ecs_is_alive(world, type)) {
        return NULL;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    if (ti && ((int64_t)ti->size * (int64_t)count) > FLECS_GUARD_MAX_SIZE) {
        return flecs_asprintf("type '%s' is too large",
            flecs_guard_path(world, e));
    }

    return NULL;
}

static char* flecs_guard_meta(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id,
    const void *value,
    bool remove)
{
    if (id == ecs_id(EcsType) || id == ecs_id(EcsTypeSerializer)) {
        if (value || remove) {
            return flecs_asprintf("cannot modify internal component '%s'",
                flecs_guard_id_str(world, id));
        }

        return NULL;
    }

    if (flecs_guard_is_type_kind(id) || id == ecs_id(EcsConstants)) {
        if (flecs_guard_type_locked(world, e) &&
            (remove || flecs_guard_value_changed(world, e, id, value)))
        {
            return flecs_asprintf("cannot change reflection data of '%s': "
                "type is already in use", flecs_guard_path(world, e));
        }
    }

    if (id == ecs_id(EcsMember) || id == EcsConstant ||
        (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsConstant))
    {
        ecs_entity_t parent = ecs_get_parent(world, e);
        if (parent && flecs_guard_type_locked(world, parent) &&
            (remove || flecs_guard_value_changed(world, e, id, value)))
        {
            return flecs_asprintf("cannot change reflection data of '%s': "
                "type is already in use", flecs_guard_path(world, parent));
        }
    }

    if (!value) {
        return NULL;
    }

    if (id == ecs_id(EcsArray)) {
        const EcsArray *v = value;
        if (flecs_guard_type_depends(world, v->type, e)) {
            return flecs_asprintf("cyclic type definition for '%s'",
                flecs_guard_path(world, e));
        }

        return flecs_guard_element_size(world, v->type, v->count, e);
    }

    if (id == ecs_id(EcsStruct)) {
        const EcsStruct *v = value;
        ecs_member_t *members = ecs_vec_first_t(&v->members, ecs_member_t);
        int32_t i, count = ecs_vec_count(&v->members);
        for (i = 0; i < count; i ++) {
            if (flecs_guard_type_depends(world, members[i].type, e)) {
                return flecs_asprintf("cyclic type definition for '%s'",
                    flecs_guard_path(world, e));
            }

            char *err = flecs_guard_element_size(
                world, members[i].type, members[i].count, e);
            if (err) {
                return err;
            }
        }

        return NULL;
    }

    if (id == ecs_id(EcsMember)) {
        const EcsMember *v = value;
        ecs_entity_t parent = ecs_get_parent(world, e);
        if (parent && flecs_guard_type_depends(world, v->type, parent)) {
            return flecs_asprintf("cyclic type definition for '%s'",
                flecs_guard_path(world, parent));
        }

        return flecs_guard_element_size(world, v->type, v->count, e);
    }

    if (id == ecs_id(EcsEnum)) {
        const EcsEnum *v = value;
        if (!v->underlying_type) {
            return NULL;
        }

        const EcsPrimitive *p = NULL;
        if (ecs_is_alive(world, v->underlying_type)) {
            p = ecs_get(world, v->underlying_type, EcsPrimitive);
        }

        if (!p || !((p->kind >= EcsU8 && p->kind <= EcsI64) ||
            p->kind == EcsUPtr || p->kind == EcsIPtr))
        {
            return flecs_asprintf("invalid underlying type for enum '%s'",
                flecs_guard_path(world, e));
        }
    }

    return NULL;
}
#endif

static char* flecs_guard_add_intern(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id,
    const void *value)
{
    if (flecs_guard_is_rest(id)) {
        return flecs_asprintf("cannot modify REST server component of '%s'",
            flecs_guard_path(world, e));
    }

    if (id == EcsDisabled && flecs_guard_is_rest_module(world, e)) {
        return flecs_asprintf("cannot disable REST module '%s'",
            flecs_guard_path(world, e));
    }

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ecs_pair_first(world, id);
        if (!ECS_IS_VALUE_PAIR(id)) {
            ecs_entity_t tgt = ecs_pair_second(world, id);

            if (rel == EcsChildOf) {
                char *err = flecs_guard_parent(world, e, tgt);
                if (err) {
                    return err;
                }
            } else if (flecs_guard_is_traversable(world, rel)) {
                if (flecs_guard_reachable(world, rel, tgt, e)) {
                    return flecs_asprintf("cannot add '%s' to '%s': "
                        "cycle detected in traversable relationship",
                            flecs_guard_id_str(world, id),
                            flecs_guard_path(world, e));
                }
            }

            if (flecs_guard_is_builtin(world, e) &&
                ecs_has_id(world, rel, EcsExclusive) &&
                ecs_has_id(world, e, ecs_pair(rel, EcsWildcard)))
            {
                return flecs_asprintf("cannot change '%s' of builtin "
                    "entity '%s'", flecs_guard_id_str(world, id),
                        flecs_guard_path(world, e));
            }
        }

        if (rel == ecs_id(EcsIdentifier) &&
            ECS_PAIR_SECOND(id) == EcsName)
        {
            char *err = flecs_guard_rename(world, e);
            if (err) {
                return err;
            }
        }
    }

#ifdef FLECS_CONSTRAINT_TRAITS
    char *err = flecs_guard_constraints(world, e, id);
    if (err) {
        return err;
    }
#endif

    if (id == ecs_id(EcsComponent) && value) {
        const EcsComponent *c = value;
        if (c->size < 0 || c->alignment < 0 ||
            (!c->size != !c->alignment) ||
            (c->alignment & (c->alignment - 1)) ||
            c->size > FLECS_GUARD_MAX_SIZE)
        {
            return flecs_asprintf("invalid size/alignment for component '%s'",
                flecs_guard_path(world, e));
        }

        const EcsComponent *cur = ecs_get(world, e, EcsComponent);
        if (cur && cur->size == c->size && cur->alignment == c->alignment) {
            return NULL;
        }

        if (cur || flecs_guard_in_use(world, e)) {
            return flecs_asprintf("cannot change size of component '%s'",
                flecs_guard_path(world, e));
        }

        if (flecs_component_is_trait_locked(world, e)) {
            return flecs_asprintf("cannot change size of component '%s': "
                "component is already queried for", flecs_guard_path(world, e));
        }
    } else if (!ecs_owns_id(world, e, id)) {
        char *err = flecs_guard_trait(world, e, id, true);
        if (err) {
            return err;
        }
    }

    if (id == ecs_id(EcsParent) && value) {
        const EcsParent *p = value;
        char *err = flecs_guard_parent(world, e, p->value);
        if (err) {
            return err;
        }
    }

#ifdef FLECS_META
    {
        char *err = flecs_guard_meta(world, e, id, value, false);
        if (err) {
            return err;
        }
    }
#endif

    return NULL;
}

char* flecs_guard_id_valid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return flecs_guard_id(ecs_get_world(world), id, false);
}

char* flecs_guard_component(
    ecs_world_t *world,
    ecs_id_t id)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_id(world, id, false);
    if (err) {
        return err;
    }

#ifdef FLECS_CONSTRAINT_TRAITS
    return flecs_guard_constraints(world, 0, flecs_guard_strip_flags(id));
#else
    return NULL;
#endif
}

char* flecs_guard_add(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return flecs_guard_set(world, entity, id, NULL);
}

char* flecs_guard_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *value)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    if ((err = flecs_guard_id(world, id, false))) {
        return err;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    if (value && ti && (ti->hooks.flags &
        (ECS_TYPE_HOOK_COPY_ILLEGAL|ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL)))
    {
        return flecs_asprintf("cannot set '%s': type is not copyable",
            flecs_guard_id_str(world, id));
    }

    if (ti && (ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL) &&
        !ecs_owns_id(world, entity, flecs_guard_strip_flags(id)))
    {
        return flecs_asprintf("cannot add '%s': type is not constructible",
            flecs_guard_id_str(world, id));
    }

    return flecs_guard_add_intern(
        world, entity, flecs_guard_strip_flags(id), value);
}

static char* flecs_guard_remove_id(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t id)
{
    if (flecs_guard_is_rest(id)) {
        return flecs_asprintf("cannot remove REST server component from '%s'",
            flecs_guard_path(world, e));
    }

    if (id == EcsDisabled && flecs_guard_is_rest_module(world, e)) {
        return flecs_asprintf("cannot enable REST module '%s'",
            flecs_guard_path(world, e));
    }

    if (flecs_guard_is_builtin(world, e)) {
        return flecs_asprintf("cannot remove '%s' from builtin entity '%s'",
            flecs_guard_id_str(world, id), flecs_guard_path(world, e));
    }

    char *err = flecs_guard_trait(world, e, id, false);
    if (err) {
        return err;
    }

#ifdef FLECS_META
    if ((err = flecs_guard_meta(world, e, id, NULL, true))) {
        return err;
    }
#endif

    return NULL;
}

char* flecs_guard_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    if ((err = flecs_guard_id(world, id, true))) {
        return err;
    }

    id = flecs_guard_strip_flags(id);

    if (!ecs_id_is_wildcard(id)) {
        if (!ecs_owns_id(world, entity, id)) {
            return NULL;
        }

        return flecs_guard_remove_id(world, entity, id);
    }

    const ecs_type_t *type = ecs_get_type(world, entity);
    int32_t i;
    for (i = 0; type && i < type->count; i ++) {
        ecs_id_t cur = type->array[i];
        if (!ecs_id_match(cur, id)) {
            continue;
        }

        if ((err = flecs_guard_remove_id(world, entity, cur))) {
            return err;
        }
    }

    return NULL;
}

static char* flecs_guard_delete_intern(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_map_t *visited,
    int32_t depth);

static char* flecs_guard_delete_table_entities(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_map_t *visited,
    int32_t depth)
{
    const ecs_entity_t *entities = ecs_table_entities(table);
    int32_t i, count = ecs_table_count(table);
    for (i = 0; i < count; i ++) {
        char *err = flecs_guard_delete_intern(
            world, entities[i], visited, depth + 1);
        if (err) {
            return err;
        }
    }

    return NULL;
}

static char* flecs_guard_delete_usage(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_id_t pattern,
    bool is_target,
    ecs_map_t *visited,
    int32_t depth)
{
    ecs_component_record_t *cr = flecs_components_get(world, pattern);
    if (!cr) {
        return NULL;
    }

    ecs_table_cache_iter_t it;
    if (!flecs_table_cache_iter(&cr->cache, &it, EcsTableNotEmpty)) {
        return NULL;
    }

    const ecs_table_cache_elem_t *elem;
    while ((elem = flecs_table_cache_next(&it))) {
        ecs_table_t *table = elem->table;
        if (!ecs_table_count(table)) {
            continue;
        }

        const ecs_type_t *type = &table->type;
        int32_t i;
        for (i = 0; i < type->count; i ++) {
            ecs_id_t id = type->array[i];
            if (!ecs_id_match(id, pattern)) {
                continue;
            }

            ecs_component_record_t *id_cr = flecs_components_get(world, id);
            ecs_flags32_t flags = id_cr ? id_cr->flags : 0;
            ecs_entity_t action = is_target ?
                ECS_ID_ON_DELETE_TARGET(flags) : ECS_ID_ON_DELETE(flags);

            if (action == EcsPanic) {
                return flecs_asprintf("cannot delete '%s': entities with "
                    "'%s' have the (OnDelete, Panic) trait",
                        flecs_guard_path(world, e),
                        flecs_guard_id_str(world, id));
            }

            if (table->flags & EcsTableHasBuiltins) {
                return flecs_asprintf("cannot delete '%s': it is used by "
                    "builtin entities", flecs_guard_path(world, e));
            }

            if (action == EcsDelete) {
                char *err = flecs_guard_delete_table_entities(
                    world, table, visited, depth);
                if (err) {
                    return err;
                }
            }
        }
    }

    return NULL;
}

static char* flecs_guard_delete_intern(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_map_t *visited,
    int32_t depth)
{
    if (ecs_map_get(visited, e)) {
        return NULL;
    }

    ecs_map_insert(visited, e, 1);

    if (depth > FLECS_GUARD_MAX_DEPTH) {
        return flecs_asprintf("cannot delete '%s': hierarchy is too deep",
            flecs_guard_path(world, e));
    }

    if (flecs_guard_is_builtin(world, e)) {
        return flecs_asprintf("cannot delete builtin entity '%s'",
            flecs_guard_path(world, e));
    }

    if (ecs_has_pair(world, e, EcsOnDelete, EcsPanic)) {
        return flecs_asprintf("cannot delete '%s': entity has the "
            "(OnDelete, Panic) trait", flecs_guard_path(world, e));
    }

    if (flecs_guard_has_rest(world, e)) {
        return flecs_asprintf("cannot delete '%s': entity owns a REST server",
            flecs_guard_path(world, e));
    }

    if (flecs_component_is_delete_locked(world, e)) {
        return flecs_asprintf("cannot delete '%s': it is still in use by "
            "queries", flecs_guard_path(world, e));
    }

    char *err;
    if ((err = flecs_guard_delete_usage(world, e, e, false, visited, depth))) {
        return err;
    }

    if ((err = flecs_guard_delete_usage(world, e,
        ecs_pair(e, EcsWildcard), false, visited, depth)))
    {
        return err;
    }

    if ((err = flecs_guard_delete_usage(world, e,
        ecs_pair(EcsWildcard, e), true, visited, depth)))
    {
        return err;
    }

    ecs_iter_t it = ecs_children(world, e);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if ((err = flecs_guard_delete_intern(
                world, it.entities[i], visited, depth + 1)))
            {
                ecs_iter_fini(&it);
                return err;
            }
        }
    }

    return NULL;
}

char* flecs_guard_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    ecs_map_t visited;
    ecs_map_init(&visited, NULL);
    err = flecs_guard_delete_intern(world, entity, &visited, 0);
    ecs_map_fini(&visited);
    return err;
}

char* flecs_guard_enable(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    if (flecs_guard_is_builtin(world, entity)) {
        return flecs_asprintf("cannot enable or disable builtin entity '%s'",
            flecs_guard_path(world, entity));
    }

    if (flecs_guard_is_rest_module(world, entity)) {
        return flecs_asprintf("cannot enable or disable REST module '%s'",
            flecs_guard_path(world, entity));
    }

    return NULL;
}

char* flecs_guard_rename(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    if (flecs_guard_is_builtin(world, entity)) {
        return flecs_asprintf("cannot rename builtin entity '%s'",
            flecs_guard_path(world, entity));
    }

    return NULL;
}

#ifdef FLECS_SCRIPT
char* flecs_guard_script(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    char *err = flecs_guard_entity(world, entity);
    if (err) {
        return err;
    }

    if (ecs_id_is_wildcard(entity) || entity == EcsThis ||
        entity == EcsVariable)
    {
        return flecs_asprintf("cannot use '%s' as script",
            flecs_guard_path(world, entity));
    }

    if (flecs_guard_is_builtin(world, entity)) {
        return flecs_asprintf("cannot use builtin entity '%s' as script",
            flecs_guard_path(world, entity));
    }

    if ((err = flecs_guard_component(world,
        ecs_pair(ecs_id(EcsScript), entity))))
    {
        return err;
    }

    return flecs_guard_add(world, entity, ecs_id(EcsScript));
}
#endif

#endif
