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
    case ECS_CANT_USE_NOT_IN_OR_EXPRESSION:
        return "cannot use NOT (!) operator in an OR (|) expression";
    case ECS_CANT_USE_OR_WITH_EMPTY_FROM_EXPRESSION:
        return "cannot use OR (|) operator in combination with an empty FROM (.) expression";
    case ECS_ZERO_CAN_ONLY_APPEAR_BY_ITSELF:
        return "0 can only appear by itself";
    case ECS_UNSESOLVED_COMPONENT_NAME:
        return "unresolved component name '%s'";
    case ECS_UNRESOLVED_ENTITY_NAME:
        return "unresolved entity name '%s'";
    }

    return "unknown error code";
}


void ecs_print_error_string(const char *signature, const char *system_id, const char *error_description, char *component_id, int count, ...) {
    int argument_number = 1;
    char error_string[200] = "";
    char error_indicator[200] = "";
    char custom_error_message[200] = "";
    uint position = 0;
    int i;

    va_list valist;
    va_start(valist, count);

    if(component_id){
        if(strlen(component_id) == 0){
            position = strlen(signature) - 1;
        } else {
            char* pointer_to_string = strstr(signature, component_id);
            position = pointer_to_string - signature;
        }
    }

    for(i = 0; i < position; i++){
        if(signature[i] == ',') argument_number++;
        error_indicator[i] = '~';
    }
    error_indicator[i] = '^';
    error_indicator[++i] = '\0';
    if (count > 0) {
        vsprintf(custom_error_message, error_description, valist);
    } else {
        sprintf(custom_error_message, error_description, component_id);
    }

    sprintf(error_string, "%s at argument #%d. Error: \"%s\"\n%s\n%s\n\0", system_id ? system_id : "Error" , argument_number, custom_error_message, signature, error_indicator);
    printf("%s", error_string);
    va_end(valist);
}