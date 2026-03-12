/**
 * @file strbuf.h
 * @brief Utility for constructing strings.
 */

#ifndef FLECS_STRBUF_H_
#define FLECS_STRBUF_H_

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initializer for ecs_strbuf_t. */
#ifdef __cplusplus
/* Fixes missing field initializer warning on g++ */
#define ECS_STRBUF_INIT (ecs_strbuf_t){}
#else
#define ECS_STRBUF_INIT (ecs_strbuf_t){0}
#endif

/** Size of the small string optimization buffer. */
#define ECS_STRBUF_SMALL_STRING_SIZE (512)

/** Maximum nesting depth for list operations. */
#define ECS_STRBUF_MAX_LIST_DEPTH (32)

/** Element tracking for nested list appends. */
typedef struct ecs_strbuf_list_elem {
    int32_t count;            /**< Number of elements appended to the list. */
    const char *separator;    /**< Separator string inserted between elements. */
} ecs_strbuf_list_elem;

/** A string buffer for efficient string construction. */
typedef struct ecs_strbuf_t {
    char *content;            /**< Pointer to the heap-allocated string content. */
    ecs_size_t length;        /**< Current length of the string in bytes. */
    ecs_size_t size;          /**< Allocated capacity of the content buffer. */

    ecs_strbuf_list_elem list_stack[ECS_STRBUF_MAX_LIST_DEPTH]; /**< Stack of nested list states. */
    int32_t list_sp;          /**< Current list stack pointer (nesting depth). */

    char small_string[ECS_STRBUF_SMALL_STRING_SIZE]; /**< Inline buffer for small string optimization. */
} ecs_strbuf_t;

/** Append format string to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param fmt The format string.
 */
FLECS_API
void ecs_strbuf_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/** Append format string with argument list to a buffer.
 *
 * @param buffer The buffer to append to.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_strbuf_vappend(
    ecs_strbuf_t *buffer,
    const char *fmt,
    va_list args);

/** Append string to buffer.
 *
 * @param buffer The buffer to append to.
 * @param str The string to append.
 */
FLECS_API
void ecs_strbuf_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/** Append character to buffer.
 *
 * @param buffer The buffer to append to.
 * @param ch The character to append.
 */
FLECS_API
void ecs_strbuf_appendch(
    ecs_strbuf_t *buffer,
    char ch);

/** Append int to buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The integer value to append.
 */
FLECS_API
void ecs_strbuf_appendint(
    ecs_strbuf_t *buffer,
    int64_t v);

/** Append float to buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The float value to append.
 * @param nan_delim The delimiter to use for NaN values.
 */
FLECS_API
void ecs_strbuf_appendflt(
    ecs_strbuf_t *buffer,
    double v,
    char nan_delim);

/** Append boolean to buffer.
 *
 * @param buffer The buffer to append to.
 * @param v The boolean value to append.
 */
FLECS_API
void ecs_strbuf_appendbool(
    ecs_strbuf_t *buffer,
    bool v);

/** Append source buffer to destination buffer.
 *
 * @param dst_buffer The destination buffer.
 * @param src_buffer The source buffer to append.
 */
FLECS_API
void ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer);

/** Append n characters to buffer.
 *
 * @param buffer The buffer to append to.
 * @param str The string to append from.
 * @param n The number of characters to append.
 */
FLECS_API
void ecs_strbuf_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/** Return result string.
 *
 * @param buffer The buffer to get the string from.
 * @return The result string, or NULL if empty.
 */
FLECS_API
char* ecs_strbuf_get(
    ecs_strbuf_t *buffer);

/** Return small string from first element (appends \\0).
 *
 * @param buffer The buffer to get the string from.
 * @return The small string.
 */
FLECS_API
char* ecs_strbuf_get_small(
    ecs_strbuf_t *buffer);

/** Reset buffer without returning a string.
 *
 * @param buffer The buffer to reset.
 */
FLECS_API
void ecs_strbuf_reset(
    ecs_strbuf_t *buffer);

/** Push a list.
 *
 * @param buffer The buffer.
 * @param list_open The string used to open the list.
 * @param separator The separator string inserted between elements.
 */
FLECS_API
void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator);

/** Pop a list.
 *
 * @param buffer The buffer.
 * @param list_close The string used to close the list.
 */
FLECS_API
void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close);

/** Insert a new element in list.
 *
 * @param buffer The buffer.
 */
FLECS_API
void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer);

/** Append character as new element in list.
 *
 * @param buffer The buffer.
 * @param ch The character to append.
 */
FLECS_API
void ecs_strbuf_list_appendch(
    ecs_strbuf_t *buffer,
    char ch);

/** Append formatted string as a new element in list.
 *
 * @param buffer The buffer.
 * @param fmt The format string.
 */
FLECS_API
void ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/** Append string as a new element in list.
 *
 * @param buffer The buffer.
 * @param str The string to append.
 */
FLECS_API
void ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/** Append n characters as a new element in list.
 *
 * @param buffer The buffer.
 * @param str The string to append from.
 * @param n The number of characters to append.
 */
FLECS_API
void ecs_strbuf_list_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/** Return the number of bytes written to the buffer. */
FLECS_API
int32_t ecs_strbuf_written(
    const ecs_strbuf_t *buffer);

/** Append a string literal to a buffer. Uses sizeof to determine length. */
#define ecs_strbuf_appendlit(buf, str)\
    ecs_strbuf_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

/** Append a string literal as a new element in a list. Uses sizeof to determine length. */
#define ecs_strbuf_list_appendlit(buf, str)\
    ecs_strbuf_list_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

#ifdef __cplusplus
}
#endif

#endif
