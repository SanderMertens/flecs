/**
 * @file addons/meta/type_support/enum_ts.c
 * @brief Enum type support.
 */

#include "type_support.h"

#ifdef FLECS_META

/* EcsConstants lifecycle */

static void flecs_constants_dtor(
    ecs_map_t *constants) 
{
    ecs_map_iter_t it = ecs_map_iter(constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        ecs_os_free(ECS_CONST_CAST(char*, c->name));
        ecs_os_free(c);
    }
    ecs_map_fini(constants);
}

static void flecs_ordered_constants_dtor(
    ecs_vec_t *ordered_constants)
{
    /* shallow fini of is ok since map deallocs name c-string member */
    ecs_vec_fini_t(NULL, ordered_constants, ecs_enum_constant_t);
}

static ECS_CTOR(EcsConstants, ptr, {
    ptr->constants = ecs_os_malloc_t(ecs_map_t);
    ecs_map_init(ptr->constants, NULL);
    ecs_vec_init_t(NULL, &ptr->ordered_constants, ecs_enum_constant_t, 0);
})

static ECS_MOVE(EcsConstants, dst, src, {
    if (dst->constants) {
        flecs_constants_dtor(dst->constants);
        ecs_os_free(dst->constants);
    }

    dst->constants = src->constants;
    src->constants = NULL;

    flecs_ordered_constants_dtor(&dst->ordered_constants);
    dst->ordered_constants = src->ordered_constants;
    ecs_os_zeromem(&src->ordered_constants);
})

static ECS_DTOR(EcsConstants, ptr, {
    if (ptr->constants) {
        flecs_constants_dtor(ptr->constants);
        ecs_os_free(ptr->constants);
    }
    flecs_ordered_constants_dtor(&ptr->ordered_constants);
})

static
int flecs_add_constant_to_enum(
    ecs_world_t *world, 
    ecs_entity_t type, 
    ecs_entity_t e,
    ecs_id_t constant_id)
{
    EcsEnum *eptr = ecs_ensure(world, type, EcsEnum);
    EcsConstants *ptr = ecs_ensure(world, type, EcsConstants);
    ecs_entity_t ut = eptr->underlying_type;

    /* It's possible that a constant is added to an entity that didn't have an
     * Enum component yet. In that case derive the underlying type from the
     * first constant. */
    if (!ut) {
        if (ecs_id_is_pair(constant_id)) {
            ut = eptr->underlying_type = ecs_pair_second(world, constant_id);
        } else {
            /* Default to i32 */
            ut = ecs_id(ecs_i32_t);
        }
    }

    ecs_assert(ut != 0, ECS_INVALID_OPERATION, 
        "missing underlying type for enum");

    const EcsPrimitive *p = ecs_get(world, ut, EcsPrimitive);
    if (!p) {
        char *path = ecs_get_path(world, ut);
        ecs_err("underlying type '%s' must be a primitive type", path);
        ecs_os_free(path);
        return -1;
    }

    bool ut_is_unsigned = false;
    ecs_primitive_kind_t kind = p->kind;
    if (kind == EcsU8 || kind == EcsU16 || kind == EcsU32 || kind == EcsU64) {
        ut_is_unsigned = true;
    }

    if (!ptr->constants) {
        ptr->constants = ecs_os_malloc_t(ecs_map_t);
        ecs_map_init(ptr->constants, NULL);
    }

    /* Remove constant from map and vector if it was already added */
    ecs_map_iter_t it = ecs_map_iter(ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        if (c->constant == e) {
            ecs_os_free(ECS_CONST_CAST(char*, c->name));
            ecs_map_remove_free(ptr->constants, ecs_map_key(&it));

            ecs_enum_constant_t* constants = ecs_vec_first_t(
                &ptr->ordered_constants, ecs_enum_constant_t);
            int32_t i, count = ecs_vec_count(&ptr->ordered_constants);
            for (i = 0; i < count; i++) {
                if (constants[i].constant == e) {
                    break;
                }
            }
            if (i < count) {
                for (int j = i; j < count - 1; j++) {
                    constants[j] = constants[j + 1];
                }
                ecs_vec_remove_last(&ptr->ordered_constants);
            }
        }
    }

    /* Check if constant sets explicit value */
    int64_t value = 0;
    uint64_t value_unsigned = 0;
    bool value_set = false;
    if (ecs_id_is_pair(constant_id)) {
        ecs_value_t v = { .type = ut };
        v.ptr = ecs_get_mut_id(world, e, ecs_pair(EcsConstant, ut));

        if (!v.ptr) {
            char *has_pair = ecs_id_str(world, constant_id);
            char *expect_pair = ecs_id_str(world, ecs_pair(EcsConstant, ut));
            char *path = ecs_get_path(world, e);
            ecs_err(
                "enum constant '%s' has incorrect value pair (expected %s, got %s)",
                    path, expect_pair, has_pair);
            ecs_os_free(path);
            ecs_os_free(has_pair);
            ecs_os_free(expect_pair);
            return -1;
        }

        ecs_meta_cursor_t c;
        if (ut_is_unsigned) {
            c = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value_unsigned);
        } else {
            c = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
        }

        if (ecs_meta_set_value(&c, &v)) {
            char *path = ecs_get_path(world, e);
            ecs_err("failed to get constant value for '%s'", path);
            ecs_os_free(path);
            return -1;
        }

        value_set = true;
    }

    /* Make sure constant value doesn't conflict if set / find the next value */
    it = ecs_map_iter(ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        if (ut_is_unsigned) {
            if (value_set) {
                if (c->value_unsigned == value_unsigned) {
                    char *path = ecs_get_path(world, e);
                    ecs_abort(ECS_INTERNAL_ERROR, 
                        "conflicting constant value %u for '%s' (other is '%s')",
                        value_unsigned, path, c->name);
                    ecs_os_free(path);
                    
                    return -1;
                }
            } else {
                if (c->value_unsigned >= value_unsigned) {
                    value_unsigned = c->value_unsigned + 1;
                }
            }
        } else {
            if (value_set) {
                if (c->value == value) {
                    char *path = ecs_get_path(world, e);
                    ecs_err("conflicting constant value %d for '%s' (other is '%s')",
                        value, path, c->name);
                    ecs_os_free(path);
                    return -1;
                }
            } else {
                if (c->value >= value) {
                    value = c->value + 1;
                }
            }
        }
    }

    ecs_map_init_if(ptr->constants, &world->allocator);
    ecs_enum_constant_t *c;
    if (ut_is_unsigned) {
        c = ecs_map_insert_alloc_t(ptr->constants, 
            ecs_enum_constant_t, value_unsigned);
        c->value_unsigned = value_unsigned;
        c->value = 0;
    } else {
        c = ecs_map_insert_alloc_t(ptr->constants, 
            ecs_enum_constant_t, (ecs_map_key_t)value);
        c->value_unsigned = 0;
        c->value = value;

    }
    c->name = ecs_os_strdup(ecs_get_name(world, e));
    c->constant = e;

    ecs_vec_init_if_t(&ptr->ordered_constants, ecs_enum_constant_t);
    ecs_enum_constant_t* ordered_c = ecs_vec_append_t(NULL,
        &ptr->ordered_constants, ecs_enum_constant_t);
    ordered_c->name = c->name;
    ordered_c->value = value;
    ordered_c->value_unsigned = value_unsigned;
    ordered_c->constant = c->constant;

    if (!value_set) {
        const ecs_type_info_t *ti = ecs_get_type_info(world, ut);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, 
            "underlying type is not a type");
        void *cptr = ecs_ensure_id(
            world, e, ecs_pair(EcsConstant, ut), flecs_ito(size_t, ti->size));
        ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ut, cptr);
        if (ut_is_unsigned) {
            if (ecs_meta_set_uint(&cur, value_unsigned)) {
                char *path = ecs_get_path(world, e);
                ecs_err("failed to assign value to constant '%s'", path);
                ecs_os_free(path);
                return -1;
            }
        } else {
            if (ecs_meta_set_int(&cur, value)) {
                char *path = ecs_get_path(world, e);
                ecs_err("failed to assign value to constant '%s'", path);
                ecs_os_free(path);
                return -1;
            }
        }
    }

    ecs_modified(world, type, EcsConstants);
    ecs_modified(world, type, EcsEnum);
    return 0;
}

static
int flecs_add_constant_to_bitmask(
    ecs_world_t *world, 
    ecs_entity_t type, 
    ecs_entity_t e,
    ecs_id_t constant_id)
{
    ecs_add(world, type, EcsBitmask);
    EcsConstants *ptr = ecs_ensure(world, type, EcsConstants);
    
    /* Remove constant from map and vector if it was already added */
    ecs_map_iter_t it = ecs_map_iter(ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        if (c->constant == e) {
            ecs_os_free(ECS_CONST_CAST(char*, c->name));
            ecs_map_remove_free(ptr->constants, ecs_map_key(&it));

            ecs_bitmask_constant_t* constants = ecs_vec_first_t(
                &ptr->ordered_constants, ecs_bitmask_constant_t);
            int32_t i, count = ecs_vec_count(&ptr->ordered_constants);
            for (i = 0; i < count; i++) {
                if (constants[i].constant == c->value) {
                    break;
                }
            }
            if (i < count) {
                for (int j = i; j < count - 1; j++) {
                    constants[j] = constants[j + 1];
                }
                ecs_vec_remove_last(&ptr->ordered_constants);
            }
        }
    }

    /* Check if constant sets explicit value */
    uint32_t value = 1;
    if (ecs_id_is_pair(constant_id)) {
        if (ecs_pair_second(world, constant_id) != ecs_id(ecs_u32_t)) {
            char *path = ecs_get_path(world, e);
            ecs_err("expected u32 type for bitmask constant '%s'", path);
            ecs_os_free(path);
            return -1;
        }

        const uint32_t *value_ptr = ecs_get_pair_second(
            world, e, EcsConstant, ecs_u32_t);
        ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        value = *value_ptr;
    } else {
        value = 1u << (ecs_u32_t)ecs_map_count(ptr->constants);
    }

    if (!ptr->constants) {
        ptr->constants = ecs_os_malloc_t(ecs_map_t);
        ecs_map_init(ptr->constants, NULL);
    }

    /* Make sure constant value doesn't conflict */
    it = ecs_map_iter(ptr->constants);
    while  (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        if (c->value == value) {
            char *path = ecs_get_path(world, e);
            ecs_err("conflicting constant value for '%s' (other is '%s')",
                path, c->name);
            ecs_os_free(path);
            return -1;
        }
    }

    ecs_map_init_if(ptr->constants, &world->allocator);

    ecs_bitmask_constant_t *c = ecs_map_insert_alloc_t(ptr->constants, 
        ecs_bitmask_constant_t, value);
    c->name = ecs_os_strdup(ecs_get_name(world, e));
    c->value = value;
    c->constant = e;

    ecs_vec_init_if_t(&ptr->ordered_constants, ecs_bitmask_constant_t);
    ecs_bitmask_constant_t* ordered_c = ecs_vec_append_t(NULL,
        &ptr->ordered_constants, ecs_bitmask_constant_t);
    ordered_c->name = c->name;
    ordered_c->value = value;
    ordered_c->constant = c->constant;

    ecs_u32_t *cptr = ecs_ensure_pair_second(
        world, e, EcsConstant, ecs_u32_t);
    ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
    cptr[0] = value;

    cptr = ecs_ensure_id(world, e, type, sizeof(uint32_t));
    cptr[0] = value;

    return 0;
}

static
void flecs_add_enum(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsEnum *data = ecs_field(it, EcsEnum, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t underlying_type = data[i].underlying_type;

        if (!underlying_type) {
            underlying_type = data[i].underlying_type = ecs_id(ecs_i32_t);
        }

        const EcsComponent *uc = ecs_get(world, underlying_type, EcsComponent);
        if (!uc) {
            char *str = ecs_get_path(world, underlying_type);
            ecs_err("uderlying_type entity for enum '%s' is not a type", str);
            ecs_os_free(str);
            continue;
        }

        if (flecs_init_type(world, e, EcsEnumType, uc->size, uc->alignment)) {
            continue;
        }

        ecs_add_id(world, e, EcsExclusive);
        ecs_add_id(world, e, EcsOneOf);
        ecs_add_id(world, e, EcsPairIsTag);
    }
}

static
void flecs_add_bitmask(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (flecs_init_type_t(world, e, EcsBitmaskType, ecs_u32_t)) {
            continue;
        }
    }
}

static
void flecs_add_constant(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (!parent) {
            ecs_err("missing parent for constant '%s'", ecs_get_name(world, e));
            continue;
        }

        if (ecs_has(world, parent, EcsEnum)) {
            flecs_add_constant_to_enum(world, parent, e, it->event_id);
        } else if (ecs_has(world, parent, EcsBitmask)) {
            flecs_add_constant_to_bitmask(world, parent, e, it->event_id);
        }
    }
}

ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_entity_t underlying = desc->underlying_type;

    if (!underlying) {
        underlying = ecs_id(ecs_i32_t);
    }

    ecs_assert(ecs_is_valid(world, underlying), ECS_INVALID_PARAMETER, 
        "invalid underlying type for enum");

    const EcsPrimitive *p = ecs_get(world, underlying, EcsPrimitive);
    if (!p) {
        char *path = ecs_get_path(world, underlying);
        ecs_err("underlying type '%s' must be a primitive type", path);
        ecs_os_free(path);
        return 0;
    }

    bool ut_is_unsigned = false;
    ecs_primitive_kind_t kind = p->kind;
    if (kind == EcsU8 || kind == EcsU16 || kind == EcsU32 || kind == EcsU64) {
        ut_is_unsigned = true;
    }

    EcsEnum *enum_data = ecs_ensure(world, t, EcsEnum);
    enum_data->underlying_type = underlying;
    ecs_modified(world, t, EcsEnum);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_enum_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity(world, {
            .name = m_desc->name
        });

        if (!m_desc->value && !m_desc->value_unsigned) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            const ecs_type_info_t *ti = ecs_get_type_info(world, underlying);
            ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, 
                "underlying type is not a type");
            void *ptr = ecs_ensure_id(world, c, 
                ecs_pair(EcsConstant, underlying), flecs_ito(size_t, ti->size));
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_meta_cursor_t cur = ecs_meta_cursor(world, underlying, ptr);

            int ret;
            if (m_desc->value) {
                if (ut_is_unsigned) {
                    char *path = ecs_get_path(world, c);
                    ecs_err("use desc::value_unsigned for constant '%s' which"
                        "has an unsigned underlying type", path);
                    ecs_os_free(path);
                    return 0;
                }
                ret = ecs_meta_set_int(&cur, m_desc->value);
            } else {
                if (!ut_is_unsigned) {
                    char *path = ecs_get_path(world, c);
                    ecs_err("use desc::value for constant '%s' which"
                        "has a signed underlying type", path);
                    ecs_os_free(path);
                    return 0;
                }
                ret = ecs_meta_set_uint(&cur, m_desc->value_unsigned);
            }

            if (ret) {
                char *type_str = ecs_get_path(world, t);
                char *utype_str = ecs_get_path(world, underlying);
                ecs_err("value for constant '%s' for enum '%s' is not valid "
                    "for underlying type '%s'", type_str, utype_str);
                ecs_os_free(utype_str);
                ecs_os_free(type_str);
                continue;
            }

            ecs_modified_id(world, c, ecs_pair(EcsConstant, underlying));
        }
    }

    ecs_set_scope(world, old_scope);
    flecs_resume_readonly(world, &rs);

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
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_add(world, t, EcsBitmask);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_bitmask_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity(world, {
            .name = m_desc->name
        });

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_second(world, c, EcsConstant, ecs_u32_t, 
                { flecs_uto(uint32_t, m_desc->value) });
        }
    }

    ecs_set_scope(world, old_scope);
    flecs_resume_readonly(world, &rs);

    if (i == 0) {
        ecs_err("bitmask '%s' has no constants", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

void flecs_meta_enum_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsEnum),
            .name = "enum", .symbol = "EcsEnum",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsEnum),
        .type.alignment = ECS_ALIGNOF(EcsEnum)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsBitmask),
            .name = "bitmask", .symbol = "EcsBitmask",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsBitmask),
        .type.alignment = ECS_ALIGNOF(EcsBitmask)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsConstants),
            .name = "constants", .symbol = "EcsConstants",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsConstants),
        .type.alignment = ECS_ALIGNOF(EcsConstants)
    });

    ecs_set_hooks(world, EcsEnum, { 
        .ctor = flecs_default_ctor,
    });

    ecs_set_hooks(world, EcsBitmask, { 
        .ctor = flecs_default_ctor,
    });

    ecs_set_hooks(world, EcsConstants, { 
        .ctor = ecs_ctor(EcsConstants),
        .move = ecs_move(EcsConstants),
        .dtor = ecs_dtor(EcsConstants),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });
    
    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsEnum) },
        .events = {EcsOnSet},
        .callback = flecs_add_enum
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsBitmask) },
        .events = {EcsOnAdd},
        .callback = flecs_add_bitmask
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = EcsConstant },
        .events = {EcsOnAdd},
        .callback = flecs_add_constant
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_pair(EcsConstant, EcsWildcard) },
        .events = {EcsOnSet},
        .callback = flecs_add_constant
    });

    ecs_add_pair(world, ecs_id(EcsEnum), EcsWith, ecs_id(EcsConstants));
    ecs_add_pair(world, ecs_id(EcsBitmask), EcsWith, ecs_id(EcsConstants));

    ecs_set(world, ecs_id(EcsEnum),    EcsDefaultChildComponent, {EcsConstant});
    ecs_set(world, ecs_id(EcsBitmask), EcsDefaultChildComponent, {EcsConstant});
}

#endif
