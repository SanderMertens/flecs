/**
 * @file poly.c
 * @brief Polymorphic object framework.
 *
 * Poly objects have a header identifying their type, which maps to a mixin
 * table of member offsets. This enables type-safe generic access to fields
 * like world, entity, observable, and dtor across different object types.
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

/* Assert that a poly object supports the given mixin and return its address. */
static
void* assert_mixin(
    const ecs_poly_t *poly,
    ecs_mixin_kind_t kind)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(kind < EcsMixinMax, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_header_t *hdr = poly;
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(hdr->type != 0, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");

    const ecs_mixins_t *mixins = hdr->mixins;
    ecs_assert(mixins != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_size_t offset = mixins->elems[kind];
    ecs_assert(offset != 0, ECS_INVALID_PARAMETER, 
        "mixin %s not available for type %s",
            mixin_kind_str[kind], mixins ? mixins->type_name : "unknown");
    (void)mixin_kind_str;

    return ECS_OFFSET(hdr, offset);
}

/* Initialize a poly object with type, size, and mixin table. */
void* flecs_poly_init_(
    ecs_poly_t *poly,
    int32_t type,
    ecs_size_t size,
    ecs_mixins_t *mixins)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_header_t *hdr = poly;
    ecs_os_memset(poly, 0, size);

    hdr->type = type;
    hdr->refcount = 1;
    hdr->mixins = mixins;

    return poly;
}

/* Deinitialize a poly object for the specified type. */
void flecs_poly_fini_(
    ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)type;

    ecs_header_t *hdr = poly;

    ecs_assert(hdr->type == type, ECS_INVALID_PARAMETER,
        "incorrect function called to free flecs object");
    hdr->type = 0;
}

/* Increment the reference count of a poly object. */
int32_t flecs_poly_claim_(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->type != 0, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    if (ecs_os_has_threading()) {
        return ecs_os_ainc(&hdr->refcount);
    } else {
        return ++hdr->refcount;
    }
}

/* Decrement the reference count of a poly object. */
int32_t flecs_poly_release_(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->type != 0, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    if (ecs_os_has_threading()) {
        return ecs_os_adec(&hdr->refcount);
    } else {
        return --hdr->refcount;
    }
}

/* Return the current reference count of a poly object. */
int32_t flecs_poly_refcount(
    ecs_poly_t *poly)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_header_t *hdr = poly;
    ecs_assert(hdr->type != 0, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    return hdr->refcount;
}

/* Get or create poly component for an entity. */
EcsPoly* flecs_poly_bind_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    if (!ecs_has_id(world, entity, tag)) {
        ecs_add_id(world, entity, tag);
    }

    /* Poly creation must not be deferred */
    bool deferred = false;
    if (ecs_is_deferred(world)) {
        deferred = true;
        ecs_defer_suspend(world);
    }

    EcsPoly *result = ecs_ensure_pair(world, entity, EcsPoly, tag);

    if (deferred) {
        ecs_defer_resume(world);
    }

    return result;
}

/* Send modified event for (Poly, Tag) pair. */
void flecs_poly_modified_(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    ecs_modified_pair(world, entity, ecs_id(EcsPoly), tag);
}

/* Get poly component for an entity. */
const EcsPoly* flecs_poly_bind_get_(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t tag)
{
    return ecs_get_pair(world, entity, EcsPoly, tag);
}

/* Get poly object from (Poly, Tag) pair on entity. */
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

/* Check if a poly object is of the specified type. */
bool flecs_poly_is_(
    const ecs_poly_t *poly,
    int32_t type)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_header_t *hdr = poly;
    ecs_assert(hdr->type != 0, ECS_INVALID_PARAMETER,
        "invalid/freed pointer to flecs object detected");
    return hdr->type == type;    
}

/* Get observable mixin from poly object. */
ecs_observable_t* flecs_get_observable(
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

/* Get dtor mixin from poly object. */
flecs_poly_dtor_t* flecs_get_dtor(
    const ecs_poly_t *poly)
{
    return (flecs_poly_dtor_t*)assert_mixin(poly, EcsMixinDtor);
}
