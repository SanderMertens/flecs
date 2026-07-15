/**
 * @file misc.c
 * @brief Miscellaneous functions.
 */
 
#include <time.h>
#include <errno.h>

#include "private_api.h"

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
        ecs_assert(v.s >= flecs_s_min[size], ECS_INVALID_CONVERSION, "%s", err);
        ecs_assert(v.s <= flecs_s_max[size], ECS_INVALID_CONVERSION, "%s", err);
    } else {
        ecs_assert(lt_zero == false, ECS_INVALID_CONVERSION, "%s", err);
        ecs_assert(u <= flecs_u_max[size], ECS_INVALID_CONVERSION, "%s", err);
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
    FILE* file = ecs_os_fopen(filename, "r");
    if (!file) {
        ecs_err("%s (%s)", ecs_os_strerror(errno), filename);
        return NULL;
    }

    int32_t size = 0;
    int32_t cap = 4096;
    char *content = ecs_os_malloc(cap + 1);
    size_t read;

    while ((read = ecs_os_fread(
        &content[size], 1, flecs_itosize(cap - size), file)))
    {
        size += flecs_uto(int32_t, read);
        if (size == cap) {
            cap *= 2;
            content = ecs_os_realloc(content, cap + 1);
        }
    }

    content[size] = '\0';
    ecs_os_fclose(file);

    return content;
}

#define FLECS_ERRSTR_MAX (256)
static char flecs_errstr_buf[FLECS_ERRSTR_MAX];
static char flecs_errstr_buf_1[FLECS_ERRSTR_MAX];
static char flecs_errstr_buf_2[FLECS_ERRSTR_MAX];
static char flecs_errstr_buf_3[FLECS_ERRSTR_MAX];
static char flecs_errstr_buf_4[FLECS_ERRSTR_MAX];
static char flecs_errstr_buf_5[FLECS_ERRSTR_MAX];

const char* flecs_errstr(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf;
}

const char* flecs_errstr_1(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf_1, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf_1;
}

const char* flecs_errstr_2(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf_2, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf_2;
}

const char* flecs_errstr_3(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf_3, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf_3;
}

const char* flecs_errstr_4(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf_4, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf_4;
}

const char* flecs_errstr_5(
    char *str)
{
    ecs_os_strncpy(flecs_errstr_buf_5, str, FLECS_ERRSTR_MAX - 1);
    ecs_os_free(str);
    return flecs_errstr_buf_5;
}
