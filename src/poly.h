/**
 * @file poly.h
 * @brief Functions for managing poly objects.
 */

#ifndef FLECS_POLY_H
#define FLECS_POLY_H

/* Tags associated with poly for (Poly, tag) components */
#define ecs_world_t_tag     invalid
#define ecs_stage_t_tag     invalid
#define ecs_query_t_tag     EcsQuery
#define ecs_observer_t_tag  EcsObserver

/* Mixin kinds */
typedef enum ecs_mixin_kind_t {
    EcsMixinWorld,
    EcsMixinEntity,
    EcsMixinObservable,
    EcsMixinDtor,
    EcsMixinMax
} ecs_mixin_kind_t;

/* The mixin array contains pointers to mixin members for different kinds of
 * flecs objects. This allows the API to retrieve data from an object regardless
 * of its type. Each mixin array is only stored once per type */
struct ecs_mixins_t {
    const char *type_name; /* Include name of mixin type so debug code doesn't
                            * need to know about every object */
    ecs_size_t elems[EcsMixinMax];                        
};

/* Mixin tables */
extern ecs_mixins_t ecs_world_t_mixins;
extern ecs_mixins_t ecs_stage_t_mixins;
extern ecs_mixins_t ecs_query_t_mixins;
extern ecs_mixins_t ecs_observer_t_mixins;

/* Types that have no mixins */
#define ecs_table_t_mixins (&(ecs_mixins_t){ NULL })

/* Initialize poly */
void* flecs_poly_init_(
    ecs_poly_t *object,
    int32_t kind,
    ecs_size_t size,
    ecs_mixins_t *mixins);

#define flecs_poly_init(object, type)\
    flecs_poly_init_(object, type##_magic, sizeof(type), &type##_mixins)

/* Deinitialize object for specified type */
void flecs_poly_fini_(
    ecs_poly_t *object,
    int32_t kind);

#define flecs_poly_fini(object, type)\
    flecs_poly_fini_(object, type##_magic)

/* Utility functions for creating an object on the heap */
#define flecs_poly_new(type)\
    (type*)flecs_poly_init(ecs_os_calloc_t(type), type)

#define flecs_poly_free(obj, type)\
    flecs_poly_fini(obj, type);\
    ecs_os_free(obj)

/* Get or create poly component for an entity */
EcsPoly* flecs_poly_bind_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define flecs_poly_bind(world, entity, T) \
    flecs_poly_bind_(world, entity, T##_tag)

/* Send modified event for (Poly, Tag) pair. */
void flecs_poly_modified_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define flecs_poly_modified(world, entity, T) \
    flecs_poly_modified_(world, entity, T##_tag)

/* Get poly component for an entity */
const EcsPoly* flecs_poly_bind_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define flecs_poly_bind_get(world, entity, T) \
    flecs_poly_bind_get_(world, entity, T##_tag)

/* Get (Poly, Tag) poly object from entity. */
ecs_poly_t* flecs_poly_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag);

#define flecs_poly_get(world, entity, T) \
    ((T*)flecs_poly_get_(world, entity, T##_tag))

/* Utilities for testing/asserting an object type */
#ifndef FLECS_NDEBUG
#define flecs_poly_assert(object, ty)\
    do {\
        ecs_assert(object != NULL, ECS_INVALID_PARAMETER, NULL);\
        const ecs_header_t *hdr = (const ecs_header_t *)object;\
        const char *type_name = hdr->mixins->type_name;\
        ecs_assert(hdr->type == ty##_magic, ECS_INVALID_PARAMETER, type_name);\
    } while (0)
#else
#define flecs_poly_assert(object, ty)
#endif

/* Get observable mixin from poly object. */
ecs_observable_t* flecs_get_observable(
    const ecs_poly_t *object);

/* Get dtor mixin from poly object. */
flecs_poly_dtor_t* flecs_get_dtor(
    const ecs_poly_t *poly);

#endif
