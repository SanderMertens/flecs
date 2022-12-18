/**
 * @file addons/flecs_cpp.c
 * @brief Utilities for C++ addon.
 */

#include "../private_api.h"
#include <ctype.h>

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
    size_t len)
{
    memcpy(type_name, func_name + ECS_FUNC_NAME_FRONT(const char*, type_name), len);
    type_name[len] = '\0';
    ecs_cpp_trim_type_name(type_name);
    return type_name;
}

char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len)
{
    // Symbol is same as name, but with '::' replaced with '.'
    ecs_os_strcpy(symbol_name, type_name);

    char *ptr;
    size_t i;
    for (i = 0, ptr = symbol_name; i < len && *ptr; i ++, ptr ++) {
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
const char* cpp_func_rchr(
    const char *func_name,
    ecs_size_t func_name_len,
    char ch)
{
    const char *r = strrchr(func_name, ch);
    if ((r - func_name) >= (func_name_len - flecs_uto(ecs_size_t, ECS_FUNC_NAME_BACK))) {
        return NULL;
    }
    return r;
}

static
const char* cpp_func_max(
    const char *a,
    const char *b)
{
    if (a > b) return a;
    return b;
}

char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t func_name_len)
{
    ecs_size_t f_len = flecs_uto(ecs_size_t, func_name_len);
    const char *start = cpp_func_rchr(func_name, f_len, ' ');
    start = cpp_func_max(start, cpp_func_rchr(func_name, f_len, ')'));
    start = cpp_func_max(start, cpp_func_rchr(func_name, f_len, ':'));
    start = cpp_func_max(start, cpp_func_rchr(func_name, f_len, ','));
    ecs_assert(start != NULL, ECS_INVALID_PARAMETER, func_name);
    start ++;
    
    ecs_size_t len = flecs_uto(ecs_size_t, 
        (f_len - (start - func_name) - flecs_uto(ecs_size_t, ECS_FUNC_NAME_BACK)));
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
        const char *ptr = strrchr(type_name, ':');
        ecs_assert(ptr != type_name, ECS_INTERNAL_ERROR, NULL);
        if (ptr) {
            ptr --;
            ecs_assert(ptr[0] == ':', ECS_INTERNAL_ERROR, NULL);
            ecs_size_t name_path_len = (ecs_size_t)(ptr - type_name);
            if (name_path_len <= ecs_os_strlen(path)) {
                if (!ecs_os_strncmp(type_name, path, name_path_len)) {
                    type_name = &type_name[name_path_len + 2];
                }
            }
        }
    }
    ecs_os_free(path);

    return type_name;
}

// Validate registered component
void ecs_cpp_component_validate(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    const char *symbol,
    size_t size,
    size_t alignment,
    bool implicit_name)
{
    /* If entity has a name check if it matches */
    if (ecs_is_valid(world, id) && ecs_get_name(world, id) != NULL) {
        if (!implicit_name && id >= EcsFirstUserComponentId) {
#ifndef FLECS_NDEBUG
            char *path = ecs_get_path_w_sep(
                world, 0, id, "::", NULL);
            if (ecs_os_strcmp(path, name)) {
                ecs_abort(ECS_INCONSISTENT_NAME,
                    "component '%s' already registered with name '%s'",
                    name, path);
            }
            ecs_os_free(path);
#endif
        }

        if (symbol) {
            const char *existing_symbol = ecs_get_symbol(world, id);
            if (existing_symbol) {
                if (ecs_os_strcmp(symbol, existing_symbol)) {
                    ecs_abort(ECS_INCONSISTENT_NAME,
                        "component '%s' with symbol '%s' already registered with symbol '%s'",
                        name, symbol, existing_symbol);
                }
            }
        }
    } else {
        /* Ensure that the entity id valid */
        if (!ecs_is_alive(world, id)) {
            ecs_ensure(world, id);
        }

        /* Register name with entity, so that when the entity is created the
        * correct id will be resolved from the name. Only do this when the
        * entity is empty. */
        ecs_add_path_w_sep(world, id, 0, name, "::", "::");
    }

    /* If a component was already registered with this id but with a 
     * different size, the ecs_component_init function will fail. */

    /* We need to explicitly call ecs_component_init here again. Even though
     * the component was already registered, it may have been registered
     * with a different world. This ensures that the component is registered
     * with the same id for the current world. 
     * If the component was registered already, nothing will change. */
    ecs_entity_t ent = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = id,
        .type.size = flecs_uto(int32_t, size),
        .type.alignment = flecs_uto(int32_t, alignment)
    });
    (void)ent;
    ecs_assert(ent == id, ECS_INTERNAL_ERROR, NULL);
}

ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment,
    bool implicit_name,
    bool *existing_out)
{
    (void)size;
    (void)alignment;

    /* If the component is not yet registered, ensure no other component
     * or entity has been registered with this name. Ensure component is 
     * looked up from root. */
    bool existing = false;
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_entity_t ent;
    if (id) {
        ent = id;
    } else {
        ent = ecs_lookup_path_w_sep(world, 0, name, "::", "::", false);
        existing = ent != 0;
    }
    ecs_set_scope(world, prev_scope);

    /* If entity exists, compare symbol name to ensure that the component
     * we are trying to register under this name is the same */
    if (ent) {
        const EcsComponent *component = ecs_get(world, ent, EcsComponent);
        if (component != NULL) {
            const char *sym = ecs_get_symbol(world, ent);
            if (sym && ecs_os_strcmp(sym, symbol)) {
                /* Application is trying to register a type with an entity that
                 * was already associated with another type. In most cases this
                 * is an error, with the exception of a scenario where the
                 * application is wrapping a C type with a C++ type.
                 * 
                 * In this case the C++ type typically inherits from the C type,
                 * and adds convenience methods to the derived class without
                 * changing anything that would change the size or layout.
                 * 
                 * To meet this condition, the new type must have the same size
                 * and alignment as the existing type, and the name of the type
                 * type must be equal to the registered name (not symbol).
                 * 
                 * The latter ensures that it was the intent of the application
                 * to alias the type, vs. accidentally registering an unrelated
                 * type with the same size/alignment. */
                char *type_path = ecs_get_fullpath(world, ent);
                if (ecs_os_strcmp(type_path, symbol) || 
                    component->size != size || 
                    component->alignment != alignment) 
                {
                    ecs_err(
                        "component with name '%s' is already registered for"\
                        " type '%s' (trying to register for type '%s')",
                            name, sym, symbol);
                    ecs_abort(ECS_NAME_IN_USE, NULL);
                }
                ecs_os_free(type_path);
            } else if (!sym) {
                ecs_set_symbol(world, ent, symbol);
            }
        }

    /* If no entity is found, lookup symbol to check if the component was
     * registered under a different name. */
    } else if (!implicit_name) {
        ent = ecs_lookup_symbol(world, symbol, false);
        ecs_assert(ent == 0 || (ent == id), ECS_INCONSISTENT_COMPONENT_ID, symbol);
    }

    if (existing_out) {
        *existing_out = existing;
    }

    return ent;
}

ecs_entity_t ecs_cpp_component_register_explicit(
    ecs_world_t *world,
    ecs_entity_t s_id,
    ecs_entity_t id,
    const char *name,
    const char *type_name,
    const char *symbol,
    size_t size,
    size_t alignment,
    bool is_component,
    bool *existing_out)
{
    char *existing_name = NULL;
    if (existing_out) *existing_out = false;

    // If an explicit id is provided, it is possible that the symbol and
    // name differ from the actual type, as the application may alias
    // one type to another.
    if (!id) {
        if (!name) {
            // If no name was provided first check if a type with the provided
            // symbol was already registered.
            id = ecs_lookup_symbol(world, symbol, false);
            if (id) {
                existing_name = ecs_get_path_w_sep(world, 0, id, "::", "::");
                name = existing_name;
                if (existing_out) *existing_out = true;
            } else {
                // If type is not yet known, derive from type name
                name = ecs_cpp_trim_module(world, type_name);
            }            
        }
    } else {
        // If an explicit id is provided but it has no name, inherit
        // the name from the type.
        if (!ecs_is_valid(world, id) || !ecs_get_name(world, id)) {
            name = ecs_cpp_trim_module(world, type_name);
        }
    }

    ecs_entity_t entity;
    if (is_component || size != 0) {
        entity = ecs_entity(world, {
            .id = s_id,
            .name = name,
            .sep = "::",
            .root_sep = "::",
            .symbol = symbol,
            .use_low_id = true
        });
        ecs_assert(entity != 0, ECS_INVALID_OPERATION, NULL);

        entity = ecs_component_init(world, &(ecs_component_desc_t){
            .entity = entity,
            .type.size = flecs_uto(int32_t, size),
            .type.alignment = flecs_uto(int32_t, alignment)
        });
        ecs_assert(entity != 0, ECS_INVALID_OPERATION, NULL);
    } else {
        entity = ecs_entity(world, {
            .id = s_id,
            .name = name,
            .sep = "::",
            .root_sep = "::",
            .symbol = symbol,
            .use_low_id = true
        });
    }

    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!s_id || s_id == entity, ECS_INTERNAL_ERROR, NULL);
    ecs_os_free(existing_name);

    return entity;
}

void ecs_cpp_enum_init(
    ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);
    ecs_add_id(world, id, EcsExclusive);
    ecs_add_id(world, id, EcsOneOf);
    ecs_add_id(world, id, EcsTag);
    flecs_resume_readonly(world, &readonly_state);
}

ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    int value)
{
    ecs_suspend_readonly_state_t readonly_state;
    world = flecs_suspend_readonly(world, &readonly_state);

    const char *parent_name = ecs_get_name(world, parent);
    ecs_size_t parent_name_len = ecs_os_strlen(parent_name);
    if (!ecs_os_strncmp(name, parent_name, parent_name_len)) {
        name += parent_name_len;
        if (name[0] == '_') {
            name ++;
        }
    }

    ecs_entity_t prev = ecs_set_scope(world, parent);
    id = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = id,
        .name = name
    });
    ecs_assert(id != 0, ECS_INVALID_OPERATION, name);
    ecs_set_scope(world, prev);

    #ifdef FLECS_DEBUG
    const EcsComponent *cptr = ecs_get(world, parent, EcsComponent);
    ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, "enum is not a component");
    ecs_assert(cptr->size == ECS_SIZEOF(int32_t), ECS_UNSUPPORTED,
        "enum component must have 32bit size");
    #endif

    ecs_set_id(world, id, parent, sizeof(int), &value);

    flecs_resume_readonly(world, &readonly_state);

    ecs_trace("#[green]constant#[reset] %s.%s created with value %d", 
        ecs_get_name(world, parent), name, value);

    return id;
}

static int32_t flecs_reset_count = 0;

int32_t ecs_cpp_reset_count_get(void) {
    return flecs_reset_count;
}

int32_t ecs_cpp_reset_count_inc(void) {
    return ++flecs_reset_count;
}

#endif
