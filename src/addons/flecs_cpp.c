#include "../private_api.h"
#include <ctype.h>

/* Utilities for C++ API */

#ifdef FLECS_CPP

/* Convert compiler-specific typenames extracted from __PRETTY_FUNCTION__ to
 * a uniform identifier */

static 
void ecs_cpp_trim_type_name(
    char *typeName) 
{
    ecs_size_t len = ecs_os_strlen(typeName);
    
    /* Remove 'const' */
    ecs_size_t const_len = ecs_os_strlen("const ");
    if ((len > const_len) && !ecs_os_strncmp(typeName, "const ", const_len)) {
        ecs_os_memmove(typeName, typeName + const_len, len - const_len);
        typeName[len - const_len] = '\0';
        len -= const_len;
    }

    /* Remove 'struct' */
    ecs_size_t struct_len = ecs_os_strlen("struct ");
    if ((len > struct_len) && !ecs_os_strncmp(typeName, "struct ", struct_len)) {
        ecs_os_memmove(typeName, typeName + struct_len, len - struct_len);
        typeName[len - struct_len] = '\0';
        len -= struct_len;
    }

    /* Remove 'class' */
    ecs_size_t class_len = ecs_os_strlen("class ");
    if ((len > class_len) && !ecs_os_strncmp(typeName, "class ", class_len)) {
        ecs_os_memmove(typeName, typeName + class_len, len - class_len);
        typeName[len - class_len] = '\0';
        len -= class_len;
    }            

    while (typeName[len - 1] == ' ' ||
            typeName[len - 1] == '&' ||
            typeName[len - 1] == '*') 
    {
        len --;
        typeName[len] = '\0';
    }

    /* Remove const at end of string */
    if (len > const_len) {
        if (!ecs_os_strncmp(&typeName[len - const_len], " const", const_len)) {
            typeName[len - const_len] = '\0';
        }
        len -= const_len;
    }

    /* Check if there are any remaining "struct " strings, which can happen
     * if this is a template type on msvc. */
    if (len > struct_len) {
        char *ptr = typeName;
        while ((ptr = strstr(ptr + 1, "struct ")) != 0) {
            /* Make sure we're not matched with part of a longer identifier
             * that contains 'struct' */
            if (ptr[-1] == '<' || ptr[-1] == ',' || isspace(ptr[-1])) {
                ecs_os_memmove(ptr, ptr + struct_len, len - struct_len);
                len -= struct_len;
            }
        }
    }
}

char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len)
{
    memcpy(type_name, func_name + ECS_FUNC_NAME_FRONT("type_name"), len);
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

char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t func_name_len)
{
    const char *last_space = strrchr(func_name, ' ');
    const char *last_paren = strrchr(func_name, ')');
    const char *last_colon = strrchr(func_name, ':');
    const char *start = (last_space > last_paren ? last_space : last_paren);
    start = start > last_colon ? start : last_colon;
    start ++;
    ecs_size_t f_len = flecs_uto(ecs_size_t, func_name_len);
    ecs_size_t len = flecs_uto(ecs_size_t, (f_len - (start - func_name) - 1));
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
    size_t size,
    size_t alignment,
    bool implicit_name)
{
    /* If entity has a name check if it matches */
    if (ecs_is_valid(world, id) && ecs_get_name(world, id) != NULL) {
        if (!implicit_name && id >= EcsFirstUserComponentId) {
#           ifndef NDEBUG
            char *path = ecs_get_path_w_sep(
                world, 0, id, "::", NULL);
            if (ecs_os_strcmp(path, name)) {
                ecs_err(
                    "component '%s' already registered with name '%s'",
                    name, path);
                ecs_abort(ECS_INCONSISTENT_NAME, NULL);
            }
            ecs_os_free(path);
#           endif
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
    ecs_entity_t ent = ecs_component_init(world, &(ecs_component_desc_t) {
        .entity.entity = id,
        .size = size,
        .alignment = alignment
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
    ecs_size_t alignment)
{
    (void)size;
    (void)alignment;

    /* If the component is not yet registered, ensure no other component
    * or entity has been registered with this name. Ensure component is 
    * looked up from root. */
    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_entity_t ent;
    if (id) {
        ent = id;
    } else {
        ent = ecs_lookup_path_w_sep(world, 0, name, "::", "::", false);
    }

    ecs_set_scope(world, prev_scope);

    /* If entity exists, compare symbol name to ensure that the component
    * we are trying to register under this name is the same */
    if (ent) {
        if (!id) {
            const char *sym = ecs_get_symbol(world, ent);
            ecs_assert(sym != NULL, ECS_MISSING_SYMBOL, 
                ecs_get_name(world, ent));
            (void)sym;

#           ifndef NDEBUG
            if (ecs_os_strcmp(sym, symbol)) {
                ecs_err(
                    "component with name '%s' is already registered for"\
                    " type '%s' (trying to register for type '%s')",
                        name, sym, symbol);
                ecs_abort(ECS_NAME_IN_USE, NULL);
            }
#           endif

        /* If an existing id was provided, it's possible that this id was
         * registered with another type. Make sure that in this case at
         * least the component size/alignment matches.
         * This allows applications to alias two different types to the same
         * id, which enables things like redefining a C type in C++ by
         * inheriting from it & adding utility functions etc. */
        } else {
            const EcsComponent *comp = ecs_get(world, ent, EcsComponent);
            if (comp) {
                ecs_assert(comp->size == size,
                    ECS_INVALID_COMPONENT_SIZE, NULL);
                ecs_assert(comp->alignment == alignment,
                    ECS_INVALID_COMPONENT_ALIGNMENT, NULL);
            } else {
                /* If the existing id is not a component, no checking is
                 * needed. */
            }
        }

    /* If no entity is found, lookup symbol to check if the component was
     * registered under a different name. */
    } else {
        ent = ecs_lookup_symbol(world, symbol, false);
        ecs_assert(ent == 0, ECS_INCONSISTENT_COMPONENT_ID, symbol);
    }

    return id;
}

ecs_entity_t ecs_cpp_component_register_explicit(
    ecs_world_t *world,
    ecs_entity_t s_id,
    ecs_entity_t id,
    const char *name,
    const char *type_name,
    const char *symbol,
    size_t size,
    size_t alignment)
{
    // If an explicit id is provided, it is possible that the symbol and
    // name differ from the actual type, as the application may alias
    // one type to another.
    if (!id) {
        if (!name) {
            // If no name was provided, retrieve the name implicitly from
            // the name_helper class.
            name = ecs_cpp_trim_module(world, type_name);
        }
    } else {
        // If an explicit id is provided but it has no name, inherit
        // the name from the type.
        if (!ecs_is_valid(world, id) || !ecs_get_name(world, id)) {
            name = ecs_cpp_trim_module(world, type_name);
        }
    }

    ecs_entity_t entity = ecs_component_init(world, &(ecs_component_desc_t){
        .entity.entity = s_id,
        .entity.name = name,
        .entity.sep = "::",
        .entity.root_sep = "::",
        .entity.symbol = symbol,
        .size = size,
        .alignment = alignment
    });

    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!s_id || s_id == entity, ECS_INTERNAL_ERROR, NULL);

    return entity;
}

ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name)
{
    ecs_entity_t prev = ecs_set_scope(world, parent);
    id = ecs_entity_init(world, &(ecs_entity_desc_t) {
        .entity = id,
        .name = name
    });
    ecs_assert(id != 0, ECS_INVALID_OPERATION, name);
    ecs_set_scope(world, prev);
    return id;
}

#endif
