#include "private_api.h"

static
char *ecs_vasprintf(
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

    ecs_os_vsprintf(result, fmt, args);

    return result;
}

static
char* ecs_colorize(
    char *msg,
    bool enable_colors)
{
    ecs_strbuf_t buff = ECS_STRBUF_INIT;
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
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr &&
                !isalpha(prev) && (prev != '\\'))
            {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) ||
                (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar &&
                 !isalpha(prev) && !isdigit(prev) && (prev != '_') &&
                 (prev != '.'))
            {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
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
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
            } else if (!ecs_os_strncmp(&ptr[2], "red]", ecs_os_strlen("red]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_RED);
            } else if (!ecs_os_strncmp(&ptr[2], "blue]", ecs_os_strlen("red]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_BLUE);
            } else if (!ecs_os_strncmp(&ptr[2], "magenta]", ecs_os_strlen("magenta]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_MAGENTA);
            } else if (!ecs_os_strncmp(&ptr[2], "cyan]", ecs_os_strlen("cyan]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
            } else if (!ecs_os_strncmp(&ptr[2], "yellow]", ecs_os_strlen("yellow]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_YELLOW);
            } else if (!ecs_os_strncmp(&ptr[2], "grey]", ecs_os_strlen("grey]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_GREY);
            } else if (!ecs_os_strncmp(&ptr[2], "white]", ecs_os_strlen("white]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "bold]", ecs_os_strlen("bold]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_BOLD);
            } else if (!ecs_os_strncmp(&ptr[2], "normal]", ecs_os_strlen("normal]"))) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "reset]", ecs_os_strlen("reset]"))) {
                overrideColor = false;
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
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
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                overrideColor = false;
                isNum = false;
                isStr = false;
                isVar = false;
            }
        }

        if (!dontAppend) {
            ecs_strbuf_appendstrn(&buff, ptr, 1);
        }

        if (!overrideColor) {
            if (((ch == '\'') || (ch == '"')) && !isStr) {
                if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (enable_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
    }

    return ecs_strbuf_get(&buff);
}

static int trace_indent = 0;
static int trace_level = 0;
static bool trace_color = true;

static
void ecs_log_print(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args)
{
    (void)level;
    (void)line;

    if (level > trace_level) {
        return;
    }

    /* Massage filename so it doesn't take up too much space */
    char file_buf[256];
    ecs_os_strcpy(file_buf, file);
    file = file_buf;

    char *file_ptr = strrchr(file, '/');
    if (!file_ptr) {
        file_ptr = strrchr(file, '\\');
    }

    if (file_ptr) {
        file = file_ptr + 1;
    } else {
        file = file_buf;
    }

    char indent[32];
    int i;
    for (i = 0; i < trace_indent; i ++) {
        indent[i * 2] = '|';
        indent[i * 2 + 1] = ' ';
    }
    indent[i * 2] = '\0';

    char *msg_nocolor = ecs_vasprintf(fmt, args);
    char *msg = ecs_colorize(msg_nocolor, trace_color);

    if (trace_color) {
        if (level >= 0) {
            ecs_os_log("%sinfo%s: %s%s%s%s:%s%d%s: %s", ECS_MAGENTA, ECS_NORMAL, 
                ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
                msg);
        } else if (level == -2) {
            ecs_os_warn("%swarn%s: %s%s%s%s:%s%d%s: %s", ECS_YELLOW, ECS_NORMAL, 
                ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
                msg);
        } else if (level == -3) {
            ecs_os_err("%serr%s:  %s%s%s%s:%s%d%s: %s", ECS_RED, ECS_NORMAL, 
                ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
                msg);
        } else if (level == -4) {
            ecs_os_err("%sfatal%s:  %s%s%s%s:%s%d%s: %s", ECS_RED, ECS_NORMAL, 
                ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
                msg);
        }
    } else {
        if (level >= 0) {
            ecs_os_log("info: %s%s:%d: %s", indent, file, line, msg);
        } else if (level == -2) {
            ecs_os_warn("warn: %s%s:%d: %s", indent, file, line, msg); 
        } else if (level == -3) {
            ecs_os_err("err:  %s%s:%d: %s", indent, file, line, msg); 
        } else if (level == -4) {
            ecs_os_err("fatal:  %s%s:%d: %s", indent, file, line, msg);
        }
    }

    ecs_os_free(msg);
    ecs_os_free(msg_nocolor);
}

void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(level, file, line, fmt, args);
    va_end(args);    
}

void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-2, file, line, fmt, args);
    va_end(args);
}

void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-3, file, line, fmt, args);
    va_end(args);
}

void _ecs_fatal(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-4, file, line, fmt, args);
    va_end(args);
}

void ecs_log_push(void) {
    trace_indent ++;
}

void ecs_log_pop(void) {
    trace_indent --;
}

void ecs_tracing_enable(
    int level)
{
    trace_level = level;
}

void ecs_tracing_color_enable(
    bool enabled)
{
    trace_color = enabled;
}

void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (trace_level >= -2) {
        va_list args;
        va_start(args, fmt);
        char *msg = ecs_vasprintf(fmt, args);

        if (column != -1) {
            if (name) {
                ecs_os_err("%s:%d: error: %s", name, column + 1, msg);
            } else {
                ecs_os_err("%d: error: %s", column + 1, msg);
            }
        } else {
            if (name) {
                ecs_os_err("%s: error: %s", name, msg);
            } else {
                ecs_os_err("error: %s", msg);
            }            
        }
        
        ecs_os_err("    %s", expr);

        if (column != -1) {
            ecs_os_err("    %*s^", column, "");
        } else {
            ecs_os_err("");
        }

        ecs_os_free(msg);        
    }

    ecs_os_abort();
}

void _ecs_abort(
    int32_t err,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        char *msg = ecs_vasprintf(fmt, args);
        va_end(args);
        _ecs_fatal(file, line, "%s (%s)", msg, ecs_strerror(err));
        ecs_os_free(msg);
    } else {
        _ecs_fatal(file, line, "%s", ecs_strerror(err));
    }

    ecs_os_abort();
}

void _ecs_assert(
    bool condition,
    int32_t err,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (!condition) {
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            char *msg = ecs_vasprintf(fmt, args);
            va_end(args);            
            _ecs_fatal(file, line, "assert(%s) %s (%s)", 
                cond_str, msg, ecs_strerror(err));
            ecs_os_free(msg);
        } else {
            _ecs_fatal(file, line, "assert(%s) %s", 
                cond_str, ecs_strerror(err));
        }

        ecs_os_abort();
    }
}

void _ecs_deprecated(
    const char *file,
    int32_t line,
    const char *msg)
{
    _ecs_err(file, line, "%s", msg);
}

#define ECS_ERR_STR(code) case code: return &(#code[4])

const char* ecs_strerror(
    int32_t error_code)
{
    switch (error_code) {
    ECS_ERR_STR(ECS_INVALID_PARAMETER);
    ECS_ERR_STR(ECS_NOT_A_COMPONENT);
    ECS_ERR_STR(ECS_TYPE_NOT_AN_ENTITY);
    ECS_ERR_STR(ECS_INTERNAL_ERROR);
    ECS_ERR_STR(ECS_ALREADY_DEFINED);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_SIZE);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_ALIGNMENT);
    ECS_ERR_STR(ECS_OUT_OF_MEMORY);
    ECS_ERR_STR(ECS_MODULE_UNDEFINED);
    ECS_ERR_STR(ECS_COLUMN_INDEX_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_COLUMN_IS_NOT_SHARED);
    ECS_ERR_STR(ECS_COLUMN_IS_SHARED);
    ECS_ERR_STR(ECS_COLUMN_HAS_NO_DATA);
    ECS_ERR_STR(ECS_COLUMN_TYPE_MISMATCH);
    ECS_ERR_STR(ECS_INVALID_WHILE_ITERATING);
    ECS_ERR_STR(ECS_INVALID_FROM_WORKER);
    ECS_ERR_STR(ECS_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_THREAD_ERROR);
    ECS_ERR_STR(ECS_MISSING_OS_API);
    ECS_ERR_STR(ECS_UNSUPPORTED);
    ECS_ERR_STR(ECS_NO_OUT_COLUMNS);
    ECS_ERR_STR(ECS_COLUMN_ACCESS_VIOLATION);
    ECS_ERR_STR(ECS_DESERIALIZE_FORMAT_ERROR);
    ECS_ERR_STR(ECS_TYPE_CONSTRAINT_VIOLATION);
    ECS_ERR_STR(ECS_COMPONENT_NOT_REGISTERED);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ID);
    ECS_ERR_STR(ECS_TYPE_INVALID_CASE);
    ECS_ERR_STR(ECS_INCONSISTENT_NAME);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ACTION);
    ECS_ERR_STR(ECS_INVALID_OPERATION);
    ECS_ERR_STR(ECS_INVALID_DELETE);
    ECS_ERR_STR(ECS_CYCLE_DETECTED);
    ECS_ERR_STR(ECS_LOCKED_STORAGE);
    }

    return "unknown error code";
}
