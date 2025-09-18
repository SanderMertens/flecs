/**
 * @file api_support.h
 * @brief Support functions and constants.
 *
 * Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests.
 * 
 * Operations may change without warning.
 */

#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

#include "api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This is the largest possible component id. Components for the most part
 * occupy the same id range as entities, however they are not allowed to overlap
 * with (8) bits reserved for id flags. */
#define ECS_MAX_COMPONENT_ID (~((uint32_t)(ECS_ID_FLAGS_MASK >> 32)))

/** The maximum number of nested function calls before the core will throw a
 * cycle detected error */
#define ECS_MAX_RECURSION (512)

/** Maximum length of a parser token (used by parser-related addons) */
#define ECS_MAX_TOKEN_SIZE (256)

/** Convert a C module name into a path.
 * This operation converts a PascalCase name to a path, for example MyFooModule
 * into my.foo.module.
 * 
 * @param c_name The C module name
 * @return The path.
*/
FLECS_API
char* flecs_module_path_from_c(
    const char *c_name);

/** Constructor that zeromem's a component value.
 * 
 * @param ptr Pointer to the value.
 * @param count Number of elements to construct.
 * @param type_info Type info for the component.
 */
FLECS_API
void flecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *type_info);

/** Create allocated string from format. 
 * 
 * @param fmt The format string.
 * @param args Format arguments.
 * @return The formatted string.
 */
FLECS_DBG_API
char* flecs_vasprintf(
    const char *fmt,
    va_list args);

/** Create allocated string from format. 
 * 
 * @param fmt The format string.
 * @return The formatted string.
 */
FLECS_API
char* flecs_asprintf(
    const char *fmt,
    ...);

/** Write an escaped character.
 * Write a character to an output string, insert escape character if necessary.
 *
 * @param out The string to write the character to.
 * @param in The input character.
 * @param delimiter The delimiter used (for example '"')
 * @return Pointer to the character after the last one written.
 */
FLECS_API
char* flecs_chresc(
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
const char* flecs_chrparse(
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
ecs_size_t flecs_stresc(
    char *out,
    ecs_size_t size,
    char delimiter,
    const char *in);

/** Return escaped string.
 * Return escaped version of input string. Same as flecs_stresc(), but returns an
 * allocated string of the right size.
 *
 * @param delimiter The delimiter used (for example '"').
 * @param in The input string.
 * @return Escaped string.
 */
FLECS_API
char* flecs_astresc(
    char delimiter,
    const char *in);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 *
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* flecs_parse_ws_eol(
    const char *ptr);

/** Parse digit.
 * This function will parse until the first non-digit character is found. The
 * provided expression must contain at least one digit character.
 *
 * @param ptr The expression to parse.
 * @param token The output buffer.
 * @return Pointer to the first non-digit character.
 */
FLECS_API
const char* flecs_parse_digit(
    const char *ptr,
    char *token);

/* Convert identifier to snake case */
FLECS_API
char* flecs_to_snake_case(
    const char *str);

/* Suspend/resume readonly state. To fully support implicit registration of
 * components, it should be possible to register components while the world is
 * in readonly mode. It is not uncommon that a component is used first from
 * within a system, which are often ran while in readonly mode.
 * 
 * Suspending readonly mode is only allowed when the world is not multithreaded.
 * When a world is multithreaded, it is not safe to (even temporarily) leave
 * readonly mode, so a multithreaded application should always explicitly
 * register components in advance. 
 * 
 * These operations also suspend deferred mode.
 * 
 * Functions are public to support language bindings.
 */
typedef struct ecs_suspend_readonly_state_t {
    bool is_readonly;
    bool is_deferred;
    bool cmd_flushing;
    int32_t defer_count;
    ecs_entity_t scope;
    ecs_entity_t with;
    ecs_commands_t cmd_stack[2];
    ecs_commands_t *cmd;
    ecs_stage_t *stage;
} ecs_suspend_readonly_state_t;

FLECS_API
ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

FLECS_API
void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

/** Number of observed entities in table.
 * Operation is public to support test cases.
 * 
 * @param table The table.
 */
FLECS_DBG_API
int32_t flecs_table_observed_count(
    const ecs_table_t *table);

/** Print backtrace to specified stream.
 * 
 * @param stream The stream to use for printing the backtrace.
 */
FLECS_DBG_API
void flecs_dump_backtrace(
    void *stream);

/** Increase refcount of poly object.
 * 
 * @param poly The poly object.
 * @return The refcount after incrementing.
 */
FLECS_API
int32_t flecs_poly_claim_(
    ecs_poly_t *poly);

/** Decrease refcount of poly object.
 * 
 * @param poly The poly object.
 * @return The refcount after decrementing.
 */
FLECS_API
int32_t flecs_poly_release_(
    ecs_poly_t *poly);

#define flecs_poly_claim(poly) \
    flecs_poly_claim_(ECS_CONST_CAST(void*, reinterpret_cast<const void*>(poly)))

#define flecs_poly_release(poly) \
    flecs_poly_release_(ECS_CONST_CAST(void*, reinterpret_cast<const void*>(poly)))

/** Return refcount of poly object.
 * 
 * @param poly The poly object.
 * @return Refcount of the poly object.
 */
FLECS_API
int32_t flecs_poly_refcount(
    ecs_poly_t *poly);

/** Get unused index for static world local component id array. 
 * This operation returns an unused index for the world-local component id 
 * array. This index can be used by language bindings to obtain a component id.
 * 
 * @return Unused index for component id array.
 */
FLECS_API
int32_t flecs_component_ids_index_get(void);

/** Get world local component id.
 * 
 * @param world The world.
 * @param index Component id array index.
 * @return The component id.
 */
FLECS_API
ecs_entity_t flecs_component_ids_get(
    const ecs_world_t *world, 
    int32_t index);

/** Get alive world local component id.
 * Same as flecs_component_ids_get, but return 0 if component is no longer 
 * alive.
 * 
 * @param world The world.
 * @param index Component id array index.
 * @return The component id.
 */
FLECS_API
ecs_entity_t flecs_component_ids_get_alive(
    const ecs_world_t *world, 
    int32_t index);

/** Set world local component id. 
 * 
 * @param world The world.
 * @param index Component id array index.
 * @param id The component id.
 */
FLECS_API
void flecs_component_ids_set(
    ecs_world_t *world, 
    int32_t index,
    ecs_entity_t id);

/** Query iterator function for trivially cached queries.
 * This operation can be called if an iterator matches the conditions for 
 * trivial iteration:
 * 
 * @param it The query iterator.
 * @return Whether the query has more results.
 */
FLECS_API
bool flecs_query_trivial_cached_next(
    ecs_iter_t *it);

#ifdef FLECS_DEBUG
/** Check if current thread has exclusive access to world.
 * This operation checks if the current thread is allowed to access the world.
 * The operation is called by internal functions before mutating the world, and
 * will panic if the current thread does not have exclusive access to the world.
 * 
 * Exclusive access is controlled by the ecs_exclusive_access_begin() and
 * ecs_exclusive_access_end() operations.
 * 
 * This operation is public so that it shows up in stack traces, but code such
 * as language bindings or wrappers could also use it to verify that the world
 * is accessed from the correct thread.
 * 
 * @param world The world.
 */
FLECS_API
void flecs_check_exclusive_world_access_write(
    const ecs_world_t *world);

/** Same as flecs_check_exclusive_world_access_write, but for read access. 
 * 
 * @param world The world.
 */
FLECS_API
void flecs_check_exclusive_world_access_read(
    const ecs_world_t *world);

#else
#define flecs_check_exclusive_world_access_write(world)
#define flecs_check_exclusive_world_access_read(world)
#endif


/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif
#define ECS_OFFSET_T(o, T) ECS_OFFSET(o, ECS_SIZEOF(T))

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))
#define ECS_ELEM_T(o, T, index) ECS_ELEM(o, ECS_SIZEOF(T), index)

/** Enable/disable bitsets */
#define ECS_BIT_SET(flags, bit) (flags) |= (bit)
#define ECS_BIT_CLEAR(flags, bit) (flags) &= ~(bit) 
#define ECS_BIT_COND(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR(flags, bit)))

#define ECS_BIT_CLEAR16(flags, bit) (flags) &= (ecs_flags16_t)~(bit)   
#define ECS_BIT_COND16(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR16(flags, bit)))

#define ECS_BIT_IS_SET(flags, bit) ((flags) & (bit))

#define ECS_BIT_SETN(flags, n) ECS_BIT_SET(flags, 1llu << n)
#define ECS_BIT_CLEARN(flags, n) ECS_BIT_CLEAR(flags, 1llu << n)
#define ECS_BIT_CONDN(flags, n, cond) ECS_BIT_COND(flags, 1llu << n, cond)

#ifdef __cplusplus
}
#endif

#endif
