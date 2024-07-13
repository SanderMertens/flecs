/**
 * @file addons/json.h
 * @brief JSON parser addon.
 *
 * Parse expression strings into component values. Entity identifiers,
 * enumerations and bitmasks are encoded as strings.
 *
 * See docs/FlecsRemoteApi.md for a description of the JSON format.
 */

#ifdef FLECS_JSON

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

#ifndef FLECS_JSON_H
#define FLECS_JSON_H

/**
 * @defgroup c_addons_json Json
 * @ingroup c_addons
 * Functions for serializing to/from JSON.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Used with ecs_ptr_from_json(), ecs_entity_from_json(). */
typedef struct ecs_from_json_desc_t {
    const char *name; /**< Name of expression (used for logging) */
    const char *expr; /**< Full expression (used for logging) */

    /** Callback that allows for specifying a custom lookup function. The
     * default behavior uses ecs_lookup() */
    ecs_entity_t (*lookup_action)(
        const ecs_world_t*,
        const char *value,
        void *ctx);
    void *lookup_ctx;

    /** Require components to be registered with reflection data. When not
     * in strict mode, values for components without reflection are ignored. */
    bool strict;
} ecs_from_json_desc_t;

/** Parse JSON string into value.
 * This operation parses a JSON expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 *
 * @param world The world.
 * @param type The type of the expression to parse.
 * @param ptr Pointer to the memory to write to.
 * @param json The JSON expression to parse.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_ptr_from_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Parse JSON object with multiple component values into entity. The format
 * is the same as the one outputted by ecs_entity_to_json(), but at the moment
 * only supports the "ids" and "values" member.
 *
 * @param world The world.
 * @param entity The entity to serialize to.
 * @param json The JSON expression to parse (see entity in JSON format manual).
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Parse JSON object with multiple entities into the world. The format is the
 * same as the one outputted by ecs_world_to_json().
 *
 * @param world The world.
 * @param json The JSON expression to parse (see iterator in JSON format manual).
 * @param desc Deserialization parameters.
 * @return Last deserialized character, NULL if failed.
 */
FLECS_API
const char* ecs_world_from_json(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Same as ecs_world_from_json(), but loads JSON from file.
 *
 * @param world The world.
 * @param filename The file from which to load the JSON.
 * @param desc Deserialization parameters.
 * @return Last deserialized character, NULL if failed.
 */
FLECS_API
const char* ecs_world_from_json_file(
    ecs_world_t *world,
    const char *filename,
    const ecs_from_json_desc_t *desc);

/** Serialize array into JSON string.
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

/** Serialize array into JSON string buffer.
 * Same as ecs_array_to_json(), but serializes to an ecs_strbuf_t instance.
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
 * Same as ecs_array_to_json(), with count = 0.
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
 * Same as ecs_ptr_to_json(), but serializes to an ecs_strbuf_t instance.
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
 * Same as ecs_type_info_to_json(), but serializes to an ecs_strbuf_t instance.
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

/** Used with ecs_iter_to_json(). */
typedef struct ecs_entity_to_json_desc_t {
    bool serialize_entity_id;  /**< Serialize entity id */
    bool serialize_doc;        /**< Serialize doc attributes */
    bool serialize_full_paths; /**< Serialize full paths for tags, components and pairs */
    bool serialize_inherited;  /**< Serialize base components */
    bool serialize_values;     /**< Serialize component values */
    bool serialize_type_info;  /**< Serialize type info (requires serialize_values) */
    bool serialize_alerts;     /**< Serialize active alerts for entity */
    ecs_entity_t serialize_refs; /**< Serialize references (incoming edges) for relationship */
    bool serialize_matches;    /**< Serialize which queries entity matches with */
} ecs_entity_to_json_desc_t;

/** Utility used to initialize JSON entity serializer. */
#define ECS_ENTITY_TO_JSON_INIT (ecs_entity_to_json_desc_t){\
    .serialize_doc = false, \
    .serialize_full_paths = false, \
    .serialize_inherited = false, \
    .serialize_values = true, \
    .serialize_type_info = false, \
    .serialize_alerts = false, \
    .serialize_refs = 0, \
    .serialize_matches = false, \
}

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
 * Same as ecs_entity_to_json(), but serializes to an ecs_strbuf_t instance.
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

/** Used with ecs_iter_to_json(). */
typedef struct ecs_iter_to_json_desc_t {
    bool serialize_entity_ids;      /**< Serialize entity ids */
    bool serialize_values;          /**< Serialize component values */
    bool serialize_doc;             /**< Serialize doc attributes */
    bool serialize_var_labels;      /**< Serialize doc names of matched variables */
    bool serialize_full_paths;      /**< Serialize full paths for tags, components and pairs */
    bool serialize_fields;          /**< Serialize field data */
    bool serialize_inherited;       /**< Serialize inherited components */
    bool serialize_table;           /**< Serialize entire table vs. matched components */
    bool serialize_type_info;       /**< Serialize type information */
    bool serialize_field_info;      /**< Serialize metadata for fields returned by query */
    bool serialize_query_info;      /**< Serialize query terms */
    bool serialize_query_plan;      /**< Serialize query plan */
    bool serialize_query_profile;   /**< Profile query performance */
    bool dont_serialize_results;    /**< If true, query won't be evaluated */
    bool serialize_alerts;          /**< Serialize active alerts for entity */
    ecs_entity_t serialize_refs;    /**< Serialize references (incoming edges) for relationship */
    bool serialize_matches;         /**< Serialize which queries entity matches with */
    ecs_poly_t *query;            /**< Query object (required for serialize_query_[plan|profile]). */
} ecs_iter_to_json_desc_t;

/** Utility used to initialize JSON iterator serializer. */
#define ECS_ITER_TO_JSON_INIT (ecs_iter_to_json_desc_t){\
    .serialize_entity_ids =      false, \
    .serialize_values =          true, \
    .serialize_doc =             false, \
    .serialize_full_paths =      false, \
    .serialize_fields =          true, \
    .serialize_inherited =       false, \
    .serialize_table =           false, \
    .serialize_type_info =       false, \
    .serialize_field_info =      false, \
    .serialize_query_info =      false, \
    .serialize_query_plan =      false, \
    .serialize_query_profile =   false, \
    .dont_serialize_results =    false, \
    .serialize_alerts =          false, \
    .serialize_refs =            false, \
    .serialize_matches =         false, \
}

/** Serialize iterator into JSON string.
 * This operation will iterate the contents of the iterator and serialize them
 * to JSON. The function accepts iterators from any source.
 *
 * @param iter The iterator to serialize to JSON.
 * @return A JSON string with the serialized iterator data, or NULL if failed.
 */
FLECS_API
char* ecs_iter_to_json(
    ecs_iter_t *iter,
    const ecs_iter_to_json_desc_t *desc);

/** Serialize iterator into JSON string buffer.
 * Same as ecs_iter_to_json(), but serializes to an ecs_strbuf_t instance.
 *
 * @param iter The iterator to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_iter_to_json_buf(
    ecs_iter_t *iter,
    ecs_strbuf_t *buf_out,
    const ecs_iter_to_json_desc_t *desc);

/** Used with ecs_iter_to_json(). */
typedef struct ecs_world_to_json_desc_t {
    bool serialize_builtin;    /**< Exclude flecs modules & contents */
    bool serialize_modules;    /**< Exclude modules & contents */
} ecs_world_to_json_desc_t;

/** Serialize world into JSON string.
 * This operation iterates the contents of the world to JSON. The operation is
 * equivalent to the following code:
 *
 * @code
 * ecs_query_t *f = ecs_query(world, {
 *   .terms = {{ .id = EcsAny }}
 * });
 *
 * ecs_iter_t it = ecs_query_init(world, &f);
 * ecs_iter_to_json_desc_t desc = { .serialize_table = true };
 * ecs_iter_to_json(iter, &desc);
 * @endcode
 *
 * @param world The world to serialize.
 * @return A JSON string with the serialized iterator data, or NULL if failed.
 */
FLECS_API
char* ecs_world_to_json(
    ecs_world_t *world,
    const ecs_world_to_json_desc_t *desc);

/** Serialize world into JSON string buffer.
 * Same as ecs_world_to_json(), but serializes to an ecs_strbuf_t instance.
 *
 * @param world The world to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_world_to_json_buf(
    ecs_world_t *world,
    ecs_strbuf_t *buf_out,
    const ecs_world_to_json_desc_t *desc);



/* Legacy deserializer functions. These can be used to load a v3 JSON string and
 * save it to the new format. These functions will be removed in the next
 * release. */

FLECS_API
const char* ecs_entity_from_json_legacy(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *json,
    const ecs_from_json_desc_t *desc);

FLECS_API
const char* ecs_world_from_json_legacy(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc);

FLECS_API
const char* ecs_world_from_json_file_legacy(
    ecs_world_t *world,
    const char *filename,
    const ecs_from_json_desc_t *desc);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
