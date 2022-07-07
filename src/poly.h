/**
 * @file poly.h
 * @brief Functions for managing poly objects.
 * 
 * The poly framework makes it possible to generalize common functionality for
 * different kinds of API objects, as well as improved type safety checks. Poly
 * objects have a header that identifiers what kind of object it is. This can
 * then be used to discover a set of "mixins" implemented by the type.
 * 
 * Mixins are like a vtable, but for members. Each type populates the table with
 * offsets to the members that correspond with the mixin. If an entry in the
 * mixin table is not set, the type does not support the mixin.
 * 
 * An example is the Iterable mixin, which makes it possible to create an 
 * iterator for any poly object (like filters, queries, the world) that 
 * implements the Iterable mixin.
 */

#ifndef FLECS_POLY_H
#define FLECS_POLY_H

#include <stddef.h>

/* Initialize poly */
void* _ecs_poly_init(
    ecs_poly_t *object,
    int32_t kind,
    ecs_size_t size,
    ecs_mixins_t *mixins);

#define ecs_poly_init(object, type)\
    _ecs_poly_init(object, type##_magic, sizeof(type), &type##_mixins)

/* Deinitialize object for specified type */
void _ecs_poly_fini(
    ecs_poly_t *object,
    int32_t kind);

#define ecs_poly_fini(object, type)\
    _ecs_poly_fini(object, type##_magic)

/* Utility functions for creating an object on the heap */
#define ecs_poly_new(type)\
    (type*)ecs_poly_init(ecs_os_calloc_t(type), type)

#define ecs_poly_free(obj, type)\
    ecs_poly_fini(obj, type);\
    ecs_os_free(obj)

/* Get or create poly component for an entity */
EcsPoly* _ecs_poly_bind(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_bind(world, entity, T) \
    _ecs_poly_bind(world, entity, T##_tag)

void _ecs_poly_modified(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_modified(world, entity, T) \
    _ecs_poly_modified(world, entity, T##_tag)

/* Get poly component for an entity */
const EcsPoly* _ecs_poly_bind_get(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_bind_get(world, entity, T) \
    _ecs_poly_bind_get(world, entity, T##_tag)

ecs_poly_t* _ecs_poly_get(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_get(world, entity, T) \
    ((T*)_ecs_poly_get(world, entity, T##_tag))

/* Utilities for testing/asserting an object type */
#ifndef FLECS_NDEBUG
void* _ecs_poly_assert(
    const ecs_poly_t *object,
    int32_t type,
    const char *file,
    int32_t line);

#define ecs_poly_assert(object, type)\
    _ecs_poly_assert(object, type##_magic, __FILE__, __LINE__)

#define ecs_poly(object, T) ((T*)ecs_poly_assert(object, T))
#else
#define ecs_poly_assert(object, type)
#define ecs_poly(object, T) ((T*)object)
#endif

bool _ecs_poly_is(
    const ecs_poly_t *object,
    int32_t type);

#define ecs_poly_is(object, type)\
    _ecs_poly_is(object, type##_magic)

/* Utility functions for getting a mixin from an object */
ecs_iterable_t* ecs_get_iterable(
    const ecs_poly_t *poly);

ecs_observable_t* ecs_get_observable(
    const ecs_poly_t *object);

ecs_poly_dtor_t* ecs_get_dtor(
    const ecs_poly_t *poly);

#endif
