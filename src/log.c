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

    size = vsnprintf(result, ecs_to_size_t(size), fmt, tmpa);

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
    char *msg)
{
    ecs_strbuf_t buff = ECS_STRBUF_INIT;
    char *ptr, ch, prev = '\0';
    bool isNum = false;
    char isStr = '\0';
    bool isVar = false;
    bool overrideColor = false;
    bool autoColor = true;
    bool dontAppend = false;
    bool use_colors = true;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dontAppend = false;

        if (!overrideColor) {
            if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr &&
                !isalpha(prev) && (prev != '\\'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) ||
                (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar &&
                 !isalpha(prev) && !isdigit(prev) && (prev != '_') &&
                 (prev != '.'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
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
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
            } else if (!ecs_os_strncmp(&ptr[2], "red]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_RED);
            } else if (!ecs_os_strncmp(&ptr[2], "blue]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BLUE);
            } else if (!ecs_os_strncmp(&ptr[2], "magenta]", ecs_os_strlen("magenta]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_MAGENTA);
            } else if (!ecs_os_strncmp(&ptr[2], "cyan]", ecs_os_strlen("cyan]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
            } else if (!ecs_os_strncmp(&ptr[2], "yellow]", ecs_os_strlen("yellow]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_YELLOW);
            } else if (!ecs_os_strncmp(&ptr[2], "grey]", ecs_os_strlen("grey]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREY);
            } else if (!ecs_os_strncmp(&ptr[2], "white]", ecs_os_strlen("white]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "bold]", ecs_os_strlen("bold]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BOLD);
            } else if (!ecs_os_strncmp(&ptr[2], "normal]", ecs_os_strlen("normal]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "reset]", ecs_os_strlen("reset]"))) {
                overrideColor = false;
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
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
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
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
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
    }

    return ecs_strbuf_get(&buff);
}

static int trace_indent = 0;
static int trace_level = 0;

void ecs_log_print(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list valist)
{
    (void)level;
    (void)line;

    if (level > trace_level) {
        return;
    }

    /* Massage filename so it doesn't take up too much space */
    char filebuff[256];
    ecs_os_strcpy(filebuff, file);
    file = filebuff;
    char *file_ptr = strrchr(file, '/');
    if (file_ptr) {
        file = file_ptr + 1;
    }

    /* Extension is likely the same for all files */
    file_ptr = strrchr(file, '.');
    if (file_ptr) {
        *file_ptr = '\0';
    }

    char indent[32];
    int i;
    for (i = 0; i < trace_indent; i ++) {
        indent[i * 2] = '|';
        indent[i * 2 + 1] = ' ';
    }
    indent[i * 2] = '\0';

    char *msg = ecs_vasprintf(fmt, valist);
    char *color_msg = ecs_colorize(msg);

    if (level >= 0) {
        ecs_os_log("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    } else if (level == -2) {
        ecs_os_warn("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    } else if (level <= -2) {
        ecs_os_err("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    }

    ecs_os_free(color_msg);
    ecs_os_free(msg);
}

void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(level, file, line, fmt, valist);
}

void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(-2, file, line, fmt, valist);
}

void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(-3, file, line, fmt, valist);
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

void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (trace_level != -2) {
        va_list valist;
        va_start(valist, fmt);
        char *msg = ecs_vasprintf(fmt, valist);

        ecs_os_err("%s:%d: error: %s", name, column + 1, msg);
        ecs_os_err("    %s", expr);
        ecs_os_err("    %*s^", column, "");
        
        ecs_os_free(msg);        
    }

    ecs_os_abort();
}

void _ecs_abort(
    int32_t error_code,
    const char *param,
    const char *file,
    int32_t line)
{
    if (param) {
        ecs_err("abort %s:%d: %s (%s)",
            file, line, ecs_strerror(error_code), param);
    } else {
        ecs_err("abort %s:%d: %s", file, line, ecs_strerror(error_code));
    }

    ecs_os_abort();
}

void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    int32_t line)
{
    if (!condition) {
        if (param) {
            ecs_err("assert(%s) %s:%d: %s (%s)",
                condition_str, file, line, ecs_strerror(error_code), param);
        } else {
            ecs_err("assert(%s) %s:%d: %s",
                condition_str, file, line, ecs_strerror(error_code));
        }

        ecs_os_abort();
    }
}

const char* ecs_strerror(
    int32_t error_code)
{
    switch (error_code) {
    case ECS_INVALID_HANDLE:
        return "invalid handle";
    case ECS_INVALID_PARAMETER:
        return "invalid parameters";
    case ECS_INVALID_COMPONENT_ID:
        return "invalid component id";
    case ECS_INVALID_TYPE_EXPRESSION:
        return "invalid type expression";
    case ECS_INVALID_SIGNATURE:
        return "invalid system signature";
    case ECS_INVALID_EXPRESSION:
        return "invalid type expression/signature";
    case ECS_MISSING_SYSTEM_CONTEXT:
        return "missing system context";
    case ECS_UNKNOWN_COMPONENT_ID:
        return "unknown component id";
    case ECS_UNKNOWN_TYPE_ID:
        return "unknown type id";
    case ECS_TYPE_NOT_AN_ENTITY:
        return "type contains more than one entity";
    case ECS_NOT_A_COMPONENT:
        return "handle is not a component";
    case ECS_INTERNAL_ERROR:
        return "internal error";
    case ECS_MORE_THAN_ONE_PREFAB:
        return "more than one prefab added to entity";
    case ECS_ALREADY_DEFINED:
        return "entity has already been defined";
    case ECS_INVALID_COMPONENT_SIZE:
        return "the specified size does not match the component";
    case ECS_OUT_OF_MEMORY:
        return "out of memory";
    case ECS_MODULE_UNDEFINED:
        return "module is undefined";
    case ECS_COLUMN_INDEX_OUT_OF_RANGE:
        return "column index out of range";
    case ECS_COLUMN_IS_NOT_SHARED:
        return "column is not shared";
    case ECS_COLUMN_IS_SHARED:
        return "column is shared";
    case ECS_COLUMN_HAS_NO_DATA:
        return "column has no data";
    case ECS_COLUMN_TYPE_MISMATCH:
        return "column retrieved with mismatching type";
    case ECS_INVALID_WHILE_MERGING:
        return "operation is invalid while merging";
    case ECS_INVALID_WHILE_ITERATING:
        return "operation is invalid while iterating";    
    case ECS_INVALID_FROM_WORKER:
        return "operation is invalid from worker thread";
    case ECS_UNRESOLVED_IDENTIFIER:
        return "unresolved identifier";
    case ECS_OUT_OF_RANGE:
        return "index is out of range";
    case ECS_COLUMN_IS_NOT_SET:
        return "column is not set (use ecs_column_test for optional columns)";
    case ECS_UNRESOLVED_REFERENCE:
        return "unresolved reference for system";
    case ECS_THREAD_ERROR:
        return "failed to create thread";
    case ECS_MISSING_OS_API:
        return "missing implementation for OS API function";
    case ECS_TYPE_TOO_LARGE:
        return "type contains too many entities";
    case ECS_INVALID_PREFAB_CHILD_TYPE:
        return "a prefab child type must have at least one INSTANCEOF element";
    case ECS_UNSUPPORTED:
        return "operation is unsupported";
    case ECS_NO_OUT_COLUMNS:
        return "on demand system has no out columns";
    case ECS_COLUMN_ACCESS_VIOLATION:
        return "invalid access to readonly column (use const)";
    case ECS_DESERIALIZE_COMPONENT_ID_CONFLICT:
        return "serialized data contains conflicting component id";
    case ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT:
        return "serialized data contains conflicting component size";   
    case ECS_DESERIALIZE_FORMAT_ERROR:
        return "serialized data has invalid format";
    case ECS_INVALID_REACTIVE_SIGNATURE:
        return "signature is not valid for reactive system (must contain at least one ANY column)";
    case ECS_INCONSISTENT_COMPONENT_NAME:
        return "component registered twice with a different name";
    case ECS_TYPE_CONSTRAINT_VIOLATION:
        return "type constraint violated";
    }

    return "unknown error code";
}
