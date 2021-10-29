#include "../private_api.h"

/* Add an extra element to the buffer */
static
void ecs_strbuf_grow(
    ecs_strbuf_t *b)
{
    /* Allocate new element */
    ecs_strbuf_element_embedded *e = ecs_os_malloc_t(ecs_strbuf_element_embedded);
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = true;
    e->super.buf = e->buf;
    e->super.pos = 0;
    e->super.next = NULL;
}

/* Add an extra dynamic element */
static
void ecs_strbuf_grow_str(
    ecs_strbuf_t *b,
    char *str,
    char *alloc_str,
    int32_t size)
{
    /* Allocate new element */
    ecs_strbuf_element_str *e = ecs_os_malloc_t(ecs_strbuf_element_str);
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = false;
    e->super.pos = size ? size : (int32_t)ecs_os_strlen(str);
    e->super.next = NULL;
    e->super.buf = str;
    e->alloc_str = alloc_str;
}

static
char* ecs_strbuf_ptr(
    ecs_strbuf_t *b)
{
    if (b->buf) {
        return &b->buf[b->current->pos];
    } else {
        return &b->current->buf[b->current->pos];
    }
}

/* Compute the amount of space left in the current element */
static
int32_t ecs_strbuf_memLeftInCurrentElement(
    ecs_strbuf_t *b)
{
    if (b->current->buffer_embedded) {
        return ECS_STRBUF_ELEMENT_SIZE - b->current->pos;
    } else {
        return 0;
    }
}

/* Compute the amount of space left */
static
int32_t ecs_strbuf_memLeft(
    ecs_strbuf_t *b)
{
    if (b->max) {
        return b->max - b->size - b->current->pos;
    } else {
        return INT_MAX;
    }
}

static
void ecs_strbuf_init(
    ecs_strbuf_t *b)
{
    /* Initialize buffer structure only once */
    if (!b->elementCount) {
        b->size = 0;
        b->firstElement.super.next = NULL;
        b->firstElement.super.pos = 0;
        b->firstElement.super.buffer_embedded = true;
        b->firstElement.super.buf = b->firstElement.buf;
        b->elementCount ++;
        b->current = (ecs_strbuf_element*)&b->firstElement;
    }
}

/* Append a format string to a buffer */
static
bool vappend(
    ecs_strbuf_t *b,
    const char* str,
    va_list args)
{
    bool result = true;
    va_list arg_cpy;

    if (!str) {
        return result;
    }

    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);

    if (!memLeft) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    va_copy(arg_cpy, args);
    memRequired = vsnprintf(
        ecs_strbuf_ptr(b), (size_t)(max_copy + 1), str, args);

    ecs_assert(memRequired != -1, ECS_INTERNAL_ERROR, NULL);

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* If string is a format string, a new buffer of size memRequired is
         * needed to re-evaluate the format string and only use the part that
         * wasn't already copied to the previous element */
        if (memRequired <= ECS_STRBUF_ELEMENT_SIZE) {
            /* Resulting string fits in standard-size buffer. Note that the
             * entire string needs to fit, not just the remainder, as the
             * format string cannot be partially evaluated */
            ecs_strbuf_grow(b);

            /* Copy entire string to new buffer */
            ecs_os_vsprintf(ecs_strbuf_ptr(b), str, arg_cpy);

            /* Ignore the part of the string that was copied into the
             * previous buffer. The string copied into the new buffer could
             * be memmoved so that only the remainder is left, but that is
             * most likely more expensive than just keeping the entire
             * string. */

            /* Update position in buffer */
            b->current->pos += memRequired;
        } else {
            /* Resulting string does not fit in standard-size buffer.
             * Allocate a new buffer that can hold the entire string. */
            char *dst = ecs_os_malloc(memRequired + 1);
            ecs_os_vsprintf(dst, str, arg_cpy);
            ecs_strbuf_grow_str(b, dst, dst, memRequired);
        }
    }

    va_end(arg_cpy);

    return ecs_strbuf_memLeft(b) > 0;
}

static
bool appendstr(
    ecs_strbuf_t *b,
    const char* str,
    int n)
{
    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);
    if (memLeft <= 0) {
        return false;
    }

    /* Never write more than what the buffer can store */
    if (n > memLeft) {
        n = memLeft;
    }

    if (n <= memLeftInElement) {
        /* Element was large enough to fit string */
        ecs_os_strncpy(ecs_strbuf_ptr(b), str, n);
        b->current->pos += n;
    } else if ((n - memLeftInElement) < memLeft) {
        ecs_os_strncpy(ecs_strbuf_ptr(b), str, memLeftInElement);

        /* Element was not large enough, but buffer still has space */
        b->current->pos += memLeftInElement;
        n -= memLeftInElement;

        /* Current element was too small, copy remainder into new element */
        if (n < ECS_STRBUF_ELEMENT_SIZE) {
            /* A standard-size buffer is large enough for the new string */
            ecs_strbuf_grow(b);

            /* Copy the remainder to the new buffer */
            if (n) {
                /* If a max number of characters to write is set, only a
                 * subset of the string should be copied to the buffer */
                ecs_os_strncpy(
                    ecs_strbuf_ptr(b),
                    str + memLeftInElement,
                    (size_t)n);
            } else {
                ecs_os_strcpy(ecs_strbuf_ptr(b), str + memLeftInElement);
            }

            /* Update to number of characters copied to new buffer */
            b->current->pos += n;
        } else {
            char *remainder = ecs_os_strdup(str);
            ecs_strbuf_grow_str(b, remainder, remainder, n);
        }
    } else {
        /* Buffer max has been reached */
        return false;
    }

    return ecs_strbuf_memLeft(b) > 0;
}

bool ecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* fmt,
    va_list args)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);
    return vappend(b, fmt, args);
}

bool ecs_strbuf_append(
    ecs_strbuf_t *b,
    const char* fmt,
    ...)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);

    va_list args;
    va_start(args, fmt);
    bool result = vappend(b, fmt, args);
    va_end(args);

    return result;
}

bool ecs_strbuf_appendstrn(
    ecs_strbuf_t *b,
    const char* str,
    int32_t len)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    return appendstr(b, str, len);
}

bool ecs_strbuf_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    return ecs_strbuf_appendstrn(b, &ch, 1);
}

bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *b,
    char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, str, str, 0);
    return true;
}

bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *b,
    const char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    /* Removes const modifier, but logic prevents changing / delete string */
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, (char*)str, NULL, 0);
    return true;
}

bool ecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    return appendstr(b, str, ecs_os_strlen(str));
}

bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer)
{
    if (src_buffer->elementCount) {
        if (src_buffer->buf) {
            return ecs_strbuf_appendstr(dst_buffer, src_buffer->buf);
        } else {
            ecs_strbuf_element *e = (ecs_strbuf_element*)&src_buffer->firstElement;

            /* Copy first element as it is inlined in the src buffer */
            ecs_strbuf_appendstrn(dst_buffer, e->buf, e->pos);

            while ((e = e->next)) {
                dst_buffer->current->next = ecs_os_malloc(sizeof(ecs_strbuf_element));
                *dst_buffer->current->next = *e;
            }
        }

        *src_buffer = ECS_STRBUF_INIT;
    }

    return true;
}

char* ecs_strbuf_get(
    ecs_strbuf_t *b) 
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    char* result = NULL;
    if (b->elementCount) {
        if (b->buf) {
            b->buf[b->current->pos] = '\0';
            result = ecs_os_strdup(b->buf);
        } else {
            void *next = NULL;
            int32_t len = b->size + b->current->pos + 1;

            ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;

            result = ecs_os_malloc(len);
            char* ptr = result;

            do {
                ecs_os_memcpy(ptr, e->buf, e->pos);
                ptr += e->pos;
                next = e->next;
                if (e != &b->firstElement.super) {
                    if (!e->buffer_embedded) {
                        ecs_os_free(((ecs_strbuf_element_str*)e)->alloc_str);
                    }
                    ecs_os_free(e);
                }
            } while ((e = next));

            result[len - 1] = '\0';
            b->length = len;
        }
    } else {
        result = NULL;
    }

    b->elementCount = 0;

    b->content = result;

    return result;
}

char *ecs_strbuf_get_small(
    ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t written = ecs_strbuf_written(b);
    ecs_assert(written <= ECS_STRBUF_ELEMENT_SIZE, ECS_INVALID_OPERATION, NULL);
    char *buf = b->firstElement.buf;
    buf[written] = '\0';
    return buf;
}

void ecs_strbuf_reset(
    ecs_strbuf_t *b) 
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    if (b->elementCount && !b->buf) {
        void *next = NULL;
        ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;
        do {
            next = e->next;
            if (e != (ecs_strbuf_element*)&b->firstElement) {
                ecs_os_free(e);
            }
        } while ((e = next));
    }

    *b = ECS_STRBUF_INIT;
}

void ecs_strbuf_list_push(
    ecs_strbuf_t *b,
    const char *list_open,
    const char *separator)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(list_open != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(separator != NULL, ECS_INVALID_PARAMETER, NULL);

    b->list_sp ++;
    b->list_stack[b->list_sp].count = 0;
    b->list_stack[b->list_sp].separator = separator;

    if (list_open) {
        ecs_strbuf_appendstr(b, list_open);
    }
}

void ecs_strbuf_list_pop(
    ecs_strbuf_t *b,
    const char *list_close)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(list_close != NULL, ECS_INVALID_PARAMETER, NULL);

    b->list_sp --;
    
    if (list_close) {
        ecs_strbuf_appendstr(b, list_close);
    }
}

void ecs_strbuf_list_next(
    ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t list_sp = b->list_sp;
    if (b->list_stack[list_sp].count != 0) {
        ecs_strbuf_appendstr(b, b->list_stack[list_sp].separator);
    }
    b->list_stack[list_sp].count ++;
}

bool ecs_strbuf_list_append(
    ecs_strbuf_t *b,
    const char *fmt,
    ...)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);

    va_list args;
    va_start(args, fmt);
    bool result = vappend(b, fmt, args);
    va_end(args);

    return result;
}

bool ecs_strbuf_list_appendstr(
    ecs_strbuf_t *b,
    const char *str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);
    return ecs_strbuf_appendstr(b, str);
}

int32_t ecs_strbuf_written(
    const ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    return b->size + b->current->pos;
}
