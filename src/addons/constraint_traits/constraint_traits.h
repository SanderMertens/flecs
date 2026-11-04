/**
 * @file constraint_traits.h
 * @brief Component traits for enforcing constraints.
 */

#ifndef FLECS_CONSTRAINT_TRAITS_PRIVATE_H
#define FLECS_CONSTRAINT_TRAITS_PRIVATE_H

void flecs_register_flag_for_trait(
    ecs_iter_t *it,
    ecs_entity_t trait,
    ecs_flags32_t flag,
    ecs_flags32_t not_flag,
    ecs_flags32_t entity_flag);

#ifdef FLECS_CONSTRAINT_TRAITS

/* Get entity that should be used for OneOf constraint from relationship. */
ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_bootstrap_constraint_traits(
    ecs_world_t *world);

char* flecs_constraint_traits_error(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_entity_t tgt);

char* flecs_singleton_constraint_error(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

#else

#define flecs_get_oneof(world, e) ((void)(world), (void)(e), 0)
#define flecs_bootstrap_constraint_traits(world) (void)world
#define flecs_constraint_traits_error(world, id, rel, tgt) (NULL)
#define flecs_singleton_constraint_error(world, entity, id) (NULL)

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
