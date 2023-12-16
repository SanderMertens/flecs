/**
 * @file poly.h
 * @brief Functions for managing poly objects.
 */

#ifndef FLECS_POLY_H
#define FLECS_POLY_H

#include <stddef.h>

/* Initialize poly */
void* ecs_poly_init_(
    ecs_poly_t *object,
    int32_t kind,
    ecs_size_t size,
    ecs_mixins_t *mixins);

#define ecs_poly_init(object, type)\
    ecs_poly_init_(object, type##_magic, sizeof(type), &type##_mixins)

/* Deinitialize object for specified type */
void ecs_poly_fini_(
    ecs_poly_t *object,
    int32_t kind);

#define ecs_poly_fini(object, type)\
    ecs_poly_fini_(object, type##_magic)

/* Utility functions for creating an object on the heap */
#define ecs_poly_new(type)\
    (type*)ecs_poly_init(ecs_os_calloc_t(type), type)

#define ecs_poly_free(obj, type)\
    ecs_poly_fini(obj, type);\
    ecs_os_free(obj)

/* Get or create poly component for an entity */
EcsPoly* ecs_poly_bind_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_bind(world, entity, T) \
    ecs_poly_bind_(world, entity, T##_tag)

void ecs_poly_modified_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_modified(world, entity, T) \
    ecs_poly_modified_(world, entity, T##_tag)

/* Get poly component for an entity */
const EcsPoly* ecs_poly_bind_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_bind_get(world, entity, T) \
    ecs_poly_bind_get_(world, entity, T##_tag)

ecs_poly_t* ecs_poly_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define ecs_poly_get(world, entity, T) \
    ((T*)ecs_poly_get_(world, entity, T##_tag))

/* Utilities for testing/asserting an object type */
#ifndef FLECS_NDEBUG
#define ecs_poly_assert(object, ty)\
    do {\
        ecs_assert(object != NULL, ECS_INVALID_PARAMETER, NULL);\
        const ecs_header_t *hdr = (const ecs_header_t *)object;\
        const char *type_name = hdr->mixins->type_name;\
        ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER, type_name);\
        ecs_assert(hdr->type == ty##_magic, ECS_INVALID_PARAMETER, type_name);\
    } while (0)
#else
#define ecs_poly_assert(object, ty)
#endif

/* Utility functions for getting a mixin from an object */
ecs_iterable_t* ecs_get_iterable(
    const ecs_poly_t *poly);

ecs_observable_t* ecs_get_observable(
    const ecs_poly_t *object);

ecs_poly_dtor_t* ecs_get_dtor(
    const ecs_poly_t *poly);

#endif
