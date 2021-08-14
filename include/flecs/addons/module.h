/**
 * @file module.h
 * @brief Module addon.
 *
 * The module addon allows for creating and importing modules. Flecs modules 
 * enable applications to organize components and systems into reusable units of
 * code that can easily be across projects.
 */

#ifdef FLECS_MODULE

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
 * @param module The module to load.
 * @param module_name The name of the module to load.
 * @param flags An integer that will be passed into the module import action.
 * @param handles_out A struct with handles to the module components/systems.
 * @param handles_size Size of the handles_out parameter.
 * @return The module entity.
 */
FLECS_API
ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t module,
    const char *module_name,
    void *handles_out,
    size_t handles_size);

/* Import a module from a library.
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

/** Register a new module.
 */
FLECS_API
ecs_entity_t ecs_module_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc);

/** Define module
 */
#define ECS_MODULE(world, id)\
    ecs_entity_t ecs_id(id) = ecs_module_init(world, &(ecs_component_desc_t){\
        .entity = {\
            .name = #id,\
            .add = {EcsModule}\
        },\
        .size = sizeof(id),\
        .alignment = ECS_ALIGNOF(id)\
    });\
    id *handles = (id*)ecs_get_mut(world, ecs_id(id), id, NULL);\
    ecs_set_scope(world, ecs_id(id));\
    (void)ecs_id(id);\
    (void)handles

/** Wrapper around ecs_import.
 * This macro provides a convenient way to load a module with the world. It can
 * be used like this:
 *
 * ECS_IMPORT(world, FlecsSystemsPhysics, 0);
 * 
 * This macro will define entity and type handles for the component associated
 * with the module. The module component will be created as a singleton. 
 * 
 * The contents of a module component are module specific, although they
 * typically contain handles to the content of the module.
 */
#define ECS_IMPORT(world, id) \
    id ecs_module(id);\
    char *id##__name = ecs_module_path_from_c(#id);\
    ecs_id_t ecs_id(id) = ecs_import(\
        world, id##Import, id##__name, &ecs_module(id), sizeof(id));\
    ecs_os_free(id##__name);\
    id##ImportHandles(ecs_module(id));\
    (void)ecs_id(id)

/** Utility macro for declaring a component inside a handles type */
#define ECS_DECLARE_COMPONENT(id)\
    ecs_id_t ecs_id(id)

/** Utility macro for declaring an entity inside a handles type */
#define ECS_DECLARE_ENTITY(id)\
    ecs_entity_t id\

/** Utility macro for declaring a type inside a handles type */
#define ECS_DECLARE_TYPE(id)\
    ECS_DECLARE_ENTITY(id)

/** Utility macro for setting a component in a module function */
#define ECS_SET_COMPONENT(id)\
    if (handles) handles->ecs_id(id) = ecs_id(id);\
    if (handles) handles->ecs_type(id) = ecs_type(id)

/** Utility macro for setting an entity in a module function */
#define ECS_SET_ENTITY(id)\
    if (handles) handles->id = id;

/** Utility macro for setting a type in a module function */
#define ECS_SET_TYPE(id)\
    if (handles) handles->id = id;\
    if (handles) handles->ecs_type(id) = ecs_type(id);

#define ECS_EXPORT_COMPONENT(id)\
    ECS_SET_COMPONENT(id)

#define ECS_EXPORT_ENTITY(id)\
    ECS_SET_ENTITY(id)

#define ECS_EXPORT_TYPE(id)\
    ECS_SET_TYPE(id)

/** Utility macro for importing a component */
#define ECS_IMPORT_COMPONENT(handles, id)\
    ecs_id_t ecs_id(id) = (handles).ecs_id(id); (void)ecs_id(id);\
    (void)ecs_id(id);\
    (void)ecs_type(id)

/** Utility macro for importing an entity */
#define ECS_IMPORT_ENTITY(handles, id)\
    ecs_entity_t id = (handles).id;\
    (void)id;\
    (void)ecs_type(id)

/** Utility macro for importing a type */
#define ECS_IMPORT_TYPE(handles, id)\
    ecs_entity_t id = (handles).id;\
    ecs_type_t ecs_type(id) = (handles).ecs_type(id);\
    (void)id;\
    (void)ecs_type(id)

#ifdef __cplusplus
}
#endif

#endif

#endif
