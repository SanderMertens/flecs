/**
 * @file misc.c
 * @brief Miscellaneous functions.
 */

#include "private_api.h"
#include <time.h>
#include <ctype.h>

#ifndef FLECS_NDEBUG
static int64_t flecs_s_min[] = { 
    [1] = INT8_MIN, [2] = INT16_MIN, [4] = INT32_MIN, [8] = INT64_MIN };
static int64_t flecs_s_max[] = { 
    [1] = INT8_MAX, [2] = INT16_MAX, [4] = INT32_MAX, [8] = INT64_MAX };
static uint64_t flecs_u_max[] = { 
    [1] = UINT8_MAX, [2] = UINT16_MAX, [4] = UINT32_MAX, [8] = UINT64_MAX };

uint64_t flecs_ito_(
    size_t size,
    bool is_signed,
    bool lt_zero,
    uint64_t u,
    const char *err)
{
    union {
        uint64_t u;
        int64_t s;
    } v;

    v.u = u;

    if (is_signed) {
        ecs_assert(v.s >= flecs_s_min[size], ECS_INVALID_CONVERSION, err);
        ecs_assert(v.s <= flecs_s_max[size], ECS_INVALID_CONVERSION, err);
    } else {
        ecs_assert(lt_zero == false, ECS_INVALID_CONVERSION, err);
        ecs_assert(u <= flecs_u_max[size], ECS_INVALID_CONVERSION, err);
    }

    return u;
}
#endif

int32_t flecs_next_pow_of_2(
    int32_t n)
{
    n --;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n ++;

    return n;
}

/** Convert time to double */
double ecs_time_to_double(
    ecs_time_t t)
{
    double result;
    result = t.sec;
    return result + (double)t.nanosec / (double)1000000000;
}

ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2)
{
    ecs_time_t result;

    if (t1.nanosec >= t2.nanosec) {
        result.nanosec = t1.nanosec - t2.nanosec;
        result.sec = t1.sec - t2.sec;
    } else {
        result.nanosec = t1.nanosec - t2.nanosec + 1000000000;
        result.sec = t1.sec - t2.sec - 1;
    }

    return result;
}

void ecs_sleepf(
    double t)
{
    if (t > 0) {
        int sec = (int)t;
        int nsec = (int)((t - sec) * 1000000000);
        ecs_os_sleep(sec, nsec);
    }
}

double ecs_time_measure(
    ecs_time_t *start)
{
    ecs_time_t stop, temp;
    ecs_os_get_time(&stop);
    temp = stop;
    stop = ecs_time_sub(stop, *start);
    *start = temp;
    return ecs_time_to_double(stop);
}

void* ecs_os_memdup(
    const void *src, 
    ecs_size_t size) 
{
    if (!src) {
        return NULL;
    }
        
    void *dst = ecs_os_malloc(size);
    ecs_assert(dst != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_os_memcpy(dst, src, size);  
    return dst;  
}

int flecs_entity_compare(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2) 
{
    (void)ptr1;
    (void)ptr2;
    return (e1 > e2) - (e1 < e2);
}

int flecs_id_qsort_cmp(const void *a, const void *b) {
    ecs_id_t id_a = *(const ecs_id_t*)a;
    ecs_id_t id_b = *(const ecs_id_t*)b;
    return (id_a > id_b) - (id_a < id_b);
}

uint64_t flecs_string_hash(
    const void *ptr)
{
    const ecs_hashed_string_t *str = ptr;
    ecs_assert(str->hash != 0, ECS_INTERNAL_ERROR, NULL);
    return str->hash;
}

char* flecs_vasprintf(
    const char *fmt,
    va_list args)
{
    ecs_size_t size = 0;
    char *result  = NULL;
    va_list tmpa;

    va_copy(tmpa, args);

    size = vsnprintf(result, 0, fmt, tmpa);

    va_end(tmpa);

    if ((int32_t)size < 0) { 
        return NULL; 
    }

    result = (char *) ecs_os_malloc(size + 1);

    if (!result) { 
        return NULL; 
    }

    ecs_os_vsnprintf(result, size + 1, fmt, args);

    return result;
}

char* flecs_asprintf(
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *result = flecs_vasprintf(fmt, args);
    va_end(args);
    return result;
}

char* flecs_to_snake_case(const char *str) {
    int32_t upper_count = 0, len = 1;
    const char *ptr = str;
    char ch, *out, *out_ptr;

    for (ptr = &str[1]; (ch = *ptr); ptr ++) {
        if (isupper(ch)) {
            upper_count ++;
        }
        len ++;
    }

    out = out_ptr = ecs_os_malloc_n(char, len + upper_count + 1);
    for (ptr = str; (ch = *ptr); ptr ++) {
        if (isupper(ch)) {
            if ((ptr != str) && (out_ptr[-1] != '_')) {
                out_ptr[0] = '_';
                out_ptr ++;
            }
            out_ptr[0] = (char)tolower(ch);
            out_ptr ++;
        } else {
            out_ptr[0] = ch;
            out_ptr ++;
        }
    }

    out_ptr[0] = '\0';

    return out;
}

char* flecs_load_from_file(
    const char *filename)
{
    FILE* file;
    char* content = NULL;
    int32_t bytes;
    size_t size;

    /* Open file for reading */
    ecs_os_fopen(&file, filename, "r");
    if (!file) {
        ecs_err("%s (%s)", ecs_os_strerror(errno), filename);
        goto error;
    }

    /* Determine file size */
    fseek(file, 0, SEEK_END);
    bytes = (int32_t)ftell(file);
    if (bytes == -1) {
        goto error;
    }
    fseek(file, 0, SEEK_SET);

    /* Load contents in memory */
    content = ecs_os_malloc(bytes + 1);
    size = (size_t)bytes;
    if (!(size = fread(content, 1, size, file)) && bytes) {
        ecs_err("%s: read zero bytes instead of %d", filename, size);
        ecs_os_free(content);
        content = NULL;
        goto error;
    } else {
        content[size] = '\0';
    }

    fclose(file);

    return content;
error:
    ecs_os_free(content);
    return NULL;
}

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

const char* flecs_chrparse(
    const char *in, 
    char *out) 
{
    const char *result = in + 1;
    char ch;

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

const char* flecs_parse_digit(
    const char *ptr,
    char *token)
{
    char *tptr = token;
    char ch = ptr[0];

    if (!isdigit(ch) && ch != '-') {
        ecs_parser_error(NULL, NULL, 0, "invalid start of number '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!isdigit(ch) && (ch != '.') && (ch != 'e')) {
            break;
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
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}
