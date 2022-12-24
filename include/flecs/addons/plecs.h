/**
 * @file addons/plecs.h
 * @brief Plecs addon.
 *
 * Plecs is a small data definition language for instantiating entities that
 * reuses the existing flecs query parser. The following examples illustrate
 * how a plecs snippet translates to regular flecs operations:
 *
 * Plecs:
 *   Entity
 * C code:
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *
 * Plecs:
 *   Position(Entity)
 * C code:
 *   ecs_entity_t Position = ecs_set_name(world, 0, "Position"); 
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *   ecs_add_id(world, Entity, Position);
 *
 * Plecs:
 *   Likes(Entity, Apples)
 * C code:
 *   ecs_entity_t Likes = ecs_set_name(world, 0, "Likes"); 
 *   ecs_entity_t Apples = ecs_set_name(world, 0, "Apples"); 
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *   ecs_add_pair(world, Entity, Likes, Apples);
 *
 * A plecs string may contain multiple statements, separated by a newline:
 *   Likes(Entity, Apples)
 *   Likes(Entity, Pears)
 *   Likes(Entity, Bananas)
 */

#ifdef FLECS_PLECS

/**
 * @defgroup c_addons_plecs Plecs
 * @brief Data definition format for loading entity data.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_PLECS_H
#define FLECS_PLECS_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
