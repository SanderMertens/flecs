/**
 * @file addons/meta/api.c
 * @brief API for creating entities with reflection data.
 */

#include "meta.h"

#ifdef FLECS_META

static
bool flecs_type_is_number(
    ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    if (!p) {
        return false;
    }

    switch(p->kind) {
    case EcsChar:
    case EcsU8:
    case EcsU16:
    case EcsU32:
    case EcsU64:
    case EcsI8:
    case EcsI16:
    case EcsI32:
    case EcsI64:
    case EcsF32:
    case EcsF64:
        return true;

    case EcsBool:
    case EcsByte:
    case EcsUPtr:
    case EcsIPtr:
    case EcsString:
    case EcsEntity:
    case EcsId:
        return false;
    default:
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
    }
}

void flecs_json_ser_bool(bool value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_strbuf_appendlit(str, "true");
    } else {
        ecs_strbuf_appendlit(str, "false");
    }
}

void flecs_json_ser_char_expression(char value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        char chbuf[3];
        flecs_chresc(chbuf, value, '"');
        ecs_strbuf_appendch(str, '"');
        ecs_strbuf_appendstr(str, chbuf);
        ecs_strbuf_appendch(str, '"');
    } else {
        ecs_strbuf_appendch(str, '0');
    }
}
void flecs_json_ser_char(char value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        char chbuf[3];
        flecs_chresc(chbuf, value, '"');
        ecs_strbuf_appendstr(str, chbuf);
    } else {
        ecs_strbuf_appendch(str, '0');
    }
}

void flecs_json_ser_byte(const uint8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

void flecs_json_ser_u8(const uint8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

void flecs_json_ser_u16(const uint16_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

void flecs_json_ser_u32(const uint32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

void flecs_json_ser_u64(const uint64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_append(str, "%llu", value);
}

void flecs_json_ser_i8(const int8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

void flecs_json_ser_i16(const int16_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

void flecs_json_ser_i32(const int32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

void flecs_json_ser_i64(const int64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, value);
}

void flecs_json_ser_f32(const float value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, (double)value, 0);
}

void flecs_json_ser_f32_expression(const ecs_f32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, (ecs_f64_t)value, '"');
}

void flecs_json_ser_f64(const double value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, value, 0);
}

void flecs_json_ser_f64_expression(const ecs_f64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, value, '"');
}

void flecs_json_ser_iptr(const intptr_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

void flecs_json_ser_uptr(const uintptr_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_append(str, "%u", value);
}

void flecs_json_ser_string_expression(const char* value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
        if (length == ecs_os_strlen(value)) {
            ecs_strbuf_appendch(str, '"');
            ecs_strbuf_appendstrn(str, value, length);
            ecs_strbuf_appendch(str, '"');
        } else {
            char *out = ecs_os_malloc(length + 3);
            flecs_stresc(out + 1, length, '"', value);
            out[0] = '"';
            out[length + 1] = '"';
            out[length + 2] = '\0';
            ecs_strbuf_appendstr(str, out);
            ecs_os_free(out);
        }
    } else {
        ecs_strbuf_appendlit(str, "null");
    }
}

void flecs_json_ser_string(const char* value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_strbuf_appendstr(str, value);
    } else {
        ecs_strbuf_appendlit(str, "null");
    }
}

void flecs_json_ser_entity(const ecs_world_t* world, ecs_entity_t e, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (!e) {
        ecs_strbuf_appendlit(str, "#0");
    } else {
        ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
    }
}

void flecs_json_ser_id(const ecs_world_t* world, ecs_id_t id, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (!id) {
        ecs_strbuf_appendlit(str, "#0");
    } else {
        ecs_id_str_buf(world, id, str);
    }
}

/* Serialize a primitive value */
int flecs_expr_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    ecs_visitor_desc_t *visitor_desc,
    const void *base) 
{
    switch(kind) {
    case EcsBool:
        if (visitor_desc->visit_bool)
            visitor_desc->visit_bool(*(const bool*)base, visitor_desc->user_data);
        break;
    case EcsChar:
        if (visitor_desc->visit_char)
            visitor_desc->visit_char(*(const char*)base, visitor_desc->user_data);
        break;
    case EcsByte:
        if (visitor_desc->visit_byte)
            visitor_desc->visit_byte(*(const uint8_t*)base, visitor_desc->user_data);
        break;
    case EcsU8:
        if (visitor_desc->visit_u8)
            visitor_desc->visit_u8(*(const uint8_t*)base, visitor_desc->user_data);
        break;
    case EcsU16:
        if (visitor_desc->visit_u16)
            visitor_desc->visit_u16(*(const uint16_t*)base, visitor_desc->user_data);
        break;
    case EcsU32:
        if (visitor_desc->visit_u32)
            visitor_desc->visit_u32(*(const uint32_t*)base, visitor_desc->user_data);
        break;
    case EcsU64:
        if (visitor_desc->visit_u64)
            visitor_desc->visit_u64(*(const uint64_t*)base, visitor_desc->user_data);
        break;
    case EcsI8:
        if (visitor_desc->visit_i8)
            visitor_desc->visit_i8(*(const int8_t*)base, visitor_desc->user_data);
        break;
    case EcsI16:
        if (visitor_desc->visit_i16)
            visitor_desc->visit_i16(*(const int16_t*)base, visitor_desc->user_data);
        break;
    case EcsI32:
        if (visitor_desc->visit_i32)
            visitor_desc->visit_i32(*(const int32_t*)base, visitor_desc->user_data);
        break;
    case EcsI64:
        if (visitor_desc->visit_i64)
            visitor_desc->visit_i64(*(const int64_t*)base, visitor_desc->user_data);
        break;
    case EcsF32:
        if (visitor_desc->visit_f32)
            visitor_desc->visit_f32(*(const float*)base, visitor_desc->user_data);
        break;
    case EcsF64:
        if (visitor_desc->visit_f64)
            visitor_desc->visit_f64(*(const double*)base, visitor_desc->user_data);
        break;
    case EcsIPtr:
        if (visitor_desc->visit_iptr)
            visitor_desc->visit_iptr(*(const intptr_t*)base, visitor_desc->user_data);
        break;
    case EcsUPtr:
        if (visitor_desc->visit_uptr)
            visitor_desc->visit_uptr(*(const uintptr_t*)base, visitor_desc->user_data);
        break;
    case EcsString:
        if (visitor_desc->visit_string)
            visitor_desc->visit_string(*ECS_CONST_CAST(const char**, base), visitor_desc->user_data);
        break;
    case EcsEntity:
        if (visitor_desc->visit_entity)
            visitor_desc->visit_entity(world, *(const ecs_entity_t*)base, visitor_desc->user_data);
        break;
    case EcsId:
        if (visitor_desc->visit_id)
            visitor_desc->visit_id(world, *(const ecs_id_t*)base, visitor_desc->user_data);
        break;
    default:
        ecs_err("invalid primitive kind");
        return -1;
    }

    return 0;
}

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    flecs_resume_readonly(world, &rs);
    return t;
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

    ecs_add(world, t, EcsEnum);

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

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_second(world, c, EcsConstant, ecs_i32_t, 
                {m_desc->value});
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
                {m_desc->value});
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

ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsArray, {
        .type = desc->type,
        .count = desc->count
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsVector, {
        .type = desc->type
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

static
bool flecs_member_range_overlaps(
    const ecs_member_value_range_t *range,
    const ecs_member_value_range_t *with)
{
    if (ECS_EQ(with->min, with->max)) {
        return false;
    }

    if (ECS_EQ(range->min, range->max)) {
        return false;
    }

    if (range->min < with->min || 
        range->max > with->max) 
    {
        return true;
    }

    return false;
}

ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
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
            goto error;
        }

        ecs_entity_t m = ecs_entity(world, {
            .name = m_desc->name
        });

        ecs_set(world, m, EcsMember, {
            .type = m_desc->type, 
            .count = m_desc->count,
            .offset = m_desc->offset,
            .unit = m_desc->unit,
            .use_offset = m_desc->use_offset
        });

        EcsMemberRanges *ranges = NULL;
        const ecs_member_value_range_t *range = &m_desc->range;
        const ecs_member_value_range_t *error = &m_desc->error_range;
        const ecs_member_value_range_t *warning = &m_desc->warning_range;
        if (ECS_NEQ(range->min, range->max)) {
            ranges = ecs_ensure(world, m, EcsMemberRanges);
            if (range->min > range->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid value range [%f..%f]",
                    member_name, range->min, range->max);
                ecs_os_free(member_name);
                goto error;
            }
            ranges->value.min = range->min;
            ranges->value.max = range->max;
        }
        if (ECS_NEQ(error->min, error->max)) {
            if (error->min > error->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid error range [%f..%f]",
                    member_name, error->min, error->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(error, range)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("error range of member '%s' overlaps with value range",
                    member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (!ranges) {
                ranges = ecs_ensure(world, m, EcsMemberRanges);
            }
            ranges->error.min = error->min;
            ranges->error.max = error->max;
        }

        if (ECS_NEQ(warning->min, warning->max)) {
            if (warning->min > warning->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid warning range [%f..%f]",
                    member_name, warning->min, warning->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, range)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("warning range of member '%s' overlaps with value "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, error)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("warning range of member '%s' overlaps with error "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }

            if (!ranges) {
                ranges = ecs_ensure(world, m, EcsMemberRanges);
            }
            ranges->warning.min = warning->min;
            ranges->warning.max = warning->max;
        }

        if (ranges && !flecs_type_is_number(world, m_desc->type)) {
            char *member_name = ecs_get_path(world, m);
            ecs_err("member '%s' has an value/error/warning range, but is not a "
                    "number", member_name);
            ecs_os_free(member_name);
            goto error;
        }

        if (ranges) {
            ecs_modified(world, m, EcsMemberRanges);
        }
    }

    ecs_set_scope(world, old_scope);
    flecs_resume_readonly(world, &rs);

    if (i == 0) {
        ecs_err("struct '%s' has no members", ecs_get_name(world, t));
        goto error;
    }

    if (!ecs_has(world, t, EcsStruct)) {
        goto error;
    }

    return t;
error:
    flecs_resume_readonly(world, &rs);
    if (t) {
        ecs_delete(world, t);
    }
    return 0;
}

ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc)
{
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->type.as_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set_ptr(world, t, EcsOpaque, &desc->type);

    flecs_resume_readonly(world, &rs);

    return t;
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

#endif
