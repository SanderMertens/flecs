#include "flecs.h"

#ifdef FLECS_MODULE

#include "../private_api.h"

char* ecs_module_path_from_c(
    const char *c_name)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    const char *ptr;
    char ch;

    for (ptr = c_name; (ch = *ptr); ptr++) {
        if (isupper(ch)) {
            ch = ecs_to_i8(tolower(ch));
            if (ptr != c_name) {
                ecs_strbuf_appendstrn(&str, ".", 1);
            }
        }

        ecs_strbuf_appendstrn(&str, &ch, 1);
    }

    return ecs_strbuf_get(&str);
}

ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t init_action,
    const char *module_name,
    void *handles_out,
    size_t handles_size)
{
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t old_scope = ecs_set_scope(world, 0);
    const char *old_name_prefix = world->name_prefix;

    ecs_entity_t e = ecs_lookup_fullpath(world, module_name);
    if (!e) {
        ecs_trace_1("import %s", module_name);
        ecs_log_push();

        /* Load module */
        init_action(world);

        /* Lookup module entity (must be registered by module) */
        e = ecs_lookup_fullpath(world, module_name);
        ecs_assert(e != 0, ECS_MODULE_UNDEFINED, module_name);

        ecs_log_pop();
    }

    /* Copy value of module component in handles_out parameter */
    if (handles_size && handles_out) {
        void *handles_ptr = ecs_get_mut_w_entity(world, e, e, NULL);
        ecs_os_memcpy(handles_out, handles_ptr, ecs_from_size_t(handles_size));   
    }

    /* Restore to previous state */
    ecs_set_scope(world, old_scope);
    world->name_prefix = old_name_prefix;

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name)
{
    ecs_assert(library_name != NULL, ECS_INVALID_PARAMETER, NULL);

    char *import_func = (char*)module_name; /* safe */
    char *module = (char*)module_name;

    if (!ecs_os_api.module_to_dl || 
        !ecs_os_api.dlopen || 
        !ecs_os_api.dlproc || 
        !ecs_os_api.dlclose) 
    {
        ecs_os_err(
            "library loading not supported, set module_to_dl, dlopen, dlclose "
            "and dlproc os API callbacks first");
        return 0;
    }

    /* If no module name is specified, try default naming convention for loading
     * the main module from the library */
    if (!import_func) {
        import_func = ecs_os_malloc(ecs_os_strlen(library_name) + ECS_SIZEOF("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = ecs_to_i8(toupper(ch));
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = ecs_to_i8(tolower(ch));
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        ecs_os_strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_trace_1("found file '%s' for library '%s'", 
            library_filename, library_name);
    }

    ecs_os_dl_t dl = ecs_os_dlopen(library_filename);
    if (!dl) {
        ecs_os_err("failed to load library '%s' ('%s')", 
            library_name, library_filename);
        
        ecs_os_free(library_filename);

        if (module != module_name) {
            ecs_os_free(module);
        }    

        return 0;
    } else {
        ecs_trace_1("library '%s' ('%s') loaded", 
            library_name, library_filename);
    }

    ecs_module_action_t action = (ecs_module_action_t)
        ecs_os_dlproc(dl, import_func);
    if (!action) {
        ecs_os_err("failed to load import function %s from library %s",
            import_func, library_name);
        ecs_os_free(library_filename);
        ecs_os_dlclose(dl);            
        return 0;
    } else {
        ecs_trace_1("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = ecs_import(world, action, module, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    ecs_os_free(library_filename);

    return result;
}

ecs_entity_t ecs_new_module(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    if (!e) {
        char *module_path = ecs_module_path_from_c(name);
        e = ecs_new_from_fullpath(world, module_path);

        EcsName *name_ptr = ecs_get_mut(world, e, EcsName, NULL);
        name_ptr->symbol = name;

        ecs_os_free(module_path);
    }

    ecs_entity_t result = ecs_new_component(world, e, NULL, size, alignment);
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);

    /* Add module tag */
    ecs_add_entity(world, result, EcsModule);

    /* Add module to itself. This way we have all the module information stored
     * in a single contained entity that we can use for namespacing */
    ecs_set_ptr_w_entity(world, result, result, size, NULL);

    /* Set the current scope to the module */
    ecs_set_scope(world, result);

    return result;
}

#endif
