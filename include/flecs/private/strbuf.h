#ifndef FLECS_STRBUF_H_
#define FLECS_STRBUF_H_

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STRBUF_INIT (ecs_strbuf_t){0}
#define ECS_STRBUF_ELEMENT_SIZE (511)
#define ECS_STRBUF_MAX_LIST_DEPTH (32)

/* A buffer builds up a list of elements which individually can be up to N bytes
 * large. While appending, data is added to these elements. More elements are
 * added on the fly when needed. When an application calls ecs_strbuf_get, all
 * elements are combined in one string and the element administration is freed.
 *
 * This approach prevents reallocs of large blocks of memory, and therefore
 * copying large blocks of memory when appending to a large buffer. A buffer
 * preallocates some memory for the element overhead so that for small strings
 * there is hardly any overhead, while for large strings the overhead is offset
 * by the reduced time spent on copying memory.
 */

typedef struct ecs_strbuf_element {
    bool buffer_embedded;
    int32_t pos;
    char *buf;
    struct ecs_strbuf_element *next;
} ecs_strbuf_element;

typedef struct ecs_strbuf_element_embedded {
    ecs_strbuf_element super;
    char buf[ECS_STRBUF_ELEMENT_SIZE + 1];
} ecs_strbuf_element_embedded;

typedef struct ecs_strbuf_element_str {
    ecs_strbuf_element super;
    char *alloc_str;
} ecs_strbuf_element_str;

typedef struct ecs_strbuf_list_elem {
    int32_t count;
    const char *separator;
} ecs_strbuf_list_elem;

typedef struct ecs_strbuf_t {
    /* When set by an application, append will write to this buffer */
    char *buf;

    /* The maximum number of characters that may be printed */
    int32_t max;

    /* Size of elements minus current element */
    int32_t size;

    /* The number of elements in use */
    int32_t elementCount;

    /* Always allocate at least one element */
    ecs_strbuf_element_embedded firstElement;

    /* The current element being appended to */
    ecs_strbuf_element *current;

    /* Stack that keeps track of number of list elements, used for conditionally
     * inserting a separator */
    ecs_strbuf_list_elem list_stack[ECS_STRBUF_MAX_LIST_DEPTH];
    int32_t list_sp;
} ecs_strbuf_t;

/* Append format string to a buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/* Append format string with argument list to a buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_vappend(
    ecs_strbuf_t *buffer,
    const char *fmt,
    va_list args);

/* Append string to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/* Append source buffer to destination buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer);

/* Append string to buffer, transfer ownership to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *buffer,
    char *str);

/* Append string to buffer, do not free/modify string.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *buffer,
    const char *str);

/* Append n characters to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_EXPORT
bool ecs_strbuf_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/* Return result string (also resets buffer) */
FLECS_EXPORT
char *ecs_strbuf_get(
    ecs_strbuf_t *buffer);

/* Reset buffer without returning a string */
FLECS_EXPORT
void ecs_strbuf_reset(
    ecs_strbuf_t *buffer);

/* Push a list */
FLECS_EXPORT
void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator);

/* Pop a new list */
FLECS_EXPORT
void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close);

/* Insert a new element in list */
FLECS_EXPORT
void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer);

/* Append formatted string as a new element in list */
FLECS_EXPORT
bool ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/* Append string as a new element in list */
FLECS_EXPORT
bool ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

#ifdef __cplusplus
}
#endif

#endif
