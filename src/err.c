
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
        return "invalid component expression";
    case ECS_MISSING_SYSTEM_CONTEXT:
        return "missing system context";
    case ECS_UNKNOWN_COMPONENT_ID:
        return "unknown component id";
    case ECS_NOT_A_COMPONENT:
        return "handle is not a component";
    }

    return "unknown error code";
}
