/**
 * @file addons/log.c
 * @brief Log addon.
 */

#include "../private_api.h"

#ifdef FLECS_LOG

static char *flecs_log_last_err = NULL;
static ecs_os_api_log_t flecs_log_prev_log = NULL;
static ecs_os_api_log_t flecs_log_prev_fatal_log = NULL;
static bool flecs_log_prev_color = false;
static int flecs_log_prev_level = 0;

static
void flecs_set_prev_log(
    ecs_os_api_log_t prev_log,
    bool try)
{
    flecs_log_prev_log = try ? NULL : prev_log;
    flecs_log_prev_fatal_log = prev_log;
}

static 
void flecs_log_capture_log(
    int32_t level, 
    const char *file,
    int32_t line, 
    const char *msg)
{
    (void)file; (void)line;

    if (level <= -4) {
        /* Make sure to always log fatal errors */
        if (flecs_log_prev_fatal_log) {
            ecs_log_enable_colors(true);
            flecs_log_prev_fatal_log(level, file, line, msg);
            ecs_log_enable_colors(false);
            return;
        } else {
            fprintf(stderr, "%s:%d: %s", file, line, msg);
        }
    }

#ifdef FLECS_DEBUG
    /* In debug mode, log unexpected errors to the console */
    if (level < 0) {
        /* Also log to previous log function in debug mode */
        if (flecs_log_prev_log) {
            ecs_log_enable_colors(true);
            flecs_log_prev_log(level, file, line, msg);
            ecs_log_enable_colors(false);
        }
    }
#endif

    if (!flecs_log_last_err && level <= -3) {
        flecs_log_last_err = ecs_os_strdup(msg);
    }
}

static
char* flecs_log_get_captured_log(void) {
    char *result = flecs_log_last_err;
    flecs_log_last_err = NULL;
    return result;
}

void ecs_log_start_capture(bool try) {
    flecs_log_prev_color = ecs_log_enable_colors(false);
    flecs_log_prev_log = ecs_os_api.log_;
    flecs_log_prev_level = ecs_os_api.log_level_;
    flecs_set_prev_log(ecs_os_api.log_, try);
    ecs_os_api.log_ = flecs_log_capture_log;
    ecs_os_api.log_level_ = -1; /* Ignore debug tracing, log warnings/errors */
}

char* ecs_log_stop_capture(void) {
    ecs_os_api.log_ = flecs_log_prev_fatal_log;
    ecs_os_api.log_level_ = flecs_log_prev_level;
    ecs_log_enable_colors(flecs_log_prev_color);
    return flecs_log_get_captured_log();
}

void flecs_colorize_buf(
    char *msg,
    bool enable_colors,
    ecs_strbuf_t *buf)
{
    ecs_assert(msg != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(buf != NULL, ECS_INTERNAL_ERROR, NULL);

    char *ptr, ch, prev = '\0';
    bool isNum = false;
    char isStr = '\0';
    bool isVar = false;
    bool overrideColor = false;
    bool autoColor = true;
    bool dontAppend = false;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dontAppend = false;

        if (!overrideColor) {
            if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr &&
                !isalpha(prev) && (prev != '\\'))
            {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) ||
                (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar &&
                 !isalpha(prev) && !isdigit(prev) && (prev != '_') &&
                 (prev != '.'))
            {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_CYAN);
                isVar = true;
            }
        }

        if (!isVar && !isStr && !isNum && ch == '#' && ptr[1] == '[') {
            bool isColor = true;
            overrideColor = true;

            /* Custom colors */
            if (!ecs_os_strncmp(&ptr[2], "]", ecs_os_strlen("]"))) {
                autoColor = false;
            } else if (!ecs_os_strncmp(&ptr[2], "green]", ecs_os_strlen("green]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_GREEN);
            } else if (!ecs_os_strncmp(&ptr[2], "red]", ecs_os_strlen("red]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_RED);
            } else if (!ecs_os_strncmp(&ptr[2], "blue]", ecs_os_strlen("blue]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_BLUE);
            } else if (!ecs_os_strncmp(&ptr[2], "magenta]", ecs_os_strlen("magenta]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_MAGENTA);
            } else if (!ecs_os_strncmp(&ptr[2], "cyan]", ecs_os_strlen("cyan]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_CYAN);
            } else if (!ecs_os_strncmp(&ptr[2], "yellow]", ecs_os_strlen("yellow]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_YELLOW);
            } else if (!ecs_os_strncmp(&ptr[2], "grey]", ecs_os_strlen("grey]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_GREY);
            } else if (!ecs_os_strncmp(&ptr[2], "white]", ecs_os_strlen("white]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "bold]", ecs_os_strlen("bold]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_BOLD);
            } else if (!ecs_os_strncmp(&ptr[2], "normal]", ecs_os_strlen("normal]"))) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "reset]", ecs_os_strlen("reset]"))) {
                overrideColor = false;
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
            } else {
                isColor = false;
                overrideColor = false;
            }

            if (isColor) {
                ptr += 2;
                while ((ch = *ptr) != ']') ptr ++;
                dontAppend = true;
            }
            if (!autoColor) {
                overrideColor = true;
            }
        }

        if (ch == '\n') {
            if (isNum || isStr || isVar || overrideColor) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
                overrideColor = false;
                isNum = false;
                isStr = false;
                isVar = false;
            }
        }

        if (!dontAppend) {
            ecs_strbuf_appendstrn(buf, ptr, 1);
        }

        if (!overrideColor) {
            if (((ch == '\'') || (ch == '"')) && !isStr) {
                if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (enable_colors) ecs_strbuf_appendlit(buf, ECS_NORMAL);
    }
}

void ecs_printv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args)
{
    (void)level;
    (void)line;

    ecs_strbuf_t msg_buf = ECS_STRBUF_INIT;

    /* Apply color. Even if we don't want color, we still need to call the
     * colorize function to get rid of the color tags (e.g. #[green]) */
    char *msg_nocolor = flecs_vasprintf(fmt, args);
    flecs_colorize_buf(msg_nocolor, 
        ecs_os_api.flags_ & EcsOsApiLogWithColors, &msg_buf);
    ecs_os_free(msg_nocolor);

    char *msg = ecs_strbuf_get(&msg_buf);

    if (msg) {
        ecs_os_api.log_(level, file, line, msg);
        ecs_os_free(msg);
    } else {
        ecs_os_api.log_(level, file, line, "");
    }
}

void ecs_print_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_printv_(level, file, line, fmt, args);
    va_end(args);    
}

void ecs_logv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args)
{
    if (level > ecs_os_api.log_level_) {
        return;
    }

    ecs_printv_(level, file, line, fmt, args);
}

void ecs_log_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (level > ecs_os_api.log_level_) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    ecs_printv_(level, file, line, fmt, args);
    va_end(args);    
}


void ecs_log_push_(
    int32_t level) 
{
    if (level <= ecs_os_api.log_level_) {
        ecs_os_api.log_indent_ ++;
    }
}

void ecs_log_pop_(
    int32_t level)
{
    if (level <= ecs_os_api.log_level_) {
        ecs_os_api.log_indent_ --;
        ecs_assert(ecs_os_api.log_indent_ >= 0, ECS_INTERNAL_ERROR, NULL);
    }
}

static
void flecs_parser_errorv(
    const char *name,
    const char *expr, 
    int64_t column_arg,
    const char *fmt,
    va_list args,
    bool is_warning)
{
    if (column_arg > 65536) {
        /* Limit column size, which prevents the code from throwing up when the
         * function is called with (expr - ptr), and expr is NULL. */
        column_arg = 0;
    }
    
    int32_t column = flecs_itoi32(column_arg);

    if (ecs_os_api.log_level_ >= -2) {
        ecs_strbuf_t msg_buf = ECS_STRBUF_INIT;

        /* Count number of newlines up until column_arg */
        int32_t i, line = 1;
        if (expr) {
            for (i = 0; i < column; i ++) {
                if (expr[i] == '\n') {
                    line ++;
                }
            }
            
            ecs_strbuf_append(&msg_buf, "%d: ", line);
        }

        ecs_strbuf_vappend(&msg_buf, fmt, args);

        if (expr) {
            ecs_strbuf_appendch(&msg_buf, '\n');

            /* Find start of line by taking column and looking for the
             * last occurring newline */
            if (column != -1) {
                const char *ptr = &expr[column];
                if (ptr[0] == '\n') {
                    ptr --;
                }

                while (ptr[0] != '\n' && ptr > expr) {
                    ptr --;
                }
                
                if (ptr[0] == '\n') {
                    ptr ++;
                }

                if (ptr == expr) {
                    /* ptr is already at start of line */
                } else {
                    column -= (int32_t)(ptr - expr);
                    expr = ptr;
                }
            }

            /* Strip newlines from current statement, if any */            
            char *newline_ptr = strchr(expr, '\n');
            if (newline_ptr) {
                /* Strip newline from expr */
                ecs_strbuf_appendstrn(&msg_buf, expr, 
                    (int32_t)(newline_ptr - expr));
            } else {
                ecs_strbuf_appendstr(&msg_buf, expr);
            }

            ecs_strbuf_appendch(&msg_buf, '\n');

            if (column != -1) {
                int32_t c;
                for (c = 0; c < column; c ++) {
                    ecs_strbuf_appendch(&msg_buf, ' ');
                }
                ecs_strbuf_appendch(&msg_buf, '^');
            }
        }

        char *msg = ecs_strbuf_get(&msg_buf);
        if (is_warning) {
            ecs_os_warn(name, 0, msg);
        } else {
            ecs_os_err(name, 0, msg);
        }
        ecs_os_free(msg);
    }
}

void ecs_parser_errorv_(
    const char *name,
    const char *expr, 
    int64_t column_arg,
    const char *fmt,
    va_list args)
{
    flecs_parser_errorv(name, expr, column_arg, fmt, args, false);
}

void ecs_parser_warningv_(
    const char *name,
    const char *expr, 
    int64_t column_arg,
    const char *fmt,
    va_list args)
{
    flecs_parser_errorv(name, expr, column_arg, fmt, args, true);
}

void ecs_parser_error_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (ecs_os_api.log_level_  >= -2) {
        va_list args;
        va_start(args, fmt);
        ecs_parser_errorv_(name, expr, column, fmt, args);
        va_end(args);
    }
}

void ecs_parser_warning_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (ecs_os_api.log_level_  >= -2) {
        va_list args;
        va_start(args, fmt);
        ecs_parser_warningv_(name, expr, column, fmt, args);
        va_end(args);
    }
}

void ecs_abort_(
    int32_t err,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        char *msg = flecs_vasprintf(fmt, args);
        va_end(args);
        ecs_fatal_(file, line, "#[red]abort()#[reset]: %s (#[blue]%s#[reset])", 
            msg, ecs_strerror(err));
        ecs_os_free(msg);
    } else {
        ecs_fatal_(file, line, "#[red]abort()#[reset]: #[blue]%s#[reset]", 
            ecs_strerror(err));
    }
    ecs_os_api.log_last_error_ = err;
}

void ecs_assert_log_(
    int32_t err,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        char *msg = flecs_vasprintf(fmt, args);
        va_end(args);
        ecs_fatal_(file, line, "#[red]assert(%s)#[reset]: %s (#[blue]%s#[reset])",
            cond_str, msg, ecs_strerror(err));
        ecs_os_free(msg);
    } else {
        ecs_fatal_(file, line, "#[red]assert(%s)#[reset] (#[blue]%s#[reset])",
            cond_str, ecs_strerror(err));
    }
    ecs_os_api.log_last_error_ = err;
}

void ecs_deprecated_(
    const char *file,
    int32_t line,
    const char *msg)
{
    ecs_err_(file, line, "%s", msg);
}

bool ecs_should_log(int32_t level) {
#   if !defined(FLECS_LOG_3)
    if (level == 3) {
        return false;
    }
#   endif
#   if !defined(FLECS_LOG_2)
    if (level == 2) {
        return false;
    }
#   endif
#   if !defined(FLECS_LOG_1)
    if (level == 1) {
        return false;
    }
#   endif

    return level <= ecs_os_api.log_level_;
}

#define ECS_ERR_STR(code) case code: return &(#code[4])

const char* ecs_strerror(
    int32_t error_code)
{
    switch (error_code) {
    ECS_ERR_STR(ECS_INVALID_PARAMETER);
    ECS_ERR_STR(ECS_INTERNAL_ERROR);
    ECS_ERR_STR(ECS_ALREADY_DEFINED);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_SIZE);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_ALIGNMENT);
    ECS_ERR_STR(ECS_NAME_IN_USE);
    ECS_ERR_STR(ECS_OUT_OF_MEMORY);
    ECS_ERR_STR(ECS_DOUBLE_FREE);
    ECS_ERR_STR(ECS_OPERATION_FAILED);
    ECS_ERR_STR(ECS_INVALID_CONVERSION);
    ECS_ERR_STR(ECS_MODULE_UNDEFINED);
    ECS_ERR_STR(ECS_MISSING_SYMBOL);
    ECS_ERR_STR(ECS_ALREADY_IN_USE);
    ECS_ERR_STR(ECS_CYCLE_DETECTED);
    ECS_ERR_STR(ECS_LEAK_DETECTED);
    ECS_ERR_STR(ECS_COLUMN_INDEX_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_COLUMN_IS_NOT_SHARED);
    ECS_ERR_STR(ECS_COLUMN_IS_SHARED);
    ECS_ERR_STR(ECS_COLUMN_TYPE_MISMATCH);
    ECS_ERR_STR(ECS_INVALID_WHILE_READONLY);
    ECS_ERR_STR(ECS_INVALID_FROM_WORKER);
    ECS_ERR_STR(ECS_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_MISSING_OS_API);
    ECS_ERR_STR(ECS_UNSUPPORTED);
    ECS_ERR_STR(ECS_ACCESS_VIOLATION);
    ECS_ERR_STR(ECS_COMPONENT_NOT_REGISTERED);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ID);
    ECS_ERR_STR(ECS_INCONSISTENT_NAME);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ACTION);
    ECS_ERR_STR(ECS_INVALID_OPERATION);
    ECS_ERR_STR(ECS_CONSTRAINT_VIOLATED);
    ECS_ERR_STR(ECS_LOCKED_STORAGE);
    }

    return "unknown error code";
}

#else

/* Empty bodies for when logging is disabled */

void ecs_log_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    (void)level;
    (void)file;
    (void)line;
    (void)fmt;
}

void ecs_parser_error_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    (void)name;
    (void)expr;
    (void)column;
    (void)fmt;
}

void ecs_parser_errorv_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    va_list args)
{
    (void)name;
    (void)expr;
    (void)column;
    (void)fmt;
    (void)args;
}


void ecs_parser_warning_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    (void)name;
    (void)expr;
    (void)column;
    (void)fmt;
}

void ecs_parser_warningv_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    va_list args)
{
    (void)name;
    (void)expr;
    (void)column;
    (void)fmt;
    (void)args;
}

void ecs_abort_(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    (void)error_code;
    (void)file;
    (void)line;
    (void)fmt;
}

void ecs_assert_log_(
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    (void)error_code;
    (void)condition_str;
    (void)file;
    (void)line;
    (void)fmt;
}

void ecs_log_start_capture(bool try) {
    (void)try;
}

char* ecs_log_stop_capture(void) {
    return NULL;
}

#endif

int ecs_log_get_level(void) {
    return ecs_os_api.log_level_;
}

int ecs_log_set_level(
    int level)
{
    int prev = ecs_os_api.log_level_;
    ecs_os_api.log_level_ = level;
    return prev;
}

bool ecs_log_enable_colors(
    bool enabled)
{
    bool prev = ecs_os_api.flags_ & EcsOsApiLogWithColors;
    ECS_BIT_COND(ecs_os_api.flags_, EcsOsApiLogWithColors, enabled);
    return prev;
}

bool ecs_log_enable_timestamp(
    bool enabled)
{
    bool prev = ecs_os_api.flags_ & EcsOsApiLogWithTimeStamp;
    ECS_BIT_COND(ecs_os_api.flags_, EcsOsApiLogWithTimeStamp, enabled);
    return prev;
}

bool ecs_log_enable_timedelta(
    bool enabled)
{
    bool prev = ecs_os_api.flags_ & EcsOsApiLogWithTimeDelta;
    ECS_BIT_COND(ecs_os_api.flags_, EcsOsApiLogWithTimeDelta, enabled);
    return prev;
}

int ecs_log_last_error(void)
{
    int result = ecs_os_api.log_last_error_;
    ecs_os_api.log_last_error_ = 0;
    return result;
}
