#include "meta.h"

#ifdef FLECS_META

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    return t;
}

ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_add(world, t, EcsEnum);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_enum_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_object(world, c, EcsConstant, ecs_i32_t, 
                {m_desc->value});
        }
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("enum '%s' has no constants", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_add(world, t, EcsBitmask);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_bitmask_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_object(world, c, EcsConstant, ecs_u32_t, 
                {m_desc->value});
        }
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("bitmask '%s' has no constants", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsArray, {
        .type = desc->type,
        .count = desc->count
    });

    return t;
}

ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsVector, {
        .type = desc->type
    });

    return t;
}

ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_member_t *m_desc = &desc->members[i];
        if (!m_desc->type) {
            break;
        }

        if (!m_desc->name) {
            ecs_err("member %d of struct '%s' does not have a name", i, 
                ecs_get_name(world, t));
            ecs_delete(world, t);
            return 0;
        }

        ecs_entity_t m = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        ecs_set(world, m, EcsMember, {
            .type = m_desc->type, 
            .count = m_desc->count,
            .unit = m_desc->unit
        });
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("struct '%s' has no members", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    if (!ecs_has(world, t, EcsStruct)) {
        /* Invalid members */
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc)
{
    char *derived_symbol = NULL;
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        goto error;
    }

    const char *symbol = desc->symbol;

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

    ecs_entity_t derived = desc->derived;
    if (derived) {
        if (!ecs_has(world, derived, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as derived is not a unit",
                ecs_get_name(world, derived), ecs_get_name(world, t));
            goto error;
        }
    }

    ecs_entity_t over = desc->over;
    if (over) {
        if (!derived) {
            ecs_err("invalid unit '%s': cannot specify over without derived",
                ecs_get_name(world, t));
            goto error;
        }
        if (!ecs_has(world, over, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as over is not a unit",
                ecs_get_name(world, over), ecs_get_name(world, t));
            goto error;
        }
    }

    ecs_unit_translation_t translation = desc->translation;

    ecs_entity_t prefix = desc->prefix;
    if (prefix) {
        if (!derived) {
            ecs_err("invalid unit '%s': cannot specify prefix without derived",
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

    if (derived) {
        bool must_match = false; /* Must derived symbol match symbol? */
        ecs_strbuf_t sbuf = ECS_STRBUF_INIT;
        if (prefix) {
            const EcsUnitPrefix *ptr = ecs_get(world, prefix, EcsUnitPrefix);
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (ptr->symbol) {
                ecs_strbuf_appendstr(&sbuf, ptr->symbol);
                must_match = true;
            }
        }

        const EcsUnit *uptr = ecs_get(world, derived, EcsUnit);
        ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
        if (uptr->symbol) {
            ecs_strbuf_appendstr(&sbuf, uptr->symbol);
        }

        if (over) {
            uptr = ecs_get(world, over, EcsUnit);
            ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (uptr->symbol) {
                ecs_strbuf_appendstr(&sbuf, "/");
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
                ecs_err("symbol '%s' for unit '%s' does not match derived"
                    " symbol '%s'", symbol, 
                        ecs_get_name(world, t), derived_symbol);
                goto error;
            }
        }
        if (!symbol && derived_symbol && (prefix || over)) {
            symbol = derived_symbol;
        }
    }

    ecs_set(world, t, EcsUnit, {
        .symbol = (char*)symbol,
        .derived = derived,
        .over = over,
        .prefix = prefix,
        .translation = translation
    });

    ecs_os_free(derived_symbol);

    return t;
error:
    if (t) {
        ecs_delete(world, t);
    }
    ecs_os_free(derived_symbol);
    return 0;
}

ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsUnitPrefix, {
        .symbol = (char*)desc->symbol,
        .translation = desc->translation
    });

    return t;
}

ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, desc);
    if (!t) {
        return 0;
    }

    ecs_add_id(world, t, EcsQuantity);

    return t;
}

#endif
