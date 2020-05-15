#include "flecs_private.h"

/* Mock types so we don't have to depend on them. 
 * TODO: Need a better workaround */
typedef uint16_t EcsAdmin;
typedef int32_t EcsConsole;

int ecs_enable_admin(
	ecs_world_t* world,
	uint16_t port)
{
    if (!ecs_import_from_library(world, "flecs.systems.civetweb", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.civetweb");
        return 1;
    }
    
    if (!ecs_import_from_library(world, "flecs.systems.admin", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.admin");
        return 1;
    }

    /* Enable monitoring */
    ecs_measure_frame_time(world, true);
    ecs_measure_system_time(world, true);

    /* Create admin instance */
    ecs_entity_t EEcsAdmin = ecs_lookup(world, "EcsAdmin");
    ecs_set(world, 0, EcsAdmin, {port});

    ecs_os_log("admin is running on port %d", port);

    return 0;
}

int ecs_enable_console(
	ecs_world_t* world)
{
    if (!ecs_import_from_library( world, "flecs.systems.console", NULL, 0)) {
        ecs_os_err("Failed to load flecs.systems.console");
        return 1;
    }

    /* Create console instance */
    ecs_entity_t EEcsConsole = ecs_lookup(world, "EcsConsole");
    ecs_set(world, 0, EcsConsole, {0});

    return 0;
}

ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t init_action,
    const char *module_name,
    int flags,
    void *handles_out,
    size_t handles_size)
{
    ecs_entity_t e = ecs_lookup(world, module_name);
    if (!e) {
        /* Load module */
        init_action(world, flags);

        /* Lookup module entity (must be registered by module) */
        e = ecs_lookup(world, module_name);
        ecs_assert(e != 0, ECS_MODULE_UNDEFINED, module_name);
    }

    /* Copy value of module component in handles_out parameter */
    if (handles_size && handles_out) {
        void *handles_ptr = ecs_get_mut_w_entity(world, e, e, NULL);
        memcpy(handles_out, handles_ptr, handles_size);        
    }    

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name,
    int flags)
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
        import_func = ecs_os_malloc(strlen(library_name) + sizeof("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = toupper(ch);
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = tolower(ch);
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_os_dbg("found file '%s' for library '%s'", 
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
        ecs_os_dbg("library '%s' ('%s') loaded", 
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
        ecs_os_dbg("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = ecs_import(world, action, module, flags, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    ecs_os_free(library_filename);

    return result;
}