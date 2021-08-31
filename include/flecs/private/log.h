/**
 * @file log.h
 * @brief Internal logging API.
 *
 * Internal utility functions for tracing, warnings and errors. 
 */

#ifndef FLECS_LOG_H
#define FLECS_LOG_H

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

FLECS_API
void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_fatal(
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_deprecated(
    const char *file, 
    int32_t line, 
    const char *msg);

FLECS_API
void ecs_log_push(void);

FLECS_API
void ecs_log_pop(void);

#ifndef FLECS_LEGACY

#define ecs_trace(lvl, ...)\
    _ecs_trace(lvl, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_warn(...)\
    _ecs_warn(__FILE__, __LINE__, __VA_ARGS__)

#define ecs_err(...)\
    _ecs_err(__FILE__, __LINE__, __VA_ARGS__)

#define ecs_fatal(...)\
    _ecs_fatal(__FILE__, __LINE__, __VA_ARGS__)

#ifndef FLECS_NO_DEPRECATED_WARNINGS
#define ecs_deprecated(...)\
    _ecs_deprecated(__FILE__, __LINE__, __VA_ARGS__)
#else
#define ecs_deprecated(...)
#endif

/* If no tracing verbosity is defined, pick default based on build config */
#if !(defined(ECS_TRACE_0) || defined(ECS_TRACE_1) || defined(ECS_TRACE_2) || defined(ECS_TRACE_3))
#if !defined(NDEBUG)
#define ECS_TRACE_3 /* Enable all tracing in debug mode. May slow things down */
#else
#define ECS_TRACE_1 /* Only enable infrequent tracing in release mode */
#endif
#endif

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

////////////////////////////////////////////////////////////////////////////////
//// Exceptions
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_API
const char* ecs_strerror(
    int32_t error_code);

/** Abort */
FLECS_API
void _ecs_abort(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

#define ecs_abort(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__); abort()

/** Assert */
FLECS_API
void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

#ifdef NDEBUG
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    _ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
    assert(condition)
#endif

FLECS_API
void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...);

FLECS_API
void _ecs_parser_errorv(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    va_list args);

#ifndef FLECS_LEGACY

#define ecs_parser_error(name, expr, column, ...)\
    _ecs_parser_error(name, expr, column, __VA_ARGS__)

#define ecs_parser_errorv(name, expr, column, fmt, args)\
    _ecs_parser_errorv(name, expr, column, fmt, args)

#endif

#ifdef __cplusplus
}
#endif

#endif
