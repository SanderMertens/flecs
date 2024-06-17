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
 */

#include "private_api.h"

static const char* mixin_kind_str[] = {
    [EcsMixinWorld] = "world",
    [EcsMixinEntity] = "entity",
    [EcsMixinObservable] = "observable",
    [EcsMixinDtor] = "dtor",
    [EcsMixinMax] = "max (should never be requested by application)"
};

ecs_mixins_t ecs_world_t_mixins = {
    .type_name = "ecs_world_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_world_t, self),
        [EcsMixinObservable] = offsetof(ecs_world_t, observable),
    }
};

ecs_mixins_t ecs_stage_t_mixins = {
    .type_name = "ecs_stage_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_stage_t, world)
    }
};

ecs_mixins_t ecs_observer_t_mixins = {
    .type_name = "ecs_observer_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_observer_t, world),
        [EcsMixinEntity] = offsetof(ecs_observer_t, entity),
        [EcsMixinDtor] = offsetof(ecs_observer_impl_t, dtor)
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
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");

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

void* flecs_poly_init_(
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
    hdr->refcount = 1;
    hdr->mixins = mixins;

    return poly;
}

void flecs_poly_fini_(
    ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)type;

    ecs_header_t *hdr = poly;

    /* Don't deinit poly that wasn't initialized */
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    ecs_assert(hdr->type == type, ECS_INVALID_PARAMETER,
        "incorrect function called to free flecs object");
    hdr->magic = 0;
}

int32_t flecs_poly_claim_(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    if (ecs_os_has_threading()) {
        return ecs_os_ainc(&hdr->refcount);
    } else {
        return ++hdr->refcount;
    }
}

int32_t flecs_poly_release_(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    if (ecs_os_has_threading()) {
        return ecs_os_adec(&hdr->refcount);
    } else {
        return --hdr->refcount;
    }
}

int32_t flecs_poly_refcount(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    return hdr->refcount;
}

EcsPoly* flecs_poly_bind_(
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
    EcsPoly *result = ecs_ensure_pair(world, entity, EcsPoly, tag);

    if (deferred) {
        ecs_defer_resume(world);
    }

    return result;
}

void flecs_poly_modified_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_modified_pair(world, entity, ecs_id(EcsPoly), tag);
}

const EcsPoly* flecs_poly_bind_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    return ecs_get_pair(world, entity, EcsPoly, tag);
}

ecs_poly_t* flecs_poly_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    const EcsPoly *p = flecs_poly_bind_get_(world, entity, tag);
    if (p) {
        return p->poly;
    }
    return NULL;
}

bool flecs_poly_is_(
    const ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_header_t *hdr = poly;
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    return hdr->type == type;    
}

ecs_observable_t* ecs_get_observable(
    const ecs_poly_t *poly)
{
    return (ecs_observable_t*)assert_mixin(poly, EcsMixinObservable);
}

const ecs_world_t* ecs_get_world(
    const ecs_poly_t *poly)
{
    if (((const ecs_header_t*)poly)->type == ecs_world_t_magic) {
        return poly;
    }
    return *(ecs_world_t**)assert_mixin(poly, EcsMixinWorld);
}

ecs_entity_t ecs_get_entity(
    const ecs_poly_t *poly)
{
    return *(ecs_entity_t*)assert_mixin(poly, EcsMixinEntity);
}

flecs_poly_dtor_t* ecs_get_dtor(
    const ecs_poly_t *poly)
{
    return (flecs_poly_dtor_t*)assert_mixin(poly, EcsMixinDtor);
}
