/**
 * @file addons/constraint_traits/constraint_traits.c
 * @brief Constraint trait support.
 */

#include "../../private_api.h"

#ifdef FLECS_CONSTRAINT_TRAITS

static
void flecs_register_final(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        if (flecs_components_get(world, ecs_pair(EcsIsA, e)) != NULL) {
            ecs_throw(ECS_INVALID_OPERATION,
                "cannot change trait 'Final' for '%s': already inherited from",
                    flecs_errstr(ecs_get_path(world, e)));
        }

        ecs_check(!flecs_component_is_trait_locked(world, e),
            ECS_INVALID_OPERATION, "cannot change trait 'Final' for '%s': "
            "already queried for (apply traits before creating queries)",
                flecs_errstr(ecs_get_path(world, e)));

    error:
        continue;
    }
}

static
void flecs_on_symmetric_add_remove(ecs_iter_t *it) {
    ecs_entity_t pair = ecs_field_id(it, 0);

    if (!ECS_HAS_ID_FLAG(pair, PAIR)) {
        /* If relationship was not added as a pair, there's nothing to do */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t rel = ECS_PAIR_FIRST(pair);
    ecs_entity_t tgt = ecs_pair_second(world, pair);
    ecs_entity_t event = it->event;

    if (tgt) {
        int i, count = it->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t subj = it->entities[i];
            if (event == EcsOnAdd) {
                if (!ecs_has_id(it->real_world, tgt, ecs_pair(rel, subj))) {
                    ecs_add_pair(it->world, tgt, rel, subj);
                }
            } else {
                if (ecs_has_id(it->real_world, tgt, ecs_pair(rel, subj))) {
                    ecs_remove_pair(it->world, tgt, rel, subj);
                }
            }
        }
    }
}

static
void flecs_register_symmetric(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t r = it->entities[i];
        flecs_assert_relation_unused(world, r, EcsSymmetric);

        /* Create observer that adds the reverse relationship when R(X, Y) is
         * added, or removes the reverse relationship when R(X, Y) is removed. */
        ecs_observer(world, {
            .entity = ecs_entity(world, { .parent = r }),
            .query.terms[0] = { .id = ecs_pair(r, EcsWildcard) },
            .callback = flecs_on_symmetric_add_remove,
            .events = {EcsOnAdd, EcsOnRemove}
        });
    }
}

#ifdef FLECS_DEBUG
static
void flecs_on_singleton_add_remove(ecs_iter_t *it) {
    ecs_entity_t component = ecs_field_id(it, 0);
    ecs_world_t *world = it->real_world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ECS_IS_PAIR(component)) {
            ecs_entity_t relationship = ECS_PAIR_FIRST(component);
            e = (uint32_t)e;
            ecs_check(relationship == e, ECS_CONSTRAINT_VIOLATED,
                "cannot add singleton pair '%s' to entity '%s': singleton "
                "component '%s' must be added to itself",
                    flecs_errstr(ecs_id_str(world, component)),
                    flecs_errstr_1(ecs_get_path(world, it->entities[i])),
                    flecs_errstr_2(ecs_get_path(it->world, relationship)));
            (void)relationship;
        } else {
            ecs_check(component == e, ECS_CONSTRAINT_VIOLATED,
                "cannot add singleton component '%s' to entity '%s': singleton"
                " component must be added to itself",
                    flecs_errstr(ecs_get_path(it->world, component)),
                    flecs_errstr_1(ecs_get_path(it->world, it->entities[i])));
        }

    error:
        continue;
    }
}
#endif

static
void flecs_register_singleton(ecs_iter_t *it) {
    flecs_register_flag_for_trait(
        it, EcsSingleton, EcsIdSingleton, 0, 0);

#ifdef FLECS_DEBUG
    ecs_world_t *world = it->real_world;
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t component = it->entities[i];

        /* Create observer that enforces that singleton is only added to self */
        ecs_observer(world, {
            .entity = ecs_entity(world, {
                .name = "debug_only_SingletonInvariantCheck", .parent = component
            }),
            .query.terms[0] = {
                .id = component, .src.id = EcsThis|EcsSelf
            },
            .callback = flecs_on_singleton_add_remove,
            .events = {EcsOnAdd}
        });

        ecs_observer(world, {
            .entity = ecs_entity(world, {
                .name = "debug_only_SingletonPairInvariantCheck", .parent = component
            }),
            .query.terms[0] = {
                .id = ecs_pair(component, EcsWildcard), .src.id = EcsThis|EcsSelf
            },
            .callback = flecs_on_singleton_add_remove,
            .events = {EcsOnAdd}
        });
    }
#endif
}

ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    if (ecs_is_alive(world, e)) {
        if (ecs_has_id(world, e, EcsOneOf)) {
            return e;
        } else {
            return ecs_get_target(world, e, EcsOneOf, 0);
        }
    } else {
        return 0;
    }
}

#ifdef FLECS_DEBUG
int flecs_check_constraint_traits(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t rel,
    ecs_entity_t tgt)
{
    if (ECS_IS_PAIR(cr->id)) {
        if (tgt) {
            /* Can't use relationship as target */
            if (ecs_has_id(world, tgt, EcsRelationship)) {
                if (!ecs_id_is_wildcard(rel) &&
                    !ecs_has_id(world, rel, EcsTrait))
                {
                    ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as target"
                        " in pair '%s': '%s' has the Relationship trait",
                            flecs_errstr(ecs_get_path(world, tgt)),
                            flecs_errstr_1(ecs_id_str(world, cr->id)),
                            flecs_errstr_2(ecs_get_path(world, tgt)));
                }
            }
        }

        if (ecs_has_id(world, rel, EcsTarget)) {
            ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as relationship "
                "in pair '%s': '%s' has the Target trait",
                    flecs_errstr(ecs_get_path(world, rel)),
                    flecs_errstr_1(ecs_id_str(world, cr->id)),
                    flecs_errstr_2(ecs_get_path(world, rel)));
        }

        if (tgt && !ecs_id_is_wildcard(tgt)) {
            /* Check if target of relationship satisfies OneOf property */
            ecs_entity_t oneof = flecs_get_oneof(world, rel);
            if (oneof) {
                if (!ecs_has_pair(world, tgt, EcsChildOf, oneof)) {
                    if (oneof == rel) {
                        ecs_throw(ECS_CONSTRAINT_VIOLATED,
                            "cannot use '%s' as target in pair '%s': "
                            "relationship '%s' has the OneOf trait and '%s' "
                            "is not a child of '%s'",
                                flecs_errstr(ecs_get_path(world, tgt)),
                                flecs_errstr_1(ecs_id_str(world, cr->id)),
                                flecs_errstr_2(ecs_get_path(world, rel)),
                                flecs_errstr_3(ecs_get_path(world, tgt)),
                                flecs_errstr_4(ecs_get_path(world, rel)));
                    } else {
                        ecs_throw(ECS_CONSTRAINT_VIOLATED,
                            "cannot use '%s' as target in pair '%s': "
                            "relationship '%s' has (OneOf, %s) and '%s' "
                            "is not a child of '%s'",
                                flecs_errstr(ecs_get_path(world, tgt)),
                                flecs_errstr_1(ecs_id_str(world, cr->id)),
                                flecs_errstr_2(ecs_get_path(world, rel)),
                                flecs_errstr_3(ecs_get_path(world, oneof)),
                                flecs_errstr_4(ecs_get_path(world, tgt)),
                                flecs_errstr_5(ecs_get_path(world, oneof)));
                    }
                }
            }

            /* Check if we're not trying to inherit from a final target */
            if (rel == EcsIsA && ecs_has_id(world, tgt, EcsFinal)) {
                ecs_throw(ECS_CONSTRAINT_VIOLATED,
                    "cannot add '(IsA, %s)': '%s' has the Final trait",
                        flecs_errstr(ecs_get_path(world, tgt)),
                        flecs_errstr_1(ecs_get_path(world, tgt)));
            }
        }
    } else {
        bool is_tgt = false;
        if (ecs_has_id(world, rel, EcsRelationship) ||
            (is_tgt = ecs_has_id(world, rel, EcsTarget)))
        {
            if (is_tgt) {
                ecs_throw(ECS_CONSTRAINT_VIOLATED,
                    "cannot use '%s' by itself: it has the Target trait and "
                    "must be used in pair with relationship",
                        flecs_errstr(ecs_get_path(world, rel)));
            } else {
                ecs_throw(ECS_CONSTRAINT_VIOLATED,
                    "cannot use '%s' by itself: it has the Relationship trait "
                    "and must be used in pair with target",
                        flecs_errstr(ecs_get_path(world, rel)));
            }
        }
    }

    return 0;
error:
    return -1;
}
#endif

void flecs_bootstrap_constraint_traits(
    ecs_world_t *world)
{
    flecs_bootstrap_make_alive(world, EcsFinal);
    flecs_bootstrap_make_alive(world, EcsTrait);
    flecs_bootstrap_make_alive(world, EcsRelationship);
    flecs_bootstrap_make_alive(world, EcsTarget);
    
    /* Traits */
    flecs_bootstrap_trait(world, EcsSymmetric);
    flecs_bootstrap_trait(world, EcsSingleton);
    flecs_bootstrap_trait(world, EcsFinal);
    flecs_bootstrap_trait(world, EcsAcyclic);
    flecs_bootstrap_trait(world, EcsOneOf);
    flecs_bootstrap_trait(world, EcsTrait);
    flecs_bootstrap_trait(world, EcsRelationship);
    flecs_bootstrap_trait(world, EcsTarget);

    /* Observers */
    ecs_observer(world, {
        .query.terms = {{ .id = EcsFinal }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_final,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsSymmetric }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_symmetric,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsSingleton }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_singleton,
        .global_observer = true
    });

    /* Properties */
    ecs_add_id(world, EcsChildOf, EcsTrait);
    ecs_add_id(world, EcsIsA, EcsTrait);
    ecs_add_id(world, EcsChildOf, EcsAcyclic);

    /* Relationships */
    ecs_add_id(world, EcsChildOf, EcsRelationship);
    ecs_add_id(world, EcsIsA, EcsRelationship);
    ecs_add_id(world, EcsSlotOf, EcsRelationship);
    ecs_add_id(world, EcsDependsOn, EcsRelationship);
    ecs_add_id(world, EcsWith, EcsRelationship);
    ecs_add_id(world, EcsOnDelete, EcsRelationship);
    ecs_add_id(world, EcsOnDeleteTarget, EcsRelationship);
    ecs_add_id(world, EcsOnInstantiate, EcsRelationship);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsRelationship);

    /* Targets */
    ecs_add_id(world, EcsOverride, EcsTarget);
    ecs_add_id(world, EcsInherit, EcsTarget);
    ecs_add_id(world, EcsDontInherit, EcsTarget);

    /* Traversable relationships are always acyclic */
    ecs_add_pair(world, EcsTraversable, EcsWith, EcsAcyclic);

    /* Modules are singletons */
    ecs_add_pair(world, EcsModule, EcsWith, EcsSingleton);

    /* Acyclic relationships */
    ecs_add_id(world, EcsWith, EcsAcyclic);

    /* Exclusive properties */
    ecs_add_id(world, EcsOneOf, EcsExclusive);
}

#endif
