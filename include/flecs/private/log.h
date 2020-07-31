#ifndef FLECS_LOG_H
#define FLECS_LOG_H

#include "api_types.h"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Color macro's
////////////////////////////////////////////////////////////////////////////////

#define ECS_BLACK   "\033[1;30m"
#define ECS_RED     "\033[0;31m"
#define ECS_GREEN   "\033[0;32m"
#define ECS_YELLOW  "\033[0;33m"
#define ECS_BLUE    "\033[0;34m"
#define ECS_MAGENTA "\033[0;35m"
#define ECS_CYAN    "\033[0;36m"
#define ECS_WHITE   "\033[1;37m"
#define ECS_GREY    "\033[0;37m"
#define ECS_NORMAL  "\033[0;49m"
#define ECS_BOLD    "\033[1;49m"


////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_EXPORT
void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_EXPORT
void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...);
FLECS_EXPORT
void ecs_log_push(void);

FLECS_EXPORT
void ecs_log_pop(void);

#ifndef FLECS_LEGACY

#define ecs_trace(lvl, ...)\
    _ecs_trace(lvl, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_warn(...)\
    _ecs_warn(__FILE__, __LINE__, __VA_ARGS__)

#define ecs_err(...)\
    _ecs_err(__FILE__, __LINE__, __VA_ARGS__)


/* If in debug mode and no tracing verbosity is defined, compile all tracing */
#if !defined(NDEBUG) && !(defined(ECS_TRACE_0) || defined(ECS_TRACE_1) || defined(ECS_TRACE_2) || defined(ECS_TRACE_3))
#define ECS_TRACE_3
#endif

#ifndef NDEBUG
#if defined(ECS_TRACE_3)
#define ecs_trace_1(...) ecs_trace(1, __VA_ARGS__);
#define ecs_trace_2(...) ecs_trace(2, __VA_ARGS__);
#define ecs_trace_3(...) ecs_trace(3, __VA_ARGS__);

#elif defined(ECS_TRACE_2)
#define ecs_trace_1(...) ecs_trace(1, __VA_ARGS__);
#define ecs_trace_2(...) ecs_trace(2, __VA_ARGS__);
#define ecs_trace_3(...)

#elif defined(ECS_TRACE_1)
#define ecs_trace_1(...) ecs_trace(1, __VA_ARGS__);
#define ecs_trace_2(...)
#define ecs_trace_3(...)
#endif
#else
#define ecs_trace_1(...)
#define ecs_trace_2(...)
#define ecs_trace_3(...)
#endif
#endif

////////////////////////////////////////////////////////////////////////////////
//// Exceptions
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_EXPORT
const char* ecs_strerror(
    int32_t error_code);

/** Abort */
FLECS_EXPORT
void _ecs_abort(
    int32_t error_code,
    const char *param,
    const char *file,
    int32_t line);

#define ecs_abort(error_code, param)\
    _ecs_abort(error_code, param, __FILE__, __LINE__); abort()

/** Assert */
FLECS_EXPORT
void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    int32_t line);

#ifdef NDEBUG
#define ecs_assert(condition, error_code, param)
#else
#define ecs_assert(condition, error_code, param)\
    _ecs_assert(condition, error_code, param, #condition, __FILE__, __LINE__);\
    assert(condition)
#endif

FLECS_EXPORT
void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...);

#ifndef FLECS_LEGACY

#define ecs_parser_error(name, expr, column, ...)\
    _ecs_parser_error(name, expr, column, __VA_ARGS__);\
    abort()

#endif

#ifdef __cplusplus
}
#endif

#endif
