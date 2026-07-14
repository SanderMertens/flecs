/**
 * @file addons/constraint_traits.h
 * @brief Constraint traits addon.
 */

#ifdef FLECS_CONSTRAINT_TRAITS

#ifndef FLECS_CONSTRAINT_TRAITS_H
#define FLECS_CONSTRAINT_TRAITS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup c_addons_constraint_traits Constraint Traits
 * @ingroup c_addons
 * Traits that enforce constraints on entities, components and relationships.
 *
 * @{
 */

/** Ensure that an entity or component cannot be used as a target in an `IsA` relationship.
 * Final can improve the performance of queries as they will not attempt to
 * substitute a final component with its subsets.
 *
 * Behavior:
 *
 * @code
 *   if IsA(X, Y) and Final(Y) throw error
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsFinal;

/** Mark relationship as commutative.
 * Behavior:
 *
 * @code
 *   if R(X, Y) then R(Y, X)
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsSymmetric;

/** Mark a relationship as acyclic. Acyclic relationships may not form cycles. */
FLECS_API extern const ecs_entity_t EcsAcyclic;

/** Ensure that a relationship target is a child of the specified entity.
 *
 * Behavior:
 *
 * @code
 *   If OneOf(R, O) and R(X, Y), Y must be a child of O
 *   If OneOf(R) and R(X, Y), Y must be a child of R
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsOneOf;

/** Can be added to components to indicate it is a trait. Traits are components
 * and/or tags that are added to other components to modify their behavior.
 */
FLECS_API extern const ecs_entity_t EcsTrait;

/** Ensure that an entity is always used in a pair as a relationship.
 *
 * Behavior:
 *
 * @code
 *   e.add(R) panics
 *   e.add(X, R) panics, unless X has the "Trait" trait
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsRelationship;

/** Ensure that an entity is always used in a pair as a target.
 *
 * Behavior:
 *
 * @code
 *   e.add(T) panics
 *   e.add(T, X) panics
 * @endcode
 */
FLECS_API extern const ecs_entity_t EcsTarget;

/** Mark component as singleton. Singleton components may only be added to
 * themselves. */
FLECS_API extern const ecs_entity_t EcsSingleton;

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_CONSTRAINT_TRAITS
