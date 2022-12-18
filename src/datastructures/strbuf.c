/**
 * @file datastructures/strbuf.c
 * @brief Utility for constructing strings.
 *
 * A buffer builds up a list of elements which individually can be up to N bytes
 * large. While appending, data is added to these elements. More elements are
 * added on the fly when needed. When an application calls ecs_strbuf_get, all
 * elements are combined in one string and the element administration is freed.
 *
 * This approach prevents reallocs of large blocks of memory, and therefore
 * copying large blocks of memory when appending to a large buffer. A buffer
 * preallocates some memory for the element overhead so that for small strings
 * there is hardly any overhead, while for large strings the overhead is offset
 * by the reduced time spent on copying memory.
 * 
 * The functionality provided by strbuf is similar to std::stringstream.
 */

#include "../private_api.h"
#include <math.h>

/**
 *  stm32tpl --  STM32 C++ Template Peripheral Library
 *  Visit https://github.com/antongus/stm32tpl for new versions
 *
 *  Copyright (c) 2011-2020 Anton B. Gusev aka AHTOXA
 */

#define MAX_PRECISION	(10)
#define EXP_THRESHOLD   (3)
#define INT64_MAX_F ((double)INT64_MAX)

static const double rounders[MAX_PRECISION + 1] =
{
	0.5,				// 0
	0.05,				// 1
	0.005,				// 2
	0.0005,				// 3
	0.00005,			// 4
	0.000005,			// 5
	0.0000005,			// 6
	0.00000005,			// 7
	0.000000005,		// 8
	0.0000000005,		// 9
	0.00000000005		// 10
};

static
char* flecs_strbuf_itoa(
    char *buf,
    int64_t v)
{
    char *ptr = buf;
    char * p1;
	char c;

	if (!v) {
		*ptr++ = '0';
    } else {
        if (v < 0) {
            ptr[0] = '-';
            ptr ++;
            v *= -1;
        }

		char *p = ptr;
		while (v) {
            int64_t vdiv = v / 10;
            int64_t vmod = v - (vdiv * 10);
			p[0] = (char)('0' + vmod);
            p ++;
			v = vdiv;
		}

		p1 = p;

		while (p > ptr) {
			c = *--p;
			*p = *ptr;
			*ptr++ = c;
		}
		ptr = p1;
	}
    return ptr;
}

static
int flecs_strbuf_ftoa(
    ecs_strbuf_t *out, 
    double f, 
    int precision,
    char nan_delim)
{
    char buf[64];
	char * ptr = buf;
	char c;
	int64_t intPart;
    int64_t exp = 0;

    if (isnan(f)) {
        if (nan_delim) {
            ecs_strbuf_appendch(out, nan_delim);
            ecs_strbuf_appendlit(out, "NaN");
            return ecs_strbuf_appendch(out, nan_delim);
        } else {
            return ecs_strbuf_appendlit(out, "NaN");
        }
    }
    if (isinf(f)) {
        if (nan_delim) {
            ecs_strbuf_appendch(out, nan_delim);
            ecs_strbuf_appendlit(out, "Inf");
            return ecs_strbuf_appendch(out, nan_delim);
        } else {
            return ecs_strbuf_appendlit(out, "Inf");
        }
    }

	if (precision > MAX_PRECISION) {
		precision = MAX_PRECISION;
    }

	if (f < 0) {
		f = -f;
		*ptr++ = '-';
	}

	if (precision < 0) {
		if (f < 1.0) precision = 6;
		else if (f < 10.0) precision = 5;
		else if (f < 100.0) precision = 4;
		else if (f < 1000.0) precision = 3;
		else if (f < 10000.0) precision = 2;
		else if (f < 100000.0) precision = 1;
		else precision = 0;
	}

	if (precision) {
		f += rounders[precision];
    }

    /* Make sure that number can be represented as 64bit int, increase exp */
    while (f > INT64_MAX_F) {
        f /= 1000 * 1000 * 1000;
        exp += 9;
    }

	intPart = (int64_t)f;
	f -= (double)intPart;

    ptr = flecs_strbuf_itoa(ptr, intPart);

	if (precision) {
		*ptr++ = '.';
		while (precision--) {
			f *= 10.0;
			c = (char)f;
			*ptr++ = (char)('0' + c);
			f -= c;
		}
	}
	*ptr = 0;

    /* Remove trailing 0s */
    while ((&ptr[-1] != buf) && (ptr[-1] == '0')) {
        ptr[-1] = '\0';
        ptr --;
    }
    if (ptr != buf && ptr[-1] == '.') {
        ptr[-1] = '\0';
        ptr --;
    }

    /* If 0s before . exceed threshold, convert to exponent to save space 
     * without losing precision. */
    char *cur = ptr;
    while ((&cur[-1] != buf) && (cur[-1] == '0')) {
        cur --;
    }

    if (exp || ((ptr - cur) > EXP_THRESHOLD)) {
        cur[0] = '\0';
        exp += (ptr - cur);
        ptr = cur;
    }

    if (exp) {
        char *p1 = &buf[1];
        if (nan_delim) {
            ecs_os_memmove(buf + 1, buf, 1 + (ptr - buf));
            buf[0] = nan_delim;
            p1 ++;
        }

        /* Make sure that exp starts after first character */
        c = p1[0];

        if (c) {
            p1[0] = '.';
            do {
                char t = (++p1)[0];
                p1[0] = c;
                c = t;
                exp ++;
            } while (c);
            ptr = p1 + 1;
        } else {
            ptr = p1;
        }


        ptr[0] = 'e';
        ptr = flecs_strbuf_itoa(ptr + 1, exp);

        if (nan_delim) {
            ptr[0] = nan_delim;
            ptr ++;
        }

        ptr[0] = '\0';
    }
    
    return ecs_strbuf_appendstrn(out, buf, (int32_t)(ptr - buf));
}

/* Add an extra element to the buffer */
static
void flecs_strbuf_grow(
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
void flecs_strbuf_grow_str(
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
char* flecs_strbuf_ptr(
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
int32_t flecs_strbuf_memLeftInCurrentElement(
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
int32_t flecs_strbuf_memLeft(
    ecs_strbuf_t *b)
{
    if (b->max) {
        return b->max - b->size - b->current->pos;
    } else {
        return INT_MAX;
    }
}

static
void flecs_strbuf_init(
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
bool flecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* str,
    va_list args)
{
    bool result = true;
    va_list arg_cpy;

    if (!str) {
        return result;
    }

    flecs_strbuf_init(b);

    int32_t memLeftInElement = flecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = flecs_strbuf_memLeft(b);

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
        flecs_strbuf_ptr(b), (size_t)(max_copy + 1), str, args);

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
            flecs_strbuf_grow(b);

            /* Copy entire string to new buffer */
            ecs_os_vsprintf(flecs_strbuf_ptr(b), str, arg_cpy);

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
            flecs_strbuf_grow_str(b, dst, dst, memRequired);
        }
    }

    va_end(arg_cpy);

    return flecs_strbuf_memLeft(b) > 0;
}

static
bool flecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str,
    int n)
{
    flecs_strbuf_init(b);

    int32_t memLeftInElement = flecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = flecs_strbuf_memLeft(b);
    if (memLeft <= 0) {
        return false;
    }

    /* Never write more than what the buffer can store */
    if (n > memLeft) {
        n = memLeft;
    }

    if (n <= memLeftInElement) {
        /* Element was large enough to fit string */
        ecs_os_strncpy(flecs_strbuf_ptr(b), str, n);
        b->current->pos += n;
    } else if ((n - memLeftInElement) < memLeft) {
        ecs_os_strncpy(flecs_strbuf_ptr(b), str, memLeftInElement);

        /* Element was not large enough, but buffer still has space */
        b->current->pos += memLeftInElement;
        n -= memLeftInElement;

        /* Current element was too small, copy remainder into new element */
        if (n < ECS_STRBUF_ELEMENT_SIZE) {
            /* A standard-size buffer is large enough for the new string */
            flecs_strbuf_grow(b);

            /* Copy the remainder to the new buffer */
            if (n) {
                /* If a max number of characters to write is set, only a
                 * subset of the string should be copied to the buffer */
                ecs_os_strncpy(
                    flecs_strbuf_ptr(b),
                    str + memLeftInElement,
                    (size_t)n);
            } else {
                ecs_os_strcpy(flecs_strbuf_ptr(b), str + memLeftInElement);
            }

            /* Update to number of characters copied to new buffer */
            b->current->pos += n;
        } else {
            /* String doesn't fit in a single element, strdup */
            char *remainder = ecs_os_strdup(str + memLeftInElement);
            flecs_strbuf_grow_str(b, remainder, remainder, n);
        }
    } else {
        /* Buffer max has been reached */
        return false;
    }

    return flecs_strbuf_memLeft(b) > 0;
}

static
bool flecs_strbuf_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    flecs_strbuf_init(b);

    int32_t memLeftInElement = flecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = flecs_strbuf_memLeft(b);
    if (memLeft <= 0) {
        return false;
    }

    if (memLeftInElement) {
        /* Element was large enough to fit string */
        flecs_strbuf_ptr(b)[0] = ch;
        b->current->pos ++;
    } else {
        flecs_strbuf_grow(b);
        flecs_strbuf_ptr(b)[0] = ch;
        b->current->pos ++;
    }

    return flecs_strbuf_memLeft(b) > 0;
}

bool ecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* fmt,
    va_list args)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_strbuf_vappend(b, fmt, args);
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
    bool result = flecs_strbuf_vappend(b, fmt, args);
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
    return flecs_strbuf_appendstr(b, str, len);
}

bool ecs_strbuf_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    return flecs_strbuf_appendch(b, ch);
}

bool ecs_strbuf_appendint(
    ecs_strbuf_t *b,
    int64_t v)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    char numbuf[32];
    char *ptr = flecs_strbuf_itoa(numbuf, v);
    return ecs_strbuf_appendstrn(b, numbuf, flecs_ito(int32_t, ptr - numbuf));
}

bool ecs_strbuf_appendflt(
    ecs_strbuf_t *b,
    double flt,
    char nan_delim)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    return flecs_strbuf_ftoa(b, flt, 10, nan_delim);
}

bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *b,
    char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_strbuf_init(b);
    flecs_strbuf_grow_str(b, str, str, 0);
    return true;
}

bool ecs_strbuf_appendstr_zerocpyn(
    ecs_strbuf_t *b,
    char *str,
    int32_t n)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_strbuf_init(b);
    flecs_strbuf_grow_str(b, str, str, n);
    return true;
}

bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *b,
    const char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    /* Removes const modifier, but logic prevents changing / delete string */
    flecs_strbuf_init(b);
    flecs_strbuf_grow_str(b, (char*)str, NULL, 0);
    return true;
}

bool ecs_strbuf_appendstr_zerocpyn_const(
    ecs_strbuf_t *b,
    const char *str,
    int32_t n)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    /* Removes const modifier, but logic prevents changing / delete string */
    flecs_strbuf_init(b);
    flecs_strbuf_grow_str(b, (char*)str, NULL, n);
    return true;
}

bool ecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_strbuf_appendstr(b, str, ecs_os_strlen(str));
}

bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer)
{
    if (src_buffer->elementCount) {
        if (src_buffer->buf) {
            return ecs_strbuf_appendstrn(
                dst_buffer, src_buffer->buf, src_buffer->length);
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
        char ch = list_open[0];
        if (ch && !list_open[1]) {
            ecs_strbuf_appendch(b, ch);
        } else {
            ecs_strbuf_appendstr(b, list_open);
        }
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
        char ch = list_close[0];
        if (ch && !list_close[1]) {
            ecs_strbuf_appendch(b, list_close[0]);
        } else {
            ecs_strbuf_appendstr(b, list_close);
        }
    }
}

void ecs_strbuf_list_next(
    ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t list_sp = b->list_sp;
    if (b->list_stack[list_sp].count != 0) {
        const char *sep = b->list_stack[list_sp].separator;
        if (sep && !sep[1]) {
            ecs_strbuf_appendch(b, sep[0]);
        } else {
            ecs_strbuf_appendstr(b, sep);
        }
    }
    b->list_stack[list_sp].count ++;
}

bool ecs_strbuf_list_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_list_next(b);
    return flecs_strbuf_appendch(b, ch);
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
    bool result = flecs_strbuf_vappend(b, fmt, args);
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

bool ecs_strbuf_list_appendstrn(
    ecs_strbuf_t *b,
    const char *str,
    int32_t n)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);
    return ecs_strbuf_appendstrn(b, str, n);
}

int32_t ecs_strbuf_written(
    const ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    if (b->current) {
        return b->size + b->current->pos;
    } else {
        return 0;
    }
}
