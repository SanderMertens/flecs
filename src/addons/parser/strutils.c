/**
 * @file addons/parser/strutils.c
 * @brief String utilities for parser addons.
 */

#include "flecs.h"

#ifdef FLECS_PARSER

#include "parser.h"
#include "grammar.h"

char* flecs_chresc(
    char *out,
    char in,
    char delimiter)
{
    char *bptr = out;
    switch(in) {
    case '\a':
        *bptr++ = '\\';
        *bptr = 'a';
        break;
    case '\b':
        *bptr++ = '\\';
        *bptr = 'b';
        break;
    case '\f':
        *bptr++ = '\\';
        *bptr = 'f';
        break;
    case '\n':
        *bptr++ = '\\';
        *bptr = 'n';
        break;
    case '\r':
        *bptr++ = '\\';
        *bptr = 'r';
        break;
    case '\t':
        *bptr++ = '\\';
        *bptr = 't';
        break;
    case '\v':
        *bptr++ = '\\';
        *bptr = 'v';
        break;
    case '\\':
        *bptr++ = '\\';
        *bptr = '\\';
        break;
    case '\033':
        *bptr = '['; /* Used for terminal colors */
        break;
    default:
        if (in == delimiter) {
            *bptr++ = '\\';
            *bptr = delimiter;
        } else {
            *bptr = in;
        }

        break;
    }

    *(++bptr) = '\0';

    return bptr;
}

static const char* flecs_hex4parse(
    const char *in,
    uint32_t *out)
{
    uint32_t result = 0;
    int i;
    for (i = 0; i < 4; i ++) {
        char ch = in[i];
        result <<= 4;
        if (ch >= '0' && ch <= '9') {
            result |= (uint32_t)(ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            result |= (uint32_t)(ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            result |= (uint32_t)(ch - 'A' + 10);
        } else {
            return NULL;
        }
    }

    *out = result;
    return in + 4;
}

static const char* flecs_unicode_chrparse(
    const char *in,
    char *out,
    int32_t *out_len)
{
    uint32_t cp;
    in = flecs_hex4parse(in, &cp);
    if (!in) {
        return NULL;
    }

    if (cp >= 0xD800 && cp <= 0xDBFF) {
        uint32_t lo;
        if (in[0] != '\\' || in[1] != 'u') {
            return NULL;
        }

        in = flecs_hex4parse(in + 2, &lo);
        if (!in || lo < 0xDC00 || lo > 0xDFFF) {
            return NULL;
        }

        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
    } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
        return NULL;
    }

    char buf[4];
    int32_t len;
    if (cp < 0x80) {
        buf[0] = (char)cp;
        len = 1;
    } else if (cp < 0x800) {
        buf[0] = (char)(0xC0 | (cp >> 6));
        buf[1] = (char)(0x80 | (cp & 0x3F));
        len = 2;
    } else if (cp < 0x10000) {
        buf[0] = (char)(0xE0 | (cp >> 12));
        buf[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        buf[2] = (char)(0x80 | (cp & 0x3F));
        len = 3;
    } else {
        buf[0] = (char)(0xF0 | (cp >> 18));
        buf[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        buf[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
        buf[3] = (char)(0x80 | (cp & 0x3F));
        len = 4;
    }

    if (out) {
        ecs_os_memcpy(out, buf, len);
    }

    if (out_len) {
        *out_len = len;
    }

    return in;
}

const char* flecs_chrparse(
    const char *in,
    char *out,
    int32_t *out_len)
{
    const char *result = in + 1;
    char ch;

    if (in[0] == '\\' && in[1] == 'u') {
        return flecs_unicode_chrparse(in + 2, out, out_len);
    }

    if (in[0] == '\\') {
        result ++;

        switch(in[1]) {
        case 'a':
            ch = '\a';
            break;
        case 'b':
            ch = '\b';
            break;
        case 'f':
            ch = '\f';
            break;
        case 'n':
            ch = '\n';
            break;
        case 'r':
            ch = '\r';
            break;
        case 't':
            ch = '\t';
            break;
        case 'v':
            ch = '\v';
            break;
        case '\\':
            ch = '\\';
            break;
        case '"':
            ch = '"';
            break;
        case '`':
            ch = '`';
            break;
        case '0':
            ch = '\0';
            break;
        case ' ':
            ch = ' ';
            break;
        case '$':
            ch = '$';
            break;
        default:
            goto error;
        }
    } else {
        ch = in[0];
    }

    if (out) {
        *out = ch;
    }

    if (out_len) {
        *out_len = 1;
    }

    return result;
error:
    return NULL;
}

ecs_size_t flecs_stresc(
    char *out,
    ecs_size_t n,
    char delimiter,
    const char *in)
{
    const char *ptr = in;
    char ch, *bptr = out, buff[3];
    ecs_size_t written = 0;
    while ((ch = *ptr++)) {
        if ((written += (ecs_size_t)(flecs_chresc(
            buff, ch, delimiter) - buff)) <= n)
        {
            /* If size != 0, an out buffer must be provided. */
            ecs_check(out != NULL, ECS_INVALID_PARAMETER, NULL);
            *bptr++ = buff[0];
            if ((ch = buff[1])) {
                *bptr = ch;
                bptr++;
            }
        }
    }

    if (bptr) {
        while (written < n) {
            *bptr = '\0';
            bptr++;
            written++;
        }
    }

    return written;
error:
    return 0;
}

char* flecs_astresc(
    char delimiter,
    const char *in)
{
    if (!in) {
        return NULL;
    }

    ecs_size_t len = flecs_stresc(NULL, 0, delimiter, in);
    char *out = ecs_os_malloc_n(char, len + 1);
    flecs_stresc(out, len, delimiter, in);
    out[len] = '\0';
    return out;
}

static bool flecs_parse_is_e(
    char e)
{
    return e == 'e' || e == 'E';
}

const char* flecs_parse_digit(
    const char *ptr,
    char *token,
    int32_t token_size)
{
    char *tptr = token;
    char ch = ptr[0];

    if (!isdigit((unsigned char)ch) && ch != '-') {
        ecs_parser_error(NULL, NULL, 0, "invalid start of number '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!isdigit((unsigned char)ch) && (ch != '.') && !flecs_parse_is_e(ch)) {
            if (ch != '-' || !flecs_parse_is_e(ptr[-1])) {
                break;
            }
        }

        if ((tptr - token) >= (token_size - 1)) {
            ecs_parser_error(NULL, NULL, 0, "number too long");
            return NULL;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    return ptr;
}

const char* flecs_parse_ws_eol(
    const char *ptr)
{
    while (isspace((unsigned char)*ptr)) {
        ptr ++;
    }

    return ptr;
}

#endif
