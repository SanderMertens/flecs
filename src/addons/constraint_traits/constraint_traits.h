/**
 * @file constraint_traits.h
 * @brief Component traits for enforcing constraints.
 */

#ifndef FLECS_CONSTRAINT_TRAITS_PRIVATE_H
#define FLECS_CONSTRAINT_TRAITS_PRIVATE_H

#ifdef FLECS_CONSTRAINT_TRAITS

void flecs_assert_relation_unused(
    ecs_world_t *world,
    ecs_entity_t rel,
    ecs_entity_t trait);

void flecs_register_flag_for_trait(
    ecs_iter_t *it,
    ecs_entity_t trait,
    ecs_flags32_t flag,
    ecs_flags32_t not_flag,
    ecs_flags32_t entity_flag);

/* Get entity that should be used for OneOf constraint from relationship. */
ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_bootstrap_constraint_traits(
    ecs_world_t *world);

#ifdef FLECS_DEBUG
int flecs_check_constraint_traits(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t rel,
    ecs_entity_t tgt);
#else
#define flecs_check_constraint_traits(world, cr, rel, tgt) (0)
#endif

#else

#define flecs_get_oneof(world, e) ((void)(world), (void)(e), 0)
#define flecs_bootstrap_constraint_traits(world) (void)world
#define flecs_check_constraint_traits(world, cr, rel, tgt) (0)

#endif

#ifdef FLECS_CONSTRAINT_TRAITS
#define flecs_bootstrap_trait(world, name)\
    flecs_bootstrap_tag(world, name)\
    ecs_add_id(world, name, EcsTrait)
#else
#define flecs_bootstrap_trait(world, name)\
    flecs_bootstrap_tag(world, name)
#endif

#endif
