/**
 * @file datastructures/strbuf.c
 * @brief Utility for constructing strings.
 *
 * A buffer builds up a string by appending to a contiguous buffer. For small
 * strings, a stack-allocated buffer is used. When the string outgrows the small
 * buffer, a heap-allocated buffer is used instead, which is grown by doubling
 * its size as needed. When an application calls ecs_strbuf_get, the final
 * string is returned and the buffer is reset.
 *
 * The functionality provided by strbuf is similar to std::stringstream.
 */

#include "../private_api.h"

#include <math.h> // isnan, isinf

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

static const double pow10s[MAX_PRECISION + 1] =
{
	1.0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10
};

static
char* flecs_strbuf_itoa(
    char *buf,
    int64_t v)
{
    char *ptr = buf;
    char * p1;
	char c;
    uint64_t uv;

    if (v < 0) {
        ptr[0] = '-';
        ptr ++;
        uv = (uint64_t)0 - (uint64_t)v;
    } else {
        uv = (uint64_t)v;
    }

	if (!uv) {
		*ptr++ = '0';
    } else {
		char *p = ptr;
		while (uv) {
            uint64_t vdiv = uv / 10;
            uint64_t vmod = uv - (vdiv * 10);
			p[0] = (char)('0' + (char)vmod);
            p ++;
			uv = vdiv;
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
void flecs_strbuf_ftoa(
    ecs_strbuf_t *out, 
    double f, 
    int precision,
    char nan_delim)
{
    char *buf, *ptr;
	char c;
	int64_t intPart;
    int64_t exp = 0;

    if (ecs_os_isnan(f)) {
        if (nan_delim) {
            ecs_strbuf_appendch(out, nan_delim);
            ecs_strbuf_appendlit(out, "NaN");
            ecs_strbuf_appendch(out, nan_delim);
            return;
        } else {
            ecs_strbuf_appendlit(out, "NaN");
            return;
        }
    }
    if (ecs_os_isinf(f)) {
        if (nan_delim) {
            ecs_strbuf_appendch(out, nan_delim);
            ecs_strbuf_appendlit(out, "Inf");
            ecs_strbuf_appendch(out, nan_delim);
            return;
        } else {
            ecs_strbuf_appendlit(out, "Inf");
            return;
        }
    }

	if (precision > MAX_PRECISION) {
		precision = MAX_PRECISION;
    }

    buf = flecs_strbuf_reserve(out, 64);
    ptr = buf;

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
    while (f >= INT64_MAX_F) {
        f /= 1000 * 1000 * 1000;
        exp += 9;
    }

	intPart = (int64_t)f;
	f -= (double)intPart;

    ptr = flecs_strbuf_itoa(ptr, intPart);

	if (precision) {
		uint64_t frac = (uint64_t)(f * pow10s[precision]);
		int32_t digits = precision;
		if (!frac) {
			digits = 0;
		}
		while (digits >= 4 && !(frac % 10000)) {
			frac /= 10000;
			digits -= 4;
		}
		while (digits && !(frac % 10)) {
			frac /= 10;
			digits --;
		}
		if (digits) {
			*ptr++ = '.';
			int32_t i;
			for (i = digits - 1; i >= 0; i --) {
				ptr[i] = (char)('0' + (char)(frac % 10));
				frac /= 10;
			}
			ptr += digits;
		}
	}
	*ptr = 0;

    /* If trailing zeros exceed the threshold, convert to exponent notation to
     * save space without losing precision. */
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

        /* Place decimal point after the first digit */
        c = p1[0];

        if (c) {
            p1[0] = '.';
            do {
                char t = (++p1)[0];
                if (t == '.') {
                    exp ++;
                    p1 --;
                    break;
                }
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

    out->length += (int32_t)(ptr - buf);
}

/* Grow the buffer */
static
void flecs_strbuf_grow(
    ecs_strbuf_t *b)
{
    if (!b->content) {
        b->content = b->small_string;
        b->size = ECS_STRBUF_SMALL_STRING_SIZE;
    } else if (b->content == b->small_string) {
        b->size *= 2;
        b->content = ecs_os_malloc_n(char, b->size);
        ecs_os_memcpy(b->content, b->small_string, b->length);
    } else {
        b->size *= 2;
        if (b->size < 16) b->size = 16;
        b->content = ecs_os_realloc_n(b->content, char, b->size);
    }
}

static
char* flecs_strbuf_ptr(
    ecs_strbuf_t *b)
{
    ecs_assert(b->content != NULL, ECS_INTERNAL_ERROR, NULL);
    return &b->content[b->length];
}

char* flecs_strbuf_reserve(
    ecs_strbuf_t *b,
    int32_t n)
{
    int32_t mem_left = b->size - b->length;
    while (n >= mem_left) {
        flecs_strbuf_grow(b);
        mem_left = b->size - b->length;
    }

    return &b->content[b->length];
}

/* Append a format string to a buffer */
static
void flecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* str,
    va_list args)
{
    va_list arg_cpy;

    if (!str) {
        return;
    }

    /* Compute the memory required to add the string to the buffer. If the
     * buffer has already been allocated, use space left in buffer, otherwise
     * use zero. */
    int32_t mem_left = b->size - b->length;
    int32_t mem_required;

    va_copy(arg_cpy, args);

    if (b->content) {
        mem_required = ecs_os_vsnprintf(
            flecs_strbuf_ptr(b), 
                flecs_itosize(mem_left), str, args);
    } else {
        mem_required = ecs_os_vsnprintf(NULL, 0, str, args);
        mem_left = 0;
    }

    ecs_assert(mem_required != -1, ECS_INTERNAL_ERROR, NULL);

    if ((mem_required + 1) >= mem_left) {
        while ((mem_required + 1) >= mem_left) {
            flecs_strbuf_grow(b);
            mem_left = b->size - b->length;
        }
        ecs_os_vsnprintf(flecs_strbuf_ptr(b), 
            flecs_itosize(mem_required + 1), str, arg_cpy);
    }

    b->length += mem_required;

    va_end(arg_cpy);
}

static
void flecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str,
    int n)
{
    int32_t mem_left = b->size - b->length;
    while (n >= mem_left) {
        flecs_strbuf_grow(b);
        mem_left = b->size - b->length;
    }

    char *dst = flecs_strbuf_ptr(b);
    if (n < 16) {
        int32_t i;
        for (i = 0; i < n; i ++) {
            dst[i] = str[i];
        }
    } else {
        ecs_os_memcpy(dst, str, n);
    }
    b->length += n;
}

static
void flecs_strbuf_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    if (b->size == b->length) {
        flecs_strbuf_grow(b);
    }

    flecs_strbuf_ptr(b)[0] = ch;
    b->length ++;
}

void ecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* fmt,
    va_list args)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_strbuf_vappend(b, fmt, args);
}

void ecs_strbuf_append(
    ecs_strbuf_t *b,
    const char* fmt,
    ...)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);

    va_list args;
    va_start(args, fmt);
    flecs_strbuf_vappend(b, fmt, args);
    va_end(args);
}

void ecs_strbuf_appendstrn(
    ecs_strbuf_t *b,
    const char* str,
    int32_t len)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_strbuf_appendstr(b, str, len);
}

void ecs_strbuf_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    flecs_strbuf_appendch(b, ch);
}

void ecs_strbuf_appendint(
    ecs_strbuf_t *b,
    int64_t v)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    char *numbuf = flecs_strbuf_reserve(b, 32);
    char *ptr = flecs_strbuf_itoa(numbuf, v);
    b->length += flecs_ito(int32_t, ptr - numbuf);
}

void ecs_strbuf_appendflt(
    ecs_strbuf_t *b,
    double flt,
    char nan_delim)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL); 
    flecs_strbuf_ftoa(b, flt, 10, nan_delim);
}

void ecs_strbuf_appendbool(
    ecs_strbuf_t *buffer,
    bool v)
{
    ecs_assert(buffer != NULL, ECS_INVALID_PARAMETER, NULL); 
    if (v) {
        ecs_strbuf_appendlit(buffer, "true");
    } else {
        ecs_strbuf_appendlit(buffer, "false");
    }
}

void ecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_strbuf_appendstr(b, str, ecs_os_strlen(str));
}

void ecs_strbuf_mergebuff(
    ecs_strbuf_t *b,
    ecs_strbuf_t *src)
{
    if (src->content && src->length) {
        flecs_strbuf_appendstr(b, src->content, src->length);
    }
    ecs_strbuf_reset(src);
}

char* ecs_strbuf_get(
    ecs_strbuf_t *b) 
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    char *result = b->content;
    if (!result) {
        return NULL;
    }

    ecs_strbuf_appendch(b, '\0');
    result = b->content;

#ifdef FLECS_SANITIZE
    ecs_assert(ecs_os_strlen(result) <= (b->length - 1), 
        ECS_INTERNAL_ERROR, NULL);
#endif

    if (result == b->small_string) {
        result = ecs_os_memdup_n(result, char, b->length);
    }

    b->length = 0;
    b->content = NULL;
    b->size = 0;
    b->list_sp = 0;
    return result;
}

char* ecs_strbuf_get_small(
    ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    char *result = b->content;
    result[b->length] = '\0';
    b->length = 0;
    b->content = NULL;
    b->size = 0;
    return result;
}

void ecs_strbuf_reset(
    ecs_strbuf_t *b) 
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    if (b->content && b->content != b->small_string) {
        ecs_os_free(b->content);
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
    ecs_assert(b->list_sp >= 0, ECS_INVALID_OPERATION, 
        "strbuf list is corrupt");
    b->list_sp ++;
    ecs_assert(b->list_sp < ECS_STRBUF_MAX_LIST_DEPTH, 
        ECS_INVALID_OPERATION, "max depth for strbuf list stack exceeded");

    b->list_stack[b->list_sp].count = 0;
    b->list_stack[b->list_sp].separator = separator;
    b->list_stack[b->list_sp].separator_len =
        separator ? ecs_os_strlen(separator) : 0;

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
    ecs_assert(b->list_sp > 0, ECS_INVALID_OPERATION, 
        "pop called more often than push for strbuf list");

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

    ecs_strbuf_list_elem *elem = &b->list_stack[b->list_sp];
    if (elem->count != 0) {
        int32_t sep_len = elem->separator_len;
        if (sep_len == 1) {
            flecs_strbuf_appendch(b, elem->separator[0]);
        } else if (sep_len != 0) {
            flecs_strbuf_appendstr(b, elem->separator, sep_len);
        }
    }
    elem->count ++;
}

void ecs_strbuf_list_appendch(
    ecs_strbuf_t *b,
    char ch)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_list_next(b);
    flecs_strbuf_appendch(b, ch);
}

void ecs_strbuf_list_append(
    ecs_strbuf_t *b,
    const char *fmt,
    ...)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(fmt != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);

    va_list args;
    va_start(args, fmt);
    flecs_strbuf_vappend(b, fmt, args);
    va_end(args);
}

void ecs_strbuf_list_appendstr(
    ecs_strbuf_t *b,
    const char *str)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);
    ecs_strbuf_appendstr(b, str);
}

void ecs_strbuf_list_appendstrn(
    ecs_strbuf_t *b,
    const char *str,
    int32_t n)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(str != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_list_next(b);
    ecs_strbuf_appendstrn(b, str, n);
}

int32_t ecs_strbuf_written(
    const ecs_strbuf_t *b)
{
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);
    return b->length;
}
