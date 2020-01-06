#include "flecs_private.h"
#include <string.h>

void _ecs_abort(
    uint32_t error_code,
    const char *param,
    const char *file,
    uint32_t line)
{
    if (param) {
        ecs_os_err("abort %s:%d: %s (%s)",
            file, line, ecs_strerror(error_code), param);
    } else {
        ecs_os_err("abort %s:%d: %s", file, line, ecs_strerror(error_code));
    }

    ecs_os_abort();
}

void _ecs_assert(
    bool condition,
    uint32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    uint32_t line)
{
    if (!condition) {
        if (param) {
            ecs_os_err("assert(%s) %s:%d: %s (%s)",
                condition_str, file, line, ecs_strerror(error_code), param);
        } else {
            ecs_os_err("assert(%s) %s:%d: %s",
                condition_str, file, line, ecs_strerror(error_code));
        }

        ecs_os_abort();
    }
}

void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int column,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    /* Most messages should fit in 80 characters, but if not, allocate a string
     * that is large enough */
    char *msg, msg_buf[80];
    int required = vsnprintf(msg_buf, 80, fmt, valist);
    if (required > 80) {
        msg = ecs_os_malloc(required + 1);
        int written = vsnprintf(msg, required, fmt, valist);
        ecs_assert(written == required, ECS_INTERNAL_ERROR, NULL);
    } else {
        msg = msg_buf;
    }

    va_end(valist);

    fprintf(stderr, "%s:%d: error: %s\n", name, column + 1, msg);
    fprintf(stderr, "    %s\n", expr);
    fprintf(stderr, "    %*s^\n", column, "");

    if (msg != msg_buf) {
        ecs_os_free(msg);
    }

    ecs_os_abort();
}

const char* ecs_strerror(
    uint32_t error_code)
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
    }

    return "unknown error code";
}
