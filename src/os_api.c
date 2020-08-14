#include "private_api.h"

void ecs_os_api_impl(ecs_os_api_t *api);

static bool ecs_os_api_initialized = false;
static int ecs_os_api_init_count = 0;

ecs_os_api_t ecs_os_api;

int64_t ecs_os_api_malloc_count = 0;
int64_t ecs_os_api_realloc_count = 0;
int64_t ecs_os_api_calloc_count = 0;
int64_t ecs_os_api_free_count = 0;

void ecs_os_set_api(
    ecs_os_api_t *os_api)
{
    if (!ecs_os_api_initialized) {
        ecs_os_api = *os_api;
        ecs_os_api_initialized = true;
    }
}

void ecs_os_init(void)
{
    if (!ecs_os_api_initialized) {
        ecs_os_set_api_defaults();
    }
    
    if (!(ecs_os_api_init_count ++)) {
        if (ecs_os_api.init) {
            ecs_os_api.init();
        }
    }
}

void ecs_os_fini(void) {
    if (!--ecs_os_api_init_count) {
        if (ecs_os_api.fini) {
            ecs_os_api.fini();
        }
    }
}

static
void ecs_log(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_error(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

static
void ecs_log_debug(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_warning(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void ecs_os_dbg(const char *fmt, ...) {
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api._log_debug) {
        ecs_os_api._log_debug(fmt, args);
    }
    va_end(args);
#else
    (void)fmt;
#endif
}

void ecs_os_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api._log_warning) {
        ecs_os_api._log_warning(fmt, args);
    }
    va_end(args);
}

void ecs_os_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api._log) {
        ecs_os_api._log(fmt, args);
    }
    va_end(args);
}

void ecs_os_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api._log_error) {
        ecs_os_api._log_error(fmt, args);
    }
    va_end(args);
}

void ecs_os_gettime(ecs_time_t *time)
{
    uint64_t now = ecs_os_time_now();
    uint64_t sec = now / 1000000000;

    assert(sec < UINT32_MAX);
    assert((now - sec * 1000000000) < UINT32_MAX);

    time->sec = (uint32_t)sec;
    time->nanosec = (uint32_t)(now - sec * 1000000000);
}

static
void* ecs_os_api_malloc(ecs_size_t size) {
    ecs_os_api_malloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return malloc((size_t)size);
}

static
void* ecs_os_api_calloc(ecs_size_t size) {
    ecs_os_api_calloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return calloc(1, (size_t)size);
}

static
void* ecs_os_api_realloc(void *ptr, ecs_size_t size) {
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_os_api_realloc_count ++;
    } else {
        /* If not actually reallocing, treat as malloc */
        ecs_os_api_malloc_count ++; 
    }
    
    return realloc(ptr, (size_t)size);
}

static
void ecs_os_api_free(void *ptr) {
    if (ptr) {
        ecs_os_api_free_count ++;
    }
    free(ptr);
}

static
char* ecs_os_api_strdup(const char *str) {
    int len = ecs_os_strlen(str);
    char *result = ecs_os_api_malloc(len + 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_os_strcpy(result, str);
    return result;
}

/* Replace dots with underscores */
static
char *module_file_base(const char *module, char sep) {
    char *base = ecs_os_strdup(module);
    ecs_size_t i, len = ecs_os_strlen(base);
    for (i = 0; i < len; i ++) {
        if (base[i] == '.') {
            base[i] = sep;
        }
    }

    return base;
}

static
char* ecs_os_api_module_to_dl(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with underscores + OS library extension */
    char *file_base = module_file_base(module, '_');

#if defined(ECS_OS_LINUX)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".so");
#elif defined(ECS_OS_DARWIN)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dylib");
#elif defined(ECS_OS_WINDOWS)
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dll");
#endif

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

static
char* ecs_os_api_module_to_etc(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with dashes + /etc */
    char *file_base = module_file_base(module, '-');

    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, "/etc");

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized != 0) {
        return;
    }

    ecs_os_time_setup();
    
    /* Memory management */
    ecs_os_api.malloc = ecs_os_api_malloc;
    ecs_os_api.free = ecs_os_api_free;
    ecs_os_api.realloc = ecs_os_api_realloc;
    ecs_os_api.calloc = ecs_os_api_calloc;

    /* Strings */
    ecs_os_api.strdup = ecs_os_api_strdup;

    /* Time */
    ecs_os_api.sleep = ecs_os_time_sleep;
    ecs_os_api.get_time = ecs_os_gettime;

    /* Logging */
    ecs_os_api._log = ecs_log;
    ecs_os_api._log_error = ecs_log_error;
    ecs_os_api._log_debug = ecs_log_debug;
    ecs_os_api._log_warning = ecs_log_warning;

    /* Modules */
    if (!ecs_os_api.module_to_dl) {
        ecs_os_api.module_to_dl = ecs_os_api_module_to_dl;
    }

    if (!ecs_os_api.module_to_etc) {
        ecs_os_api.module_to_etc = ecs_os_api_module_to_etc;
    }

    ecs_os_api.abort = abort;
}
