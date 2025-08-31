/**
 * @file addons/meta/type_support/units_ts.c
 * @brief Units type support.
 */

#include "type_support.h"

#ifdef FLECS_META

/* EcsUnit lifecycle */

static void flecs_unit_dtor(
    EcsUnit *ptr) 
{
    ecs_os_free(ptr->symbol);
}

static ECS_COPY(EcsUnit, dst, src, {
    flecs_unit_dtor(dst);
    dst->symbol = ecs_os_strdup(src->symbol);
    dst->base = src->base;
    dst->over = src->over;
    dst->prefix = src->prefix;
    dst->translation = src->translation;
})

static ECS_MOVE(EcsUnit, dst, src, {
    flecs_unit_dtor(dst);
    dst->symbol = src->symbol;
    dst->base = src->base;
    dst->over = src->over;
    dst->prefix = src->prefix;
    dst->translation = src->translation;

    src->symbol = NULL;
    src->base = 0;
    src->over = 0;
    src->prefix = 0;
    src->translation = (ecs_unit_translation_t){0};
})

static ECS_DTOR(EcsUnit, ptr, { flecs_unit_dtor(ptr); })


/* EcsUnitPrefix lifecycle */

static void flecs_unit_prefix_dtor(
    EcsUnitPrefix *ptr) 
{
    ecs_os_free(ptr->symbol);
}

static ECS_COPY(EcsUnitPrefix, dst, src, {
    flecs_unit_prefix_dtor(dst);
    dst->symbol = ecs_os_strdup(src->symbol);
    dst->translation = src->translation;
})

static ECS_MOVE(EcsUnitPrefix, dst, src, {
    flecs_unit_prefix_dtor(dst);
    dst->symbol = src->symbol;
    dst->translation = src->translation;

    src->symbol = NULL;
    src->translation = (ecs_unit_translation_t){0};
})

static ECS_DTOR(EcsUnitPrefix, ptr, { flecs_unit_prefix_dtor(ptr); })

bool flecs_unit_validate(
    ecs_world_t *world,
    ecs_entity_t t,
    EcsUnit *data)
{
    char *derived_symbol = NULL;
    const char *symbol = data->symbol;

    ecs_entity_t base = data->base;
    ecs_entity_t over = data->over;
    ecs_entity_t prefix = data->prefix;
    ecs_unit_translation_t translation = data->translation;

    if (base) {
        if (!ecs_has(world, base, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as base is not a unit",
                ecs_get_name(world, base), ecs_get_name(world, t));
            goto error;
        }
    }

    if (over) {
        if (!base) {
            ecs_err("invalid unit '%s': cannot specify over without base",
                ecs_get_name(world, t));
            goto error;
        }
        if (!ecs_has(world, over, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as over is not a unit",
                ecs_get_name(world, over), ecs_get_name(world, t));
            goto error;
        }
    }

    if (prefix) {
        if (!base) {
            ecs_err("invalid unit '%s': cannot specify prefix without base",
                ecs_get_name(world, t));
            goto error;
        }
        const EcsUnitPrefix *prefix_ptr = ecs_get(world, prefix, EcsUnitPrefix);
        if (!prefix_ptr) {
            ecs_err("entity '%s' for unit '%s' used as prefix is not a prefix",
                ecs_get_name(world, over), ecs_get_name(world, t));
            goto error;
        }

        if (translation.factor || translation.power) {
            if (prefix_ptr->translation.factor != translation.factor ||
                prefix_ptr->translation.power != translation.power)
            {
                ecs_err(
                    "factor for unit '%s' is inconsistent with prefix '%s'",
                    ecs_get_name(world, t), ecs_get_name(world, prefix));
                goto error;
            }
        } else {
            translation = prefix_ptr->translation;
        }
    }

    if (base) {
        bool must_match = false; /* Must base symbol match symbol? */
        ecs_strbuf_t sbuf = ECS_STRBUF_INIT;
        if (prefix) {
            const EcsUnitPrefix *ptr = ecs_get(world, prefix, EcsUnitPrefix);
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (ptr->symbol) {
                ecs_strbuf_appendstr(&sbuf, ptr->symbol);
                must_match = true;
            }
        }

        const EcsUnit *uptr = ecs_get(world, base, EcsUnit);
        ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
        if (uptr->symbol) {
            ecs_strbuf_appendstr(&sbuf, uptr->symbol);
        }

        if (over) {
            uptr = ecs_get(world, over, EcsUnit);
            ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (uptr->symbol) {
                ecs_strbuf_appendch(&sbuf, '/');
                ecs_strbuf_appendstr(&sbuf, uptr->symbol);
                must_match = true;
            }
        }

        derived_symbol = ecs_strbuf_get(&sbuf);
        if (derived_symbol && !ecs_os_strlen(derived_symbol)) {
            ecs_os_free(derived_symbol);
            derived_symbol = NULL;
        }

        if (derived_symbol && symbol && ecs_os_strcmp(symbol, derived_symbol)) {
            if (must_match) {
                ecs_err("symbol '%s' for unit '%s' does not match base"
                    " symbol '%s'", symbol, 
                        ecs_get_name(world, t), derived_symbol);
                goto error;
            }
        }
        if (!symbol && derived_symbol && (prefix || over)) {
            ecs_os_free(data->symbol);
            data->symbol = derived_symbol;
        } else {
            ecs_os_free(derived_symbol);
        }
    }

    data->base = base;
    data->over = over;
    data->prefix = prefix;
    data->translation = translation;

    return true;
error:
    ecs_os_free(derived_symbol);
    return false;
}

static
void flecs_set_unit(ecs_iter_t *it) {
    EcsUnit *u = ecs_field(it, EcsUnit, 0);

    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        flecs_unit_validate(world, e, &u[i]);
    }
}

static
void flecs_unit_quantity_monitor(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    if (it->event == EcsOnAdd) {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = it->entities[i];
            ecs_add_pair(world, e, EcsQuantity, e);
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = it->entities[i];
            ecs_remove_pair(world, e, EcsQuantity, e);
        }
    }
}

ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_entity_t quantity = desc->quantity;
    if (quantity) {
        if (!ecs_has_id(world, quantity, EcsQuantity)) {
            ecs_err("entity '%s' for unit '%s' is not a quantity",
                ecs_get_name(world, quantity), ecs_get_name(world, t));
            goto error;
        }

        ecs_add_pair(world, t, EcsQuantity, desc->quantity);
    } else {
        ecs_remove_pair(world, t, EcsQuantity, EcsWildcard);
    }

    EcsUnit *value = ecs_ensure(world, t, EcsUnit);
    value->base = desc->base;
    value->over = desc->over;
    value->translation = desc->translation;
    value->prefix = desc->prefix;
    ecs_os_strset(&value->symbol, desc->symbol);

    if (!flecs_unit_validate(world, t, value)) {
        goto error;
    }

    ecs_modified(world, t, EcsUnit);

    flecs_resume_readonly(world, &rs);
    return t;
error:
    if (t) {
        ecs_delete(world, t);
    }
    flecs_resume_readonly(world, &rs);
    return 0;
}

ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsUnitPrefix, {
        .symbol = ECS_CONST_CAST(char*, desc->symbol),
        .translation = desc->translation
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = ecs_entity_init(world, desc);
    if (!t) {
        return 0;
    }

    ecs_add_id(world, t, EcsQuantity);

    flecs_resume_readonly(world, &rs);

    return t;
}

void flecs_meta_units_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsUnit),
            .name = "unit", .symbol = "EcsUnit",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsInherit))
        }),
        .type.size = sizeof(EcsUnit),
        .type.alignment = ECS_ALIGNOF(EcsUnit)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsUnitPrefix),
            .name = "unit_prefix", .symbol = "EcsUnitPrefix",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsInherit))
        }),
        .type.size = sizeof(EcsUnitPrefix),
        .type.alignment = ECS_ALIGNOF(EcsUnitPrefix)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = EcsQuantity,
            .name = "quantity", .symbol = "EcsQuantity",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsInherit))
        })
    });

    ecs_set_hooks(world, EcsUnit, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsUnit),
        .copy = ecs_copy(EcsUnit),
        .dtor = ecs_dtor(EcsUnit)
    });

    ecs_set_hooks(world, EcsUnitPrefix, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsUnitPrefix),
        .copy = ecs_copy(EcsUnitPrefix),
        .dtor = ecs_dtor(EcsUnitPrefix)
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsUnit) },
        .events = {EcsOnSet},
        .callback = flecs_set_unit
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms = {
            { .id = ecs_id(EcsUnit) },
            { .id = EcsQuantity }
        },
        .events = { EcsMonitor },
        .callback = flecs_unit_quantity_monitor
    });

    ecs_add_id(world, EcsQuantity, EcsExclusive);
    ecs_add_id(world, EcsQuantity, EcsPairIsTag);
}

#endif
