/**
 * @file json.h
 * @brief JSON parser addon.
 *
 * Parse expression strings into component values. Entity identifiers, 
 * enumerations and bitmasks are encoded as strings.
 */

#ifdef FLECS_JSON

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#include "expr.h"

#ifndef FLECS_JSON_H
#define FLECS_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

/** Parse JSON string into value.
 * This operation parses a JSON expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param name The name of the expression (used for debug logs).
 * @param expr The full expression (used for debug logs).
 * @param ptr The pointer to the expression to parse.
 * @param type The type of the expression to parse.
 * @param data_out Pointer to the memory to write to.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_json(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t type,
    void *data_out);

/** Serialize value into JSON string.
 * This operation serializes a value of the provided type to a JSON string. The 
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with JSON expression, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into JSON string buffer.
 * Same as ecs_ptr_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf_out);

/** Serialize entity into JSON string.
 * This creates a JSON object with the entity's (path) name, which components
 * and tags the entity has, and the component values.
 * 
 * The operation may fail if the entity contains components with invalid values.
 * 
 * @param world The world.
 * @param entity The entity to serialize to JSON.
 * @return A JSON string with the serialized entity data, or NULL if failed.
 */
FLECS_API
char* ecs_entity_to_json(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Serialize entity into JSON string buffer.
 * Same as ecs_entity_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param entity The entity to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf_out);

#ifdef __cplusplus
}
#endif

#endif

#endif
