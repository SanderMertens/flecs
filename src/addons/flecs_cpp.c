/**
 * @file addons/flecs_cpp.c
 * @brief Utilities for C++ addon.
 */

#include "../private_api.h"

/* Utilities for C++ API */

#ifdef FLECS_CPP

/* Convert compiler-specific typenames extracted from __PRETTY_FUNCTION__ to
 * a uniform identifier */

#define ECS_CONST_PREFIX "const "
#define ECS_STRUCT_PREFIX "struct "
#define ECS_CLASS_PREFIX "class "
#define ECS_ENUM_PREFIX "enum "

#define ECS_CONST_LEN (-1 + (ecs_size_t)sizeof(ECS_CONST_PREFIX))
#define ECS_STRUCT_LEN (-1 + (ecs_size_t)sizeof(ECS_STRUCT_PREFIX))
#define ECS_CLASS_LEN (-1 + (ecs_size_t)sizeof(ECS_CLASS_PREFIX))
#define ECS_ENUM_LEN (-1 + (ecs_size_t)sizeof(ECS_ENUM_PREFIX))

static
ecs_size_t ecs_cpp_strip_prefix(
    char *typeName,
    ecs_size_t len,
    const char *prefix,
    ecs_size_t prefix_len)
{
    if ((len > prefix_len) && !ecs_os_strncmp(typeName, prefix, prefix_len)) {
        ecs_os_memmove(typeName, typeName + prefix_len, len - prefix_len);
        typeName[len - prefix_len] = '\0';
        len -= prefix_len;
    }
    return len;
}

static 
void ecs_cpp_trim_type_name(
    char *typeName) 
{
    ecs_size_t len = ecs_os_strlen(typeName);

    len = ecs_cpp_strip_prefix(typeName, len, ECS_CONST_PREFIX, ECS_CONST_LEN);
    len = ecs_cpp_strip_prefix(typeName, len, ECS_STRUCT_PREFIX, ECS_STRUCT_LEN);
    len = ecs_cpp_strip_prefix(typeName, len, ECS_CLASS_PREFIX, ECS_CLASS_LEN);
    len = ecs_cpp_strip_prefix(typeName, len, ECS_ENUM_PREFIX, ECS_ENUM_LEN);

    while (typeName[len - 1] == ' ' ||
            typeName[len - 1] == '&' ||
            typeName[len - 1] == '*') 
    {
        len --;
        typeName[len] = '\0';
    }

    /* Remove const at end of string */
    if (len > ECS_CONST_LEN) {
        if (!ecs_os_strncmp(&typeName[len - ECS_CONST_LEN], " const", ECS_CONST_LEN)) {
            typeName[len - ECS_CONST_LEN] = '\0';
        }
        len -= ECS_CONST_LEN;
    }

    /* Check if there are any remaining "struct " strings, which can happen
     * if this is a template type on msvc. */
    if (len > ECS_STRUCT_LEN) {
        char *ptr = typeName;
        while ((ptr = strstr(ptr + 1, ECS_STRUCT_PREFIX)) != 0) {
            /* Make sure we're not matched with part of a longer identifier
             * that contains 'struct' */
            if (ptr[-1] == '<' || ptr[-1] == ',' || isspace(ptr[-1])) {
                ecs_os_memmove(ptr, ptr + ECS_STRUCT_LEN, 
                    ecs_os_strlen(ptr + ECS_STRUCT_LEN) + 1);
                len -= ECS_STRUCT_LEN;
            }
        }
    }
}

char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len,
    size_t front_len)
{
    memcpy(type_name, func_name + front_len, len);
    type_name[len] = '\0';
    ecs_cpp_trim_type_name(type_name);
    return type_name;
}

char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len)
{
    const char *ptr;
    size_t i;
    for (i = 0, ptr = type_name; i < len && *ptr; i ++, ptr ++) {
        if (*ptr == ':') {
            symbol_name[i] = '.';
            ptr ++;
        } else {
            symbol_name[i] = *ptr;
        }
    }

    symbol_name[i] = '\0';

    return symbol_name;
}

static
const char* flecs_cpp_func_rchr(
    const char *func_name,
    ecs_size_t func_name_len,
    ecs_size_t func_back_len,
    char ch)
{
    const char *r = strrchr(func_name, ch);
    if ((r - func_name) >= (func_name_len - flecs_uto(ecs_size_t, func_back_len))) {
        return NULL;
    }
    return r;
}

static
const char* flecs_cpp_func_max(
    const char *a,
    const char *b)
{
    if (a > b) return a;
    return b;
}

char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t func_name_len,
    size_t func_back_len)
{
    ecs_size_t f_len = flecs_uto(ecs_size_t, func_name_len);
    ecs_size_t fb_len = flecs_uto(ecs_size_t, func_back_len);
    const char *start = flecs_cpp_func_rchr(func_name, f_len, fb_len, ' ');
    start = flecs_cpp_func_max(start, flecs_cpp_func_rchr(
        func_name, f_len, fb_len, ')'));
    start = flecs_cpp_func_max(start, flecs_cpp_func_rchr(
        func_name, f_len, fb_len, ':'));
    start = flecs_cpp_func_max(start, flecs_cpp_func_rchr(
        func_name, f_len, fb_len, ','));
    ecs_assert(start != NULL, ECS_INVALID_PARAMETER, func_name);
    start ++;
    
    ecs_size_t len = flecs_uto(ecs_size_t, 
        (f_len - (start - func_name) - fb_len));
    ecs_os_memcpy_n(constant_name, start, char, len);
    constant_name[len] = '\0';
    return constant_name;
}

// Names returned from the name_helper class do not start with ::
// but are relative to the root. If the namespace of the type
// overlaps with the namespace of the current module, strip it from
// the implicit identifier.
// This allows for registration of component types that are not in the 
// module namespace to still be registered under the module scope.
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name)
{
    ecs_entity_t scope = ecs_get_scope(world);
    if (!scope) {
        return type_name;
    }

    char *path = ecs_get_path_w_sep(world, 0, scope, "::", NULL);
    if (path) {
        ecs_size_t len = ecs_os_strlen(path);
        if (!ecs_os_strncmp(path, type_name, len) && type_name[len] == ':') {
            // Type is a child of current parent, trim name of parent
            type_name += len;
            ecs_assert(type_name[0], ECS_INVALID_PARAMETER, 
                "invalid C++ type name");
            ecs_assert(type_name[0] == ':', ECS_INVALID_PARAMETER,
                "invalid C++ type name");
            ecs_assert(type_name[1] == ':', ECS_INVALID_PARAMETER,
                "invalid C++ type name");
            type_name += 2;
        } else {
            // Type is not a child of current parent, trim entire path
            char *ptr = strrchr(type_name, ':');
            if (ptr) {
                type_name = ptr + 1;
            }

        }
    }

    ecs_os_free(path);

    return type_name;
}

ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    ecs_entity_t id,
    int32_t ids_index,
    const char *name,
    const char *cpp_name,
    const char *cpp_symbol,
    size_t size,
    size_t alignment,
    bool is_component,
    bool explicit_registration,
    bool *registered_out,
    bool *existing_out)
{
    ecs_assert(registered_out != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(existing_out != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t c = flecs_component_ids_get(world, ids_index);

    if (!c || !ecs_is_alive(world, c)) {
    } else {
        return c;
    }

    const char *user_name = NULL;
    bool implicit_name = true;
    ecs_entity_t module = 0;

    if (explicit_registration) {
        user_name = name;
        implicit_name = false;

        if (!user_name) {
            user_name = cpp_name;
        
            /* Keep track of whether name was explicitly set. If not, and 
             * the component was already registered, just use the registered 
             * name. The registered name may differ from the typename as the 
             * registered name includes the flecs scope. This can in theory 
             * be different from the C++ namespace though it is good 
             * practice to keep them the same */
            implicit_name = true;
        }

        /* If component is registered by module, ensure it's registered in
         * the scope of the module. */
        module = ecs_get_scope(world);
        
        /* Strip off the namespace part of the component name, unless a name 
         * was explicitly provided by the application. */
        if (module && implicit_name) {
            /* If the type is a template type, make sure to ignore ::
             * inside the template parameter list. */
            const char *start = strchr(user_name, '<'), *last_elem = NULL;
            if (start) {
                const char *ptr = start;
                while (ptr[0] && (ptr[0] != ':') && (ptr > user_name)) {
                    ptr --;
                }
                if (ptr[0] == ':') {
                    last_elem = ptr;
                }
            }
        
            if (last_elem) {
                name = last_elem + 1;
            }
        }
    }

    /* At this point it is possible that the type was already registered
     * with the world, just not for this binary. The registration code
     * uses the type symbol to check if it was already registered. Note
     * that the symbol is separate from the typename, as an application
     * can override a component name when registering a type. */

    /* If the component is not yet registered, ensure no other component
     * or entity has been registered with this name. Ensure component is 
     * looked up from root. */
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    if (id) {
        c = id;
    } else {
        c = ecs_lookup_path_w_sep(world, 0, user_name, "::", "::", false);
        *existing_out = c != 0 && ecs_has(world, c, EcsComponent);
    }
    ecs_set_scope(world, prev_scope);

    /* If entity exists, compare symbol name to ensure that the component
     * we are trying to register under this name is the same */
    if (c) {
        const EcsComponent *component = ecs_get(world, c, EcsComponent);
        if (component != NULL) {
            const char *sym = ecs_get_symbol(world, c);
            if (sym && ecs_os_strcmp(sym, cpp_symbol)) {
                /* Application is trying to register a type with an entity
                 * that was already associated with another type. In most 
                 * cases this is an error, with the exception of a scenario
                 * where the application is wrapping a C type with a C++ 
                 * type.
                 * 
                 * In this case the C++ type typically inherits from the C 
                 * type, and adds convenience methods to the derived class 
                 * without changing anything that would change the size or 
                 * layout.
                 * 
                 * To meet this condition, the new type must have the same 
                 * size and alignment as the existing type, and the name of 
                 * the type type must be equal to the registered name.
                 * 
                 * The latter ensures that it was the intent of the 
                 * application to alias the type, vs. accidentally 
                 * registering an unrelated type with the same 
                 * size/alignment. */
                char *type_path = ecs_get_path(world, c);
                if (ecs_os_strcmp(type_path, cpp_symbol)) {
                    ecs_err(
                        "component with name '%s' is already registered for"\
                        " type '%s' (trying to register for type '%s')",
                            name, sym, cpp_symbol);
                    ecs_abort(ECS_NAME_IN_USE, NULL);
                }

                if (flecs_itosize(component->size) != size || 
                    flecs_itosize(component->alignment) != alignment)
                {
                    ecs_err(
                        "component with name '%s' is already registered with"\
                        " mismatching size/alignment)", name);
                    ecs_abort(ECS_INVALID_COMPONENT_SIZE, NULL);
                }

                ecs_os_free(type_path);
            } else if (!sym) {
                ecs_set_symbol(world, c, cpp_symbol);
            }
        }

    /* If no entity is found, lookup symbol to check if the component was
     * registered under a different name. */
    } else if (!implicit_name) {
        c = ecs_lookup_symbol(world, cpp_symbol, false, false);
        ecs_assert(c == 0 || (c == id), 
            ECS_INCONSISTENT_COMPONENT_ID, cpp_symbol);
    }

    const char *symbol = NULL;

    if (c) {
        symbol = ecs_get_symbol(world, c);
    }

    if (!symbol) {
        symbol = cpp_symbol;
    }

    /* When a component is implicitly registered, ensure that it's not
     * registered in the current scope of the application/that "with"
     * components get added to the component entity. */
    prev_scope = ecs_set_scope(world, module);
    ecs_entity_t prev_with = ecs_set_with(world, 0);
    char *existing_name = NULL;

    /* If an explicit id is provided, it is possible that the symbol and
     * name differ from the actual type, as the application may alias
     * one type to another. */
    if (!c) {
        if (!name) {
            /* If no name was provided first check if a type with the 
             * provided symbol was already registered. */
            ecs_id_t e = ecs_lookup_symbol(world, symbol, false, false);
            if (e) {
                existing_name = ecs_get_path_w_sep(world, 0, e, "::", "::");
                name = existing_name;
                *existing_out = true;
            } else {
                /* If type is not yet known, derive from type name */
                name = ecs_cpp_trim_module(world, cpp_name);
            }
        }
    } else {
        /* If an explicit id is provided but it has no name, inherit
         * the name from the type. */
        if (!ecs_is_valid(world, c) || !ecs_get_name(world, c)) {
            name = ecs_cpp_trim_module(world, cpp_name);
        }
    }

    if (is_component || size != 0) {
        c = ecs_entity(world, {
            .id = c,
            .name = name,
            .sep = "::",
            .root_sep = "::",
            .symbol = symbol,
            .use_low_id = true
        });

        ecs_assert(c != 0, ECS_INVALID_OPERATION, 
            "registration failed for component %s", name);

        c = ecs_component_init(world, &(ecs_component_desc_t){
            .entity = c,
            .type.size = flecs_uto(int32_t, size),
            .type.alignment = flecs_uto(int32_t, alignment)
        });

        ecs_assert(c != 0, ECS_INVALID_OPERATION, 
            "registration failed for component %s", name);
    } else {
        c = ecs_entity(world, {
            .id = c,
            .name = name,
            .sep = "::",
            .root_sep = "::",
            .symbol = symbol,
            .use_low_id = true
        });
    }

    ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_os_free(existing_name);

    ecs_set_with(world, prev_with);
    ecs_set_scope(world, prev_scope);

    /* Set world local component id */
    flecs_component_ids_set(world, ids_index, c);

    *registered_out = true;

    return c;
}

void ecs_cpp_enum_init(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t underlying_type)
{
    (void)world;
    (void)id;
    (void)underlying_type;
#ifdef FLECS_META
    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);
    EcsEnum *ptr = ecs_ensure(world, id, EcsEnum);
    ptr->underlying_type = underlying_type;
    ecs_modified(world, id, EcsEnum);
    flecs_resume_readonly(world, &readonly_state);
#else
    /* Make sure that enums still behave the same even without meta */
    ecs_add_id(world, id, EcsExclusive);
    ecs_add_id(world, id, EcsOneOf);
#endif
}

ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    void *value,
    ecs_entity_t value_type,
    size_t value_size)
{
    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);

    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent != 0, ECS_INTERNAL_ERROR, NULL);

    const char *parent_name = ecs_get_name(world, parent);
    ecs_size_t parent_name_len = ecs_os_strlen(parent_name);
    if (!ecs_os_strncmp(name, parent_name, parent_name_len)) {
        name += parent_name_len;
        if (name[0] == '_') {
            name ++;
        }
    }

    ecs_entity_t prev = ecs_set_scope(world, parent);
    id = ecs_entity(world, {
        .id = id,
        .name = name
    });
    ecs_assert(id != 0, ECS_INVALID_OPERATION, name);
    ecs_set_scope(world, prev);

#ifdef FLECS_DEBUG
    const EcsComponent *cptr = ecs_get(world, parent, EcsComponent);
    ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, "enum is not a component");
#endif

    ecs_set_id(world, id, ecs_pair(EcsConstant, value_type), value_size, value);

    flecs_resume_readonly(world, &readonly_state);

#ifdef FLECS_META
    if (ecs_should_log(0)) {
        ecs_value_t v = { .type = value_type, .ptr = value };
        char *str = NULL;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, 
            ecs_id(ecs_string_t), &str);
        ecs_meta_set_value(&cur, &v);
        ecs_trace("#[green]constant#[reset] %s.%s created with value %s", 
            ecs_get_name(world, parent), name, str);
        ecs_os_free(str);
    }
#endif

    return id;
}

#ifdef FLECS_META
const ecs_member_t* ecs_cpp_last_member(
    const ecs_world_t *world, 
    ecs_entity_t type)
{
    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    if (!st) {
        char *type_str = ecs_get_path(world, type);
        ecs_err("entity '%s' is not a struct", type_str);
        ecs_os_free(type_str);
        return 0;
    }

    ecs_member_t *m = ecs_vec_get_t(&st->members, ecs_member_t, 
        ecs_vec_count(&st->members) - 1);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    return m;
}
#endif

ecs_cpp_get_mut_t ecs_cpp_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *new_ptr,
    size_t size)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_cpp_get_mut_t result;

    if (flecs_defer_cmd(stage)) {
        result.ptr = flecs_defer_cpp_set(world, stage, entity, id,
            flecs_utosize(size), new_ptr);
        /* Modified command is already inserted */
        result.call_modified = false;
        return result;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(world, entity, id, r, 
        flecs_uto(int32_t, size));
    
    result.ptr = dst.ptr;

    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            result.call_modified = false;
            goto done;
        }
    }

    /* Not deferring, so need to call modified after setting the component */
    result.call_modified = true;

    if (dst.ti->hooks.on_replace) {
        flecs_invoke_replace_hook(
            world, r->table, entity, id, dst.ptr, new_ptr, dst.ti);
    }

done:
    flecs_defer_end(world, stage);
    
    return result;
error:
    return (ecs_cpp_get_mut_t){0};
}

ecs_cpp_get_mut_t ecs_cpp_assign(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *new_ptr,
    size_t size)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_cpp_get_mut_t result;

    if (flecs_defer_cmd(stage)) {
        result.ptr = flecs_defer_cpp_assign(
            world, stage, entity, id, flecs_uto(int32_t, size), new_ptr);
        /* Modified command is already inserted */
        result.call_modified = false;
        return result;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_get_mut(world, entity, id, r, 
        flecs_uto(int32_t, size));

    ecs_assert(dst.ptr != NULL, ECS_INVALID_OPERATION, 
        "entity does not have component, use set() instead");
        
    result.ptr = dst.ptr;

    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            result.call_modified = false;
            goto done;
        }
    }

    /* Not deferring, so need to call modified after setting the component */
    result.call_modified = true;

    if (dst.ti->hooks.on_replace) {
        flecs_invoke_replace_hook(
            world, r->table, entity, id, dst.ptr, new_ptr, dst.ti);
    }

done:
    flecs_defer_end(world, stage);
    
    return result;
error:
    return (ecs_cpp_get_mut_t){0};
}

#endif
