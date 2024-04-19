/**
 * @file addons/expr.h
 * @brief Flecs expression parser addon.
 *
 * Parse expression strings into component values. The notation is similar to
 * JSON but with a smaller footprint, native support for (large) integer types,
 * character types, enumerations, bitmasks and entity identifiers.
 *
 * Examples:
 *
 * Member names:
 *
 *     {x: 10, y: 20}
 *
 * No member names (uses member ordering):
 *
 *     {10, 20}
 *
 * Enum values:
 *
 *     {color: Red}
 *
 * Bitmask values:
 *
 *     {toppings: Lettuce|Tomato}
 *
 * Collections:
 *
 *     {points: [10, 20, 30]}
 *
 * Nested objects:
 *
 *     {start: {x: 10, y: 20}, stop: {x: 30, y: 40}}
 *
 */

#ifdef FLECS_EXPR

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_EXPR_H
#define FLECS_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup c_addons_expr Expr
 * @ingroup c_addons
 * Serialize/deserialize values to string.
 *
 * @{
 */

/** Write an escaped character.
 * Write a character to an output string, insert escape character if necessary.
 *
 * @param out The string to write the character to.
 * @param in The input character.
 * @param delimiter The delimiter used (for example '"')
 * @return Pointer to the character after the last one written.
 */
FLECS_API
char* ecs_chresc(
    char *out,
    char in,
    char delimiter);

/** Parse an escaped character.
 * Parse a character with a potential escape sequence.
 *
 * @param in Pointer to character in input string.
 * @param out Output string.
 * @return Pointer to the character after the last one read.
 */
const char* ecs_chrparse(
    const char *in,
    char *out);

/** Write an escaped string.
 * Write an input string to an output string, escape characters where necessary.
 * To determine the size of the output string, call the operation with a NULL
 * argument for 'out', and use the returned size to allocate a string that is
 * large enough.
 *
 * @param out Pointer to output string (must be).
 * @param size Maximum number of characters written to output.
 * @param delimiter The delimiter used (for example '"').
 * @param in The input string.
 * @return The number of characters that (would) have been written.
 */
FLECS_API
ecs_size_t ecs_stresc(
    char *out,
    ecs_size_t size,
    char delimiter,
    const char *in);

/** Return escaped string.
 * Return escaped version of input string. Same as ecs_stresc(), but returns an
 * allocated string of the right size.
 *
 * @param delimiter The delimiter used (for example '"').
 * @param in The input string.
 * @return Escaped string.
 */
FLECS_API
char* ecs_astresc(
    char delimiter,
    const char *in);

/** Serialize value into expression string.
 * This operation serializes a value of the provided type to a string. The
 * memory pointed to must be large enough to contain a value of the used type.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with expression, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_expr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into expression buffer.
 * Same as ecs_ptr_to_expr(), but serializes to an ecs_strbuf_t instance.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf);

/** Similar as ecs_ptr_to_expr(), but serializes values to string.
 * Whereas the output of ecs_ptr_to_expr() is a valid expression, the output of
 * ecs_ptr_to_str() is a string representation of the value. In most cases the
 * output of the two operations is the same, but there are some differences:
 * - Strings are not quoted
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with result, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into string buffer.
 * Same as ecs_ptr_to_str(), but serializes to an ecs_strbuf_t instance.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_str_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf);

/** Serialize primitive value into string buffer.
 * Serializes a primitive value to an ecs_strbuf_t instance. This operation can
 * be reused by other serializers to avoid having to write boilerplate code that
 * serializes primitive values to a string.
 *
 * @param world The world.
 * @param kind The kind of primitive value.
 * @param data The value to serialize
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_primitive_to_expr_buf(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *data,
    ecs_strbuf_t *buf);

/** Parse expression token.
 * Expression tokens can contain more characters (such as '|') than tokens
 * parsed by the query (term) parser.
 *
 * @param name The name of the expression (used for debug logs).
 * @param expr The full expression (used for debug logs).
 * @param ptr The pointer to the expression to parse.
 * @param token The buffer to write to (must have size ECS_MAX_TOKEN_SIZE)
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char *ecs_parse_expr_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif
