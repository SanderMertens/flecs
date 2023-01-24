/**
 * @file os_api.h
 * @brief Operating system abstraction API.
 * 
 * The OS API implements an overridable interface for implementing functions 
 * that are operating system specific, in addition to a number of hooks which
 * allow for customization by the user, like logging.
 */

#include "private_api.h"
#include <ctype.h>
#include <time.h>

void ecs_os_api_impl(ecs_os_api_t *api);

static bool ecs_os_api_initialized = false;
static bool ecs_os_api_initializing = false;
static int ecs_os_api_init_count = 0;

#ifndef __EMSCRIPTEN__
ecs_os_api_t ecs_os_api = {
    .flags_ = EcsOsApiHighResolutionTimer | EcsOsApiLogWithColors,
    .log_level_ = -1 /* Disable tracing by default, but log warnings/errors */
};
#else
/* Disable colors by default for emscripten */
ecs_os_api_t ecs_os_api = {
    .flags_ = EcsOsApiHighResolutionTimer,
    .log_level_ = -1 /* Disable tracing by default, but log warnings/errors */
};
#endif

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

ecs_os_api_t ecs_os_get_api(void) {
    return ecs_os_api;
}

void ecs_os_init(void)
{
    if (!ecs_os_api_initialized) {
        ecs_os_set_api_defaults();
    }
    
    if (!(ecs_os_api_init_count ++)) {
        if (ecs_os_api.init_) {
            ecs_os_api.init_();
        }
    }
}

void ecs_os_fini(void) {
    if (!--ecs_os_api_init_count) {
        if (ecs_os_api.fini_) {
            ecs_os_api.fini_();
        }
    }
}

/* Assume every non-glibc Linux target has no execinfo.
   This mainly fixes musl support, as musl doesn't define any preprocessor macro specifying its presence. */ 
#if defined(ECS_TARGET_LINUX) && !defined(__GLIBC__)
#define HAVE_EXECINFO 0
#elif !defined(ECS_TARGET_WINDOWS) && !defined(ECS_TARGET_EM) && !defined(ECS_TARGET_ANDROID)
#define HAVE_EXECINFO 1
#else
#define HAVE_EXECINFO 0
#endif

#if HAVE_EXECINFO
#include <execinfo.h>
#define ECS_BT_BUF_SIZE 100

void flecs_dump_backtrace(
    FILE *stream) 
{
    int nptrs;
    void *buffer[ECS_BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, ECS_BT_BUF_SIZE);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        return;
    }

    for (int j = 1; j < nptrs; j++) {
        fprintf(stream, "%s\n", strings[j]);
    }

    free(strings);
}
#else
void flecs_dump_backtrace(
    FILE *stream)
{ 
    (void)stream;
}
#endif
#undef HAVE_EXECINFO_H

static
void flecs_log_msg(
    int32_t level,
    const char *file, 
    int32_t line,  
    const char *msg)
{
    FILE *stream;
    if (level >= 0) {
        stream = stdout;
    } else {
        stream = stderr;
    }

    bool use_colors = ecs_os_api.flags_ & EcsOsApiLogWithColors;
    bool timestamp = ecs_os_api.flags_ & EcsOsApiLogWithTimeStamp;
    bool deltatime = ecs_os_api.flags_ & EcsOsApiLogWithTimeDelta;

    time_t now = 0;

    if (deltatime) {
        now = time(NULL);
        time_t delta = 0;
        if (ecs_os_api.log_last_timestamp_) {
            delta = now - ecs_os_api.log_last_timestamp_;
        }
        ecs_os_api.log_last_timestamp_ = (int64_t)now;

        if (delta) {
            if (delta < 10) {
                fputs(" ", stream);
            }
            if (delta < 100) {
                fputs(" ", stream);
            }
            char time_buf[20];
            ecs_os_sprintf(time_buf, "%u", (uint32_t)delta);
            fputs("+", stream);
            fputs(time_buf, stream);
            fputs(" ", stream);
        } else {
            fputs("     ", stream);
        }
    }

    if (timestamp) {
        if (!now) {
            now = time(NULL);
        }
        char time_buf[20];
        ecs_os_sprintf(time_buf, "%u", (uint32_t)now);
        fputs(time_buf, stream);
        fputs(" ", stream);
    }

    if (level >= 4) {
        if (use_colors) fputs(ECS_NORMAL, stream);
        fputs("jrnl", stream);
    } else if (level >= 0) {
        if (level == 0) {
            if (use_colors) fputs(ECS_MAGENTA, stream);
        } else {
            if (use_colors) fputs(ECS_GREY, stream);
        }
        fputs("info", stream);
    } else if (level == -2) {
        if (use_colors) fputs(ECS_YELLOW, stream);
        fputs("warning", stream);
    } else if (level == -3) {
        if (use_colors) fputs(ECS_RED, stream);
        fputs("error", stream);
    } else if (level == -4) {
        if (use_colors) fputs(ECS_RED, stream);
        fputs("fatal", stream);
    }

    if (use_colors) fputs(ECS_NORMAL, stream);
    fputs(": ", stream);

    if (level >= 0) {
        if (ecs_os_api.log_indent_) {
            char indent[32];
            int i, indent_count = ecs_os_api.log_indent_;
            if (indent_count > 15) indent_count = 15;

            for (i = 0; i < indent_count; i ++) {
                indent[i * 2] = '|';
                indent[i * 2 + 1] = ' ';
            }

            if (ecs_os_api.log_indent_ != indent_count) {
                indent[i * 2 - 2] = '+';
            }

            indent[i * 2] = '\0';

            fputs(indent, stream);
        }
    }

    if (level < 0) {
        if (file) {
            const char *file_ptr = strrchr(file, '/');
            if (!file_ptr) {
                file_ptr = strrchr(file, '\\');
            }

            if (file_ptr) {
                file = file_ptr + 1;
            }

            fputs(file, stream);
            fputs(": ", stream);
        }

        if (line) {
            fprintf(stream, "%d: ", line);
        }
    }

    fputs(msg, stream);

    fputs("\n", stream);

    if (level == -4) {
        flecs_dump_backtrace(stream);
    }
}

void ecs_os_dbg(
    const char *file, 
    int32_t line, 
    const char *msg)
{
    if (ecs_os_api.log_) {
        ecs_os_api.log_(1, file, line, msg);
    }
}

void ecs_os_trace(
    const char *file, 
    int32_t line, 
    const char *msg) 
{
    if (ecs_os_api.log_) {
        ecs_os_api.log_(0, file, line, msg);
    }
}

void ecs_os_warn(
    const char *file, 
    int32_t line, 
    const char *msg) 
{
    if (ecs_os_api.log_) {
        ecs_os_api.log_(-2, file, line, msg);
    }
}

void ecs_os_err(
    const char *file, 
    int32_t line, 
    const char *msg) 
{
    if (ecs_os_api.log_) {
        ecs_os_api.log_(-3, file, line, msg);
    }
}

void ecs_os_fatal(
    const char *file, 
    int32_t line, 
    const char *msg) 
{
    if (ecs_os_api.log_) {
        ecs_os_api.log_(-4, file, line, msg);
    }
}

static
void ecs_os_gettime(ecs_time_t *time) {
    ecs_assert(ecs_os_has_time() == true, ECS_MISSING_OS_API, NULL);
    
    uint64_t now = ecs_os_now();
    uint64_t sec = now / 1000000000;

    assert(sec < UINT32_MAX);
    assert((now - sec * 1000000000) < UINT32_MAX);

    time->sec = (uint32_t)sec;
    time->nanosec = (uint32_t)(now - sec * 1000000000);
}

static
void* ecs_os_api_malloc(ecs_size_t size) {
    ecs_os_linc(&ecs_os_api_malloc_count);
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return malloc((size_t)size);
}

static
void* ecs_os_api_calloc(ecs_size_t size) {
    ecs_os_linc(&ecs_os_api_calloc_count);
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return calloc(1, (size_t)size);
}

static
void* ecs_os_api_realloc(void *ptr, ecs_size_t size) {
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_os_linc(&ecs_os_api_realloc_count);
    } else {
        /* If not actually reallocing, treat as malloc */
        ecs_os_linc(&ecs_os_api_malloc_count);
    }
    
    return realloc(ptr, (size_t)size);
}

static
void ecs_os_api_free(void *ptr) {
    if (ptr) {
        ecs_os_linc(&ecs_os_api_free_count);
    }
    free(ptr);
}

static
char* ecs_os_api_strdup(const char *str) {
    if (str) {
        int len = ecs_os_strlen(str);
        char *result = ecs_os_malloc(len + 1);
        ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
        ecs_os_strcpy(result, str);
        return result;
    } else {
        return NULL;
    }
}

void ecs_os_strset(char **str, const char *value) {
    char *old = str[0];
    str[0] = ecs_os_strdup(value);
    ecs_os_free(old);
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

#   if defined(ECS_TARGET_LINUX) || defined(ECS_TARGET_FREEBSD)
    ecs_strbuf_appendlit(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendlit(&lib, ".so");
#   elif defined(ECS_TARGET_DARWIN)
    ecs_strbuf_appendlit(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendlit(&lib, ".dylib");
#   elif defined(ECS_TARGET_WINDOWS)
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendlit(&lib, ".dll");
#   endif

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

static
char* ecs_os_api_module_to_etc(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with dashes + /etc */
    char *file_base = module_file_base(module, '-');

    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendlit(&lib, "/etc");

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized != 0) {
        return;
    }

    if (ecs_os_api_initializing != 0) {
        return;
    }

    ecs_os_api_initializing = true;
    
    /* Memory management */
    ecs_os_api.malloc_ = ecs_os_api_malloc;
    ecs_os_api.free_ = ecs_os_api_free;
    ecs_os_api.realloc_ = ecs_os_api_realloc;
    ecs_os_api.calloc_ = ecs_os_api_calloc;

    /* Strings */
    ecs_os_api.strdup_ = ecs_os_api_strdup;

    /* Time */
    ecs_os_api.get_time_ = ecs_os_gettime;

    /* Logging */
    ecs_os_api.log_ = flecs_log_msg;

    /* Modules */
    if (!ecs_os_api.module_to_dl_) {
        ecs_os_api.module_to_dl_ = ecs_os_api_module_to_dl;
    }

    if (!ecs_os_api.module_to_etc_) {
        ecs_os_api.module_to_etc_ = ecs_os_api_module_to_etc;
    }

    ecs_os_api.abort_ = abort;

#   ifdef FLECS_OS_API_IMPL
    /* Initialize defaults to OS API IMPL addon, but still allow for overriding
     * by the application */
    ecs_set_os_api_impl();
    ecs_os_api_initialized = false;
#   endif

    ecs_os_api_initializing = false;
}

bool ecs_os_has_heap(void) {
    return 
        (ecs_os_api.malloc_ != NULL) &&
        (ecs_os_api.calloc_ != NULL) &&
        (ecs_os_api.realloc_ != NULL) &&
        (ecs_os_api.free_ != NULL);
}

bool ecs_os_has_threading(void) {
    return
        (ecs_os_api.mutex_new_ != NULL) &&
        (ecs_os_api.mutex_free_ != NULL) &&
        (ecs_os_api.mutex_lock_ != NULL) &&
        (ecs_os_api.mutex_unlock_ != NULL) &&
        (ecs_os_api.cond_new_ != NULL) &&
        (ecs_os_api.cond_free_ != NULL) &&
        (ecs_os_api.cond_wait_ != NULL) &&
        (ecs_os_api.cond_signal_ != NULL) &&
        (ecs_os_api.cond_broadcast_ != NULL) &&
        (ecs_os_api.thread_new_ != NULL) &&
        (ecs_os_api.thread_join_ != NULL) &&
        (ecs_os_api.thread_self_ != NULL);
}

bool ecs_os_has_time(void) {
    return 
        (ecs_os_api.get_time_ != NULL) &&
        (ecs_os_api.sleep_ != NULL) &&
        (ecs_os_api.now_ != NULL);
}

bool ecs_os_has_logging(void) {
    return (ecs_os_api.log_ != NULL);
}

bool ecs_os_has_dl(void) {
    return 
        (ecs_os_api.dlopen_ != NULL) &&
        (ecs_os_api.dlproc_ != NULL) &&
        (ecs_os_api.dlclose_ != NULL);  
}

bool ecs_os_has_modules(void) {
    return 
        (ecs_os_api.module_to_dl_ != NULL) &&
        (ecs_os_api.module_to_etc_ != NULL);
}

#if defined(ECS_TARGET_WINDOWS)
static char error_str[255];
#endif

const char* ecs_os_strerror(int err) {
#   if defined(ECS_TARGET_WINDOWS)
    strerror_s(error_str, 255, err);
    return error_str;
#   else
    return strerror(err);
#   endif
}
