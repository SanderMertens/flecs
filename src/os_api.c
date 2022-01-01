#include "private_api.h"

void ecs_os_api_impl(ecs_os_api_t *api);

static bool ecs_os_api_initialized = false;
static int ecs_os_api_init_count = 0;

#ifndef __EMSCRIPTEN__
ecs_os_api_t ecs_os_api = {
    .log_with_color_ = true,
    .log_level_ = -1 /* disable tracing by default, but enable >= warnings */
};
#else
/* Disable colors by default for emscripten */
ecs_os_api_t ecs_os_api = {
    .log_level_ = -1
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

#if !defined(_MSC_VER) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !defined(_WIN32)
#include <execinfo.h>
#define ECS_BT_BUF_SIZE 100
static
void dump_backtrace(
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

    for (int j = 3; j < nptrs; j++) {
        fprintf(stream, "%s\n", strings[j]);
    }

    free(strings);
}
#else
static
void dump_backtrace(
    FILE *stream)
{ 
    (void)stream;
}
#endif

static
void log_msg(
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

    if (level >= 0) {
        if (ecs_os_api.log_with_color_) fputs(ECS_MAGENTA, stream);
        fputs("info", stream);
    } else if (level == -2) {
        if (ecs_os_api.log_with_color_) fputs(ECS_YELLOW, stream);
        fputs("warning", stream);
    } else if (level == -3) {
        if (ecs_os_api.log_with_color_) fputs(ECS_RED, stream);
        fputs("error", stream);
    } else if (level == -4) {
        if (ecs_os_api.log_with_color_) fputs(ECS_RED, stream);
        fputs("fatal", stream);
    }

    if (ecs_os_api.log_with_color_) fputs(ECS_NORMAL, stream);
    fputs(": ", stream);

    if (level >= 0) {
        if (ecs_os_api.log_indent_) {
            char indent[32];
            int i;
            for (i = 0; i < ecs_os_api.log_indent_; i ++) {
                indent[i * 2] = '|';
                indent[i * 2 + 1] = ' ';
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
        dump_backtrace(stream);
    }
}

void ecs_os_dbg(
    const char *file, 
    int32_t line, 
    const char *msg)
{
#ifndef NDEBUG
    if (ecs_os_api.log_) {
        ecs_os_api.log_(1, file, line, msg);
    }
#else
    (void)file;
    (void)line;
    (void)msg;
#endif
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
void ecs_os_gettime(ecs_time_t *time)
{
    uint64_t now = flecs_os_time_now();
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

    flecs_os_time_setup();
    
    /* Memory management */
    ecs_os_api.malloc_ = ecs_os_api_malloc;
    ecs_os_api.free_ = ecs_os_api_free;
    ecs_os_api.realloc_ = ecs_os_api_realloc;
    ecs_os_api.calloc_ = ecs_os_api_calloc;

    /* Strings */
    ecs_os_api.strdup_ = ecs_os_api_strdup;

    /* Time */
    ecs_os_api.sleep_ = flecs_os_time_sleep;
    ecs_os_api.get_time_ = ecs_os_gettime;

    /* Logging */
    ecs_os_api.log_ = log_msg;

    /* Modules */
    if (!ecs_os_api.module_to_dl_) {
        ecs_os_api.module_to_dl_ = ecs_os_api_module_to_dl;
    }

    if (!ecs_os_api.module_to_etc_) {
        ecs_os_api.module_to_etc_ = ecs_os_api_module_to_etc;
    }

    ecs_os_api.abort_ = abort;
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
        (ecs_os_api.thread_join_ != NULL);   
}

bool ecs_os_has_time(void) {
    return 
        (ecs_os_api.get_time_ != NULL) &&
        (ecs_os_api.sleep_ != NULL);
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

#if defined(_MSC_VER)
static char error_str[255];
#endif

const char* ecs_os_strerror(int err) {
#if defined(_MSC_VER)
    strerror_s(error_str, 255, err);
    return error_str;
#else
    return strerror(err);
#endif
}
