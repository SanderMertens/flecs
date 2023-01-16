/**
 * @file poly.c
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

#include "private_api.h"

static const char* mixin_kind_str[] = {
    [EcsMixinWorld] = "world",
    [EcsMixinEntity] = "entity",
    [EcsMixinObservable] = "observable",
    [EcsMixinIterable] = "iterable",
    [EcsMixinDtor] = "dtor",
    [EcsMixinMax] = "max (should never be requested by application)"
};

ecs_mixins_t ecs_world_t_mixins = {
    .type_name = "ecs_world_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_world_t, self),
        [EcsMixinObservable] = offsetof(ecs_world_t, observable),
        [EcsMixinIterable] = offsetof(ecs_world_t, iterable)
    }
};

ecs_mixins_t ecs_stage_t_mixins = {
    .type_name = "ecs_stage_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_stage_t, world)
    }
};

ecs_mixins_t ecs_query_t_mixins = {
    .type_name = "ecs_query_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_query_t, filter.world),
        [EcsMixinEntity] = offsetof(ecs_query_t, filter.entity),
        [EcsMixinIterable] = offsetof(ecs_query_t, iterable),
        [EcsMixinDtor] = offsetof(ecs_query_t, dtor)
    }
};

ecs_mixins_t ecs_observer_t_mixins = {
    .type_name = "ecs_observer_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_observer_t, filter.world),
        [EcsMixinEntity] = offsetof(ecs_observer_t, filter.entity),
        [EcsMixinDtor] = offsetof(ecs_observer_t, dtor)
    }
};

ecs_mixins_t ecs_filter_t_mixins = {
    .type_name = "ecs_filter_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_filter_t, world),
        [EcsMixinEntity] = offsetof(ecs_filter_t, entity),
        [EcsMixinIterable] = offsetof(ecs_filter_t, iterable),
        [EcsMixinDtor] = offsetof(ecs_filter_t, dtor)
    }
};

static
void* assert_mixin(
    const ecs_poly_t *poly,
    ecs_mixin_kind_t kind)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(kind < EcsMixinMax, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_header_t *hdr = poly;
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER, NULL);

    const ecs_mixins_t *mixins = hdr->mixins;
    ecs_assert(mixins != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_size_t offset = mixins->elems[kind];
    ecs_assert(offset != 0, ECS_INVALID_PARAMETER, 
        "mixin %s not available for type %s",
            mixin_kind_str[kind], mixins ? mixins->type_name : "unknown");
    (void)mixin_kind_str;

    /* Object has mixin, return its address */
    return ECS_OFFSET(hdr, offset);
}

void* _ecs_poly_init(
    ecs_poly_t *poly,
    int32_t type,
    ecs_size_t size,
    ecs_mixins_t *mixins)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_header_t *hdr = poly;
    ecs_os_memset(poly, 0, size);

    hdr->magic = ECS_OBJECT_MAGIC;
    hdr->type = type;
    hdr->mixins = mixins;

    return poly;
}

void _ecs_poly_fini(
    ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)type;

    ecs_header_t *hdr = poly;

    /* Don't deinit poly that wasn't initialized */
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(hdr->type == type, ECS_INVALID_PARAMETER, NULL);
    hdr->magic = 0;
}

EcsPoly* _ecs_poly_bind(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    /* Add tag to the entity for easy querying. This will make it possible to
     * query for `Query` instead of `(Poly, Query) */
    if (!ecs_has_id(world, entity, tag)) {
        ecs_add_id(world, entity, tag);
    }

    /* Never defer creation of a poly object */
    bool deferred = false;
    if (ecs_is_deferred(world)) {
        deferred = true;
        ecs_defer_suspend(world);
    }

    /* If this is a new poly, leave the actual creation up to the caller so they
     * call tell the difference between a create or an update */
    EcsPoly *result = ecs_get_mut_pair(world, entity, EcsPoly, tag);

    if (deferred) {
        ecs_defer_resume(world);
    }

    return result;
}

void _ecs_poly_modified(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_modified_pair(world, entity, ecs_id(EcsPoly), tag);
}

const EcsPoly* _ecs_poly_bind_get(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    return ecs_get_pair(world, entity, EcsPoly, tag);
}

ecs_poly_t* _ecs_poly_get(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    const EcsPoly *p = _ecs_poly_bind_get(world, entity, tag);
    if (p) {
        return p->poly;
    }
    return NULL;
}

#define assert_object(cond, file, line, type_name)\
    _ecs_assert((cond), ECS_INVALID_PARAMETER, #cond, file, line, type_name);\
    assert(cond)

#ifndef FLECS_NDEBUG
void* _ecs_poly_assert(
    const ecs_poly_t *poly,
    int32_t type,
    const char *file,
    int32_t line)
{
    assert_object(poly != NULL, file, line, 0);
    
    const ecs_header_t *hdr = poly;
    const char *type_name = hdr->mixins->type_name;
    assert_object(hdr->magic == ECS_OBJECT_MAGIC, file, line, type_name);
    assert_object(hdr->type == type, file, line, type_name);
    return (ecs_poly_t*)poly;
}
#endif

bool _ecs_poly_is(
    const ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_header_t *hdr = poly;
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER, NULL);
    return hdr->type == type;    
}

ecs_iterable_t* ecs_get_iterable(
    const ecs_poly_t *poly)
{
    return (ecs_iterable_t*)assert_mixin(poly, EcsMixinIterable);
}

ecs_observable_t* ecs_get_observable(
    const ecs_poly_t *poly)
{
    return (ecs_observable_t*)assert_mixin(poly, EcsMixinObservable);
}

const ecs_world_t* ecs_get_world(
    const ecs_poly_t *poly)
{
    if (((ecs_header_t*)poly)->type == ecs_world_t_magic) {
        return poly;
    }
    return *(ecs_world_t**)assert_mixin(poly, EcsMixinWorld);
}

ecs_entity_t ecs_get_entity(
    const ecs_poly_t *poly)
{
    return *(ecs_entity_t*)assert_mixin(poly, EcsMixinEntity);
}

ecs_poly_dtor_t* ecs_get_dtor(
    const ecs_poly_t *poly)
{
    return (ecs_poly_dtor_t*)assert_mixin(poly, EcsMixinDtor);
}
