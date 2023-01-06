/**
 * @file addons/json.h
 * @brief JSON parser addon.
 *
 * Parse expression strings into component values. Entity identifiers, 
 * enumerations and bitmasks are encoded as strings.
 * 
 * See docs/JsonFormat.md for a description of the JSON format.
 */

#ifdef FLECS_JSON

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#ifndef FLECS_JSON_H
#define FLECS_JSON_H

/**
 * @defgroup c_addons_json Json
 * @brief Functions for serializing to/from JSON.
 * 
 * \ingroup c_addons
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Used with ecs_parse_json. */
typedef struct ecs_parse_json_desc_t {
    const char *name; /* Name of expression (used for logging) */
    const char *expr; /* Full expression (used for logging) */
} ecs_parse_json_desc_t;

/** Parse JSON string into value.
 * This operation parses a JSON expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param type The type of the expression to parse.
 * @param data_out Pointer to the memory to write to.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_json(
    const ecs_world_t *world,
    const char *ptr,
    ecs_entity_t type,
    void *data_out,
    const ecs_parse_json_desc_t *desc);

/** Parse JSON object with multiple component values into entity. The format
 * is the same as the one outputted by ecs_entity_to_json, but at the moment
 * only supports the "ids" and "values" member.
 */
FLECS_API
const char* ecs_parse_json_values(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *ptr,
    const ecs_parse_json_desc_t *desc);

/** Serialize value into JSON string.
 * This operation serializes a value of the provided type to a JSON string. The 
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * If count is 0, the function will serialize a single value, not wrapped in
 * array brackets. If count is >= 1, the operation will serialize values to a
 * a comma-separated list inside of array brackets.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param count The number of elements to serialize.
 * @return String with JSON expression, or NULL if failed.
 */
FLECS_API
char* ecs_array_to_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    int32_t count);

/** Serialize value into JSON string buffer.
 * Same as ecs_array_to_json_buf, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param count The number of elements to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    int32_t count,
    ecs_strbuf_t *buf_out);

/** Serialize value into JSON string.
 * Same as ecs_array_to_json, with count = 0.
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

/** Serialize type info to JSON.
 * This serializes type information to JSON, and can be used to store/transmit
 * the structure of a (component) value.
 * 
 * If the provided type does not have reflection data, "0" will be returned.
 * 
 * @param world The world.
 * @param type The type to serialize to JSON.
 * @return A JSON string with the serialized type info, or NULL if failed.
 */
FLECS_API
char* ecs_type_info_to_json(
    const ecs_world_t *world,
    ecs_entity_t type);

/** Serialize type info into JSON string buffer.
 * Same as ecs_type_info_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_type_info_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf_out);

/** Used with ecs_iter_to_json. */
typedef struct ecs_entity_to_json_desc_t {
    bool serialize_path;       /**< Serialize full pathname */
    bool serialize_meta_ids;   /**< Serialize 'meta' ids (Name, ChildOf, etc) */
    bool serialize_label;      /**< Serialize doc name */
    bool serialize_brief;      /**< Serialize brief doc description */
    bool serialize_link;       /**< Serialize doc link (URL) */
    bool serialize_color;      /**< Serialize doc color */
    bool serialize_id_labels;  /**< Serialize labels of (component) ids */
    bool serialize_base;       /**< Serialize base components */
    bool serialize_private;    /**< Serialize private components */
    bool serialize_hidden;     /**< Serialize ids hidden by override */
    bool serialize_values;     /**< Serialize component values */
    bool serialize_type_info;  /**< Serialize type info (requires serialize_values) */
} ecs_entity_to_json_desc_t;

#define ECS_ENTITY_TO_JSON_INIT (ecs_entity_to_json_desc_t){true, false,\
    false, false, false, false, false, true, false, false, false, false }

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
    ecs_entity_t entity,
    const ecs_entity_to_json_desc_t *desc);

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
    ecs_strbuf_t *buf_out,
    const ecs_entity_to_json_desc_t *desc);

/** Used with ecs_iter_to_json. */
typedef struct ecs_iter_to_json_desc_t {
    bool serialize_term_ids;        /**< Serialize term (query) component ids */
    bool serialize_ids;             /**< Serialize actual (matched) component ids */
    bool serialize_sources;         /**< Serialize sources */
    bool serialize_variables;       /**< Serialize variables */
    bool serialize_is_set;          /**< Serialize is_set (for optional terms) */
    bool serialize_values;          /**< Serialize component values */
    bool serialize_entities;        /**< Serialize entities (for This terms) */
    bool serialize_entity_labels;   /**< Serialize doc name for entities */
    bool serialize_entity_ids;      /**< Serialize numerical ids for entities */
    bool serialize_variable_labels; /**< Serialize doc name for variables */
    bool serialize_variable_ids;    /**< Serialize numerical ids for variables */
    bool serialize_colors;          /**< Serialize doc color for entities */
    bool measure_eval_duration;     /**< Serialize evaluation duration */
    bool serialize_type_info;       /**< Serialize type information */
} ecs_iter_to_json_desc_t;

#define ECS_ITER_TO_JSON_INIT (ecs_iter_to_json_desc_t){\
    .serialize_term_ids =        true,  \
    .serialize_ids =             true,  \
    .serialize_sources =         true,  \
    .serialize_variables =       true,  \
    .serialize_is_set =          true,  \
    .serialize_values =          true,  \
    .serialize_entities =        true,  \
    .serialize_entity_labels =   false, \
    .serialize_entity_ids =      false, \
    .serialize_variable_labels = false, \
    .serialize_variable_ids =    false, \
    .serialize_colors =          false, \
    .measure_eval_duration =     false, \
    .serialize_type_info =       false  \
}

/** Serialize iterator into JSON string.
 * This operation will iterate the contents of the iterator and serialize them
 * to JSON. The function acccepts iterators from any source.
 * 
 * @param world The world.
 * @param iter The iterator to serialize to JSON.
 * @return A JSON string with the serialized iterator data, or NULL if failed.
 */
FLECS_API
char* ecs_iter_to_json(
    const ecs_world_t *world,
    ecs_iter_t *iter,
    const ecs_iter_to_json_desc_t *desc);

/** Serialize iterator into JSON string buffer.
 * Same as ecs_iter_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param iter The iterator to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_iter_to_json_buf(
    const ecs_world_t *world,
    ecs_iter_t *iter,
    ecs_strbuf_t *buf_out,
    const ecs_iter_to_json_desc_t *desc);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
