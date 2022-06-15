/**
 * @file poly.h
 * @brief Functions for managing poly objects.
 */

#ifndef FLECS_POLY_H
#define FLECS_POLY_H

/* Initialize object header & mixins for specified type */
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

/* Bind entity with poly object */
void ecs_poly_bind(
    ecs_poly_t *poly,
    ecs_entity_t kind,
    ecs_entity_t entity);

/* Get or create poly component for an entity */
EcsPoly* ecs_poly_bind_ensure(
    ecs_world_t *world,
    ecs_entity_t kind,
    ecs_entity_t entity);

/* Get poly component for an entity */
const EcsPoly* ecs_poly_bind_get(
    const ecs_world_t *world,
    ecs_entity_t kind,
    ecs_entity_t entity);

/* Create entity for poly */
ecs_entity_t ecs_poly_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc);

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
#define ecs_poly(object, T) object
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
