
#include "include/private/types.h"

static uint32_t last_error_code;
static const char *last_param;

void ecs_throw(
    uint32_t error_code,
    const char *param)
{
    last_error_code = error_code;
    last_param = param;
}

uint32_t ecs_raise(void)
{
    uint32_t code = last_error_code;

    if (last_error_code) {
        if (last_param) {
            fprintf(stderr, "error: %s (%s)\n",
                ecs_strerror(last_error_code), last_param);
        } else {
            fprintf(stderr, "error: %s\n", ecs_strerror(last_error_code));
        }

        last_error_code = 0;
        last_param = NULL;
    }

    return code;
}

uint32_t ecs_catch(void)
{
    uint32_t code = last_error_code;
    last_error_code = 0;
    last_param = NULL;
    return code;
}

void _ecs_abort(
    uint32_t error_code,
    const char *param,
    const char *file,
    uint32_t line)
{
    if (param) {
        fprintf(stderr, "abort %s:%d: %s (%s)\n",
            file, line, ecs_strerror(error_code), param);
    } else {
        fprintf(stderr, "abort %s:%d: %s\n", file, line, ecs_strerror(error_code));
    }
    abort();
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
            fprintf(stderr, "assert(%s) %s:%d: %s (%s)\n",
                condition_str, file, line, ecs_strerror(error_code), param);
        } else {
            fprintf(stderr, "assert(%s) %s:%d: %s\n",
                condition_str, file, line, ecs_strerror(error_code));
        }
        abort();
    }
}

const char* ecs_strerror(
    uint32_t error_code)
{
    switch (error_code) {
    case ECS_INVALID_HANDLE:
        return "invalid handle";
    case ECS_INVALID_PARAMETERS:
        return "invalid parameters";
    case ECS_INVALID_COMPONENT_ID:
        return "invalid component id";
    case ECS_INVALID_COMPONENT_EXPRESSION:
        return "invalid component expression for system";
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
    case ecs_type_IN_USE:
        return "type specified by system is already in use";
    case ECS_INTERNAL_ERROR:
        return "internal error";
    case ECS_MORE_THAN_ONE_PREFAB:
        return "more than one prefab added to entity";
    case ECS_ENTITY_ALREADY_DEFINED:
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
    case ECS_INVALID_WHILE_MERGING:
        return "operation is invalid while merging";
    case ECS_UNRESOLVED_IDENTIFIER:
        return "unresolved identifier";
    case ECS_OUT_OF_RANGE:
        return "index is out of range";
    case ECS_COLUMN_IS_NOT_SET:
        return "column is not set (use ecs_column_test for optional columns)";
    case ECS_UNRESOLVED_REFERENCE:
        return "unresolved reference for system";
    }

    return "unknown error code";
}
