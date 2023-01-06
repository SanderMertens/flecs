/**
 * @file addons/module.h
 * @brief Module addon.
 *
 * The module addon allows for creating and importing modules. Flecs modules 
 * enable applications to organize components and systems into reusable units of
 * code that can easily be across projects.
 */

#ifdef FLECS_MODULE

/**
 * @defgroup c_addons_module Module
 * @brief Modules organize components, systems and more in reusable units of code.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MODULE_H
#define FLECS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

/** Import a module.
 * This operation will load a modules and store the public module handles in the
 * handles_out out parameter. The module name will be used to verify if the
 * module was already loaded, in which case it won't be reimported. The name
 * will be translated from PascalCase to an entity path (pascal.case) before the
 * lookup occurs.
 *
 * Module contents will be stored as children of the module entity. This 
 * prevents modules from accidentally defining conflicting identifiers. This is
 * enforced by setting the scope before and after loading the module to the 
 * module entity id.
 *
 * A more convenient way to import a module is by using the ECS_IMPORT macro.
 *
 * @param world The world.
 * @param module The module import function.
 * @param module_name The name of the module.
 * @return The module entity.
 */
FLECS_API
ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t module,
    const char *module_name);

/** Same as ecs_import, but with name to scope conversion.
 * PascalCase names are automatically converted to scoped names.
 *
 * @param world The world.
 * @param module The module import function.
 * @param module_name_c The name of the module.
 * @return The module entity.
 */
FLECS_API
ecs_entity_t ecs_import_c(
    ecs_world_t *world,
    ecs_module_action_t module,
    const char *module_name_c);

/** Import a module from a library.
 * Similar to ecs_import, except that this operation will attempt to load the
 * module from a dynamic library.
 *
 * A library may contain multiple modules, which is why both a library name and
 * a module name need to be provided. If only a library name is provided, the
 * library name will be reused for the module name.
 *
 * The library will be looked up using a canonical name, which is in the same
 * form as a module, like `flecs.components.transform`. To transform this
 * identifier to a platform specific library name, the operation relies on the
 * module_to_dl callback of the os_api which the application has to override if
 * the default does not yield the correct library name.
 *
 * @param world The world.
 * @param library_name The name of the library to load.
 * @param module_name The name of the module to load.
 */
FLECS_API
ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name);

/** Register a new module. */
FLECS_API
ecs_entity_t ecs_module_init(
    ecs_world_t *world,
    const char *c_name,
    const ecs_component_desc_t *desc);

/** Define module. */
#define ECS_MODULE_DEFINE(world, id)\
    {\
        ecs_component_desc_t desc = {0};\
        desc.entity = ecs_id(id);\
        ecs_id(id) = ecs_module_init(world, #id, &desc);\
        ecs_set_scope(world, ecs_id(id));\
    }

#define ECS_MODULE(world, id)\
    ecs_entity_t ecs_id(id) = 0; ECS_MODULE_DEFINE(world, id)\
    (void)ecs_id(id);

/** Wrapper around ecs_import.
 * This macro provides a convenient way to load a module with the world. It can
 * be used like this:
 *
 * ECS_IMPORT(world, FlecsSystemsPhysics);
 */
#define ECS_IMPORT(world, id) ecs_import_c(world, id##Import, #id);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
