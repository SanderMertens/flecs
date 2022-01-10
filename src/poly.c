#include "private_api.h"
#include <stddef.h>

static const char* mixin_kind_str[] = {
    [EcsMixinBase] = "base (should never be requested by application)",
    [EcsMixinWorld] = "world",
    [EcsMixinObservable] = "observable",
    [EcsMixinIterable] = "iterable",
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
        [EcsMixinBase] = offsetof(ecs_stage_t, world),
        [EcsMixinWorld] = offsetof(ecs_stage_t, world)
    }
};

ecs_mixins_t ecs_query_t_mixins = {
    .type_name = "ecs_query_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_query_t, world),
        [EcsMixinIterable] = offsetof(ecs_query_t, iterable)
    }
};

ecs_mixins_t ecs_filter_t_mixins = {
    .type_name = "ecs_filter_t",
    .elems = {
        [EcsMixinIterable] = offsetof(ecs_filter_t, iterable)
    }
};

static
void* get_mixin(
    const ecs_poly_t *poly,
    ecs_mixin_kind_t kind)
{
    ecs_assert(poly != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(kind < EcsMixinMax, ECS_INVALID_PARAMETER, NULL);
    
    const ecs_header_t *hdr = poly;
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(hdr->magic == ECS_OBJECT_MAGIC, ECS_INVALID_PARAMETER, NULL);

    const ecs_mixins_t *mixins = hdr->mixins;
    if (!mixins) {
        /* Object has no mixins */
        goto not_found;
    }

    ecs_size_t offset = mixins->elems[kind];
    if (offset == 0) {
        /* Object has mixins but not the requested one. Try to find the mixin
         * in the poly's base */
        goto find_in_base;
    }

    /* Object has mixin, return its address */
    return ECS_OFFSET(hdr, offset);

find_in_base:    
    if (offset) {
        /* If the poly has a base, try to find the mixin in the base */
        ecs_poly_t *base = *(ecs_poly_t**)ECS_OFFSET(hdr, offset);
        if (base) {
            return get_mixin(base, kind);
        }
    }
    
not_found:
    /* Mixin wasn't found for poly */
    return NULL;
}

static
void* assert_mixin(
    const ecs_poly_t *poly,
    ecs_mixin_kind_t kind)
{
    void *ptr = get_mixin(poly, kind);
    if (!ptr) {
        const ecs_header_t *header = poly;
        const ecs_mixins_t *mixins = header->mixins;
        ecs_err("%s not available for type %s", 
            mixin_kind_str[kind],
            mixins ? mixins->type_name : "unknown");
        ecs_os_abort();
    }

    return ptr;
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

#define assert_object(cond, file, line)\
    _ecs_assert((cond), ECS_INVALID_PARAMETER, #cond, file, line, NULL);\
    assert(cond)

#ifndef NDEBUG
void _ecs_poly_assert(
    const ecs_poly_t *poly,
    int32_t type,
    const char *file,
    int32_t line)
{
    assert_object(poly != NULL, file, line);
    
    const ecs_header_t *hdr = poly;
    assert_object(hdr->magic == ECS_OBJECT_MAGIC, file, line);
    assert_object(hdr->type == type, file, line);
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
    return *(ecs_world_t**)assert_mixin(poly, EcsMixinWorld);
}
