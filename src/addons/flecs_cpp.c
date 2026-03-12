/**
 * @file addons/flecs_cpp.c
 * @brief Utilities for C++ addon.
 */

#include "../private_api.h"

/* C++ type name normalization utilities */

#ifdef FLECS_CPP

/* Prefix strings stripped from compiler-generated type names */

#define ECS_CONST_PREFIX "const "
#define ECS_STRUCT_PREFIX "struct "
#define ECS_CLASS_PREFIX "class "
#define ECS_ENUM_PREFIX "enum "

#define ECS_CONST_LEN (-1 + (ecs_size_t)sizeof(ECS_CONST_PREFIX))
#define ECS_STRUCT_LEN (-1 + (ecs_size_t)sizeof(ECS_STRUCT_PREFIX))
#define ECS_CLASS_LEN (-1 + (ecs_size_t)sizeof(ECS_CLASS_PREFIX))
#define ECS_ENUM_LEN (-1 + (ecs_size_t)sizeof(ECS_ENUM_PREFIX))

/* Strip a prefix string from the beginning of a type name. */
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

/* Remove compiler-specific qualifiers and decorations from a type name. */
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
    ecs_assert(type_name != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!len) {
        len = flecs_itosize(ecs_os_strlen(type_name));
    }

    if (!symbol_name) {
        symbol_name = ecs_os_malloc_n(char, flecs_uto(int32_t, len + 1));
        ecs_assert(symbol_name != NULL, ECS_OUT_OF_MEMORY, NULL);
    }

    const char *ptr;
    size_t i;
    for (i = 0, ptr = type_name; i < len && *ptr; i ++, ptr ++) {
        if (*ptr == ':' && ptr[1] == ':') {
            symbol_name[i] = '.';
            ptr ++;
        } else {
            symbol_name[i] = *ptr;
        }
    }

    symbol_name[i] = '\0';

    return symbol_name;
}

/* Find the last occurrence of a character within the relevant portion of a function name. */
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

/* Return the pointer that points further into the string. */
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
    ecs_assert(start != NULL, ECS_INVALID_PARAMETER, "%s", func_name);
    start ++;
    
    ecs_size_t len = flecs_uto(ecs_size_t, 
        (f_len - (start - func_name) - fb_len));
    ecs_os_memcpy_n(constant_name, start, char, len);
    constant_name[len] = '\0';
    return constant_name;
}

/* Strip the current module namespace prefix from a type name.
 * If the type's namespace overlaps with the current module, strip the
 * module prefix so out-of-module types register under the module scope. */
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
            /* Type is a child of current parent, trim name of parent */
            type_name += len;
            ecs_assert(type_name[0], ECS_INVALID_PARAMETER, 
                "invalid C++ type name");
            ecs_assert(type_name[0] == ':', ECS_INVALID_PARAMETER,
                "invalid C++ type name");
            ecs_assert(type_name[1] == ':', ECS_INVALID_PARAMETER,
                "invalid C++ type name");
            type_name += 2;
        } else {
            /* Type is not a child of current parent, trim entire path */
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
    const ecs_cpp_component_desc_t *desc)
{
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    const char *name = desc->name;
    const char *cpp_symbol = desc->cpp_symbol;

    bool existing = false;
    ecs_entity_t c = flecs_component_ids_get(world, desc->ids_index);

    if (!c || !ecs_is_alive(world, c)) {
    } else {
        return c;
    }

    ecs_assert(desc->cpp_name != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!desc->cpp_symbol) {
        ecs_size_t len = ecs_os_strlen(desc->cpp_name);
        char *symbol_name = ecs_os_alloca_n(char, len + 1);
        cpp_symbol = ecs_cpp_get_symbol_name(
            symbol_name, desc->cpp_name, flecs_itosize(len));
    }

    const char *user_name = NULL;
    bool implicit_name = true;
    ecs_entity_t module = 0;

    if (desc->explicit_registration) {
        user_name = desc->name;
        implicit_name = false;

        if (!user_name) {
            user_name = desc->cpp_name;
        
            /* No explicit name: reuse the registered name if already
             * registered (it may include the flecs scope). */
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

    /* Type may already be registered in the world from another binary.
     * Look up by symbol (separate from typename) from root scope. */
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    if (desc->id) {
        c = desc->id;
    } else {
        c = ecs_lookup_path_w_sep(world, 0, user_name, "::", "::", false);
        existing = c != 0 && ecs_has(world, c, EcsComponent);
    }
    ecs_set_scope(world, prev_scope);

    /* If entity exists, compare symbol name to ensure that the component
     * we are trying to register under this name is the same */
    if (c) {
        const EcsComponent *component = ecs_get(world, c, EcsComponent);
        if (component != NULL) {
            const char *sym = ecs_get_symbol(world, c);
            if (sym && ecs_os_strcmp(sym, cpp_symbol)) {
                /* Symbol mismatch: allow only if the C++ type is a
                 * same-layout wrapper of the C type (name must match
                 * and size/alignment must be identical). */
                char *type_path = ecs_get_path(world, c);
                if (ecs_os_strcmp(type_path, cpp_symbol)) {
                    ecs_err(
                        "component with name '%s' is already registered for"\
                        " type '%s' (trying to register for type '%s')",
                            name, sym, cpp_symbol);
                    ecs_abort(ECS_NAME_IN_USE, NULL);
                }

                if (flecs_itosize(component->size) != desc->size || 
                    flecs_itosize(component->alignment) != desc->alignment)
                {
                    ecs_err(
                        "component with name '%s' is already registered with"\
                        " mismatching size/alignment", name);
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
        ecs_assert(c == 0 || (c == desc->id), 
            ECS_INCONSISTENT_COMPONENT_ID, "%s", cpp_symbol);
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
                existing = true;
            } else {
                /* If type is not yet known, derive from type name */
                name = ecs_cpp_trim_module(world, desc->cpp_name);
            }
        }
    } else {
        /* If an explicit id is provided but it has no name, inherit
         * the name from the type. */
        if (!ecs_is_valid(world, c) || !ecs_get_name(world, c)) {
            name = ecs_cpp_trim_module(world, desc->cpp_name);
        }
    }

    if (desc->is_component || desc->size != 0) {
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
            .type.size = flecs_uto(int32_t, desc->size),
            .type.alignment = flecs_uto(int32_t, desc->alignment)
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
    flecs_component_ids_set(world, desc->ids_index, c);

    if (desc->lifecycle_action && desc->size && !existing) {
        desc->lifecycle_action(world, c);
    }

    if (desc->enum_action) {
       desc->enum_action(world, c);
    }

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
    ecs_assert(id != 0, ECS_INVALID_OPERATION, "%s", name);
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
ecs_member_t* ecs_cpp_last_member(
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
        result.stage = NULL;
        return result;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_ensure(world, entity, id, r, 
        flecs_uto(int32_t, size));
    
    result.ptr = dst.ptr;
    result.world = world;
    result.stage = stage;

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
        result.stage = NULL;
        return result;
    }

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t dst = flecs_get_mut(world, entity, id, r, 
        flecs_uto(int32_t, size));

    ecs_assert(dst.ptr != NULL, ECS_INVALID_OPERATION, 
        "entity does not have component, use set() instead");
        
    result.ptr = dst.ptr;
    result.world = world;
    result.stage = stage;

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
    return result;
error:
    return (ecs_cpp_get_mut_t){0};
}

ecs_entity_t ecs_cpp_new(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name,
    const char *sep,
    const char *root_sep)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (!parent && !name) {
        if (!stage->scope && !stage->with) {
            ecs_entity_t result = flecs_new_id(world);
            flecs_add_to_root_table(world, result);
            return result;
        }
    }

    ecs_entity_desc_t desc = {0};
    desc.parent = parent;
    desc.name = name;
    desc.sep = sep;
    desc.root_sep = root_sep;
    return ecs_entity_init(world, &desc);
}

#endif
