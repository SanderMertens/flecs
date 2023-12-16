/**
 * @file addons/plecs.h
 * @brief Flecs script module.
 * 
 * For script, see examples/plecs.
 */

#ifdef FLECS_PLECS

/**
 * @defgroup c_addons_plecs Flecs script
 * @brief Data definition format for loading entity data.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#ifndef FLECS_PLECS_H
#define FLECS_PLECS_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScript);

/* Script component */
typedef struct EcsScript {
    ecs_vec_t using_;
    char *script;
    ecs_vec_t prop_defaults;
    ecs_world_t *world;
} EcsScript;

/** Parse plecs string.
 * This parses a plecs string and instantiates the entities in the world.
 *
 * @param world The world.
 * @param name The script name (typically the file).
 * @param str The plecs string.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *str);

/** Parse plecs file.
 * This parses a plecs file and instantiates the entities in the world. This
 * operation is equivalent to loading the file contents and passing it to
 * ecs_plecs_from_str.
 *
 * @param world The world.
 * @param filename The plecs file name.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename);

/** Used with ecs_script_init */
typedef struct ecs_script_desc_t {
    ecs_entity_t entity;      /* Set to customize entity handle associated with script */
    const char *filename;     /* Set to load script from file */
    const char *str;          /* Set to parse script from string */
} ecs_script_desc_t;

/** Load managed script.
 * A managed script tracks which entities it creates, and keeps those entities
 * synchronized when the contents of the script are updated. When the script is
 * updated, entities that are no longer in the new version will be deleted.
 * 
 * This feature is experimental.
 * 
 * @param world The world.
 * @param desc Script descriptor.
 */
FLECS_API
ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc);

#define ecs_script(world, ...)\
    ecs_script_init(world, &(ecs_script_desc_t) __VA_ARGS__)

/** Update script with new code. 
 * 
 * @param world The world.
 * @param script The script entity.
 * @param instance An assembly instance (optional).
 * @param str The script code.
 * @param vars Optional preset variables for script parameterization.
 */
FLECS_API
int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const char *str,
    ecs_vars_t *vars);

/** Clear all entities associated with script.
 *
 * @param world The world.
 * @param script The script entity.
 * @param instance The script instance.
 */
FLECS_API
void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance);

/* Module import */
FLECS_API
void FlecsScriptImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
