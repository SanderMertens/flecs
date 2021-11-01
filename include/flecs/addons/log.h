/**
 * @file log.h
 * @brief Logging addon.
 * 
 * The logging addon provides an API for (debug) tracing and reporting errors
 * at various levels. When enabled, the logging addon can provide more detailed
 * information about the state of the ECS and any errors that may occur.
 * 
 * The logging addon can be disabled to reduce footprint of the library, but
 * limits information logged to only file, line and error code.
 * 
 * When enabled the logging addon can be configured to exclude levels of tracing
 * from the build to reduce the impact on performance. By default all debug 
 * tracing is enabled for debug builds, tracing is enabled at release builds.
 * 
 * Applications can change the logging level at runtime with ecs_log_set_level,
 * but what is actually logged depends on what is compiled (when compiled 
 * without debug tracing, setting the runtime level to debug won't have an 
 * effect).
 * 
 * The logging addon uses the OS API log_ function for all tracing.
 * 
 * Note that even when the logging addon is not enabled, its header/source must
 * be included in a build. To prevent unused variable warnings in the code, some
 * API functions are included when the addon is disabled, but have empty bodies.
 */

#ifndef FLECS_LOG_H
#define FLECS_LOG_H

#ifdef FLECS_LOG

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_log(
    int32_t level,
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


////////////////////////////////////////////////////////////////////////////////
//// Error reporting
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

/** Parser error.
 * This function is used by parsers, and includes additional information such
 * as the failed expression, position in the expression and the name of the
 * expression/script being parsed. */
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

#ifdef __cplusplus
}
#endif

#else // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Functions/macro's that do nothing when logging is disabled
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_log(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

#define _ecs_deprecated(file, line, msg)\
    (void)file;\
    (void)line;\
    (void)msg

#define ecs_log_push()
#define ecs_log_pop()

#define ecs_strerror(error_code)\
    (void)error_code

FLECS_API
void _ecs_abort(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

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

#endif // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Logging Macro's
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macro's */

/* Base logging function. Accepts a custom level */
#define ecs_log(level, ...)\
    _ecs_log(level, __FILE__, __LINE__, __VA_ARGS__)

/* Tracing. Used for logging of infrequent events  */
#define _ecs_trace(file, line, ...) _ecs_log(0, file, line, __VA_ARGS__)
#define ecs_trace(...) _ecs_trace(__FILE__, __LINE__, __VA_ARGS__)

/* Warning. Used when an issue occurs, but operation is successful */
#define _ecs_warn(file, line, ...) _ecs_log(-2, file, line, __VA_ARGS__)
#define ecs_warn(...) _ecs_warn(__FILE__, __LINE__, __VA_ARGS__)

/* Error. Used when an issue occurs, and operation failed. */
#define _ecs_err(file, line, ...) _ecs_log(-3, file, line, __VA_ARGS__)
#define ecs_err(...) _ecs_err(__FILE__, __LINE__, __VA_ARGS__)

/* Fatal. Used when an issue occurs, and the application cannot continue. */
#define _ecs_fatal(file, line, ...) _ecs_log(-3, file, line, __VA_ARGS__)
#define ecs_fatal(...) _ecs_fatal(__FILE__, __LINE__, __VA_ARGS__)

/* Optionally include warnings about using deprecated features */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
#define ecs_deprecated(...)\
    _ecs_deprecated(__FILE__, __LINE__, __VA_ARGS__)
#else
#define ecs_deprecated(...)
#endif // FLECS_NO_DEPRECATED_WARNINGS

/* If no tracing verbosity is defined, pick default based on build config */
#if !(defined(ECS_TRACE_0) || defined(ECS_TRACE_1) || defined(ECS_TRACE_2) || defined(ECS_TRACE_3))
#if !defined(NDEBUG)
#define ECS_TRACE_3 /* Enable all tracing in debug mode. May slow things down */
#else
#define ECS_TRACE_0 /* Only enable infrequent tracing in release mode */
#endif // !defined(NDEBUG)
#endif // !(defined(ECS_TRACE_0) || defined(ECS_TRACE_1) || defined(ECS_TRACE_2) || defined(ECS_TRACE_3))


/* Define/undefine macro's based on compiled-in tracing level. This can optimize
 * out tracing statements from a build, which improves performance. */

#if defined(ECS_TRACE_3) /* All debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

#elif defined(ECS_TRACE_2) /* Level 2 and below debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...)

#elif defined(ECS_TRACE_1) /* Level 1 debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#elif defined(ECS_TRACE_0) /* No debug tracing enabled */
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#else /* No tracing enabled */
#undef ecs_trace
#define ecs_trace(...)
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#endif // defined(ECS_TRACE_3)

#define ecs_dbg ecs_dbg_1 /* Default debug tracing is at level 1 */

#ifdef NDEBUG
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    _ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
    assert(condition)
#endif // NDEBUG

#define ecs_abort(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__); abort()

#define ecs_parser_error(name, expr, column, ...)\
    _ecs_parser_error(name, expr, column, __VA_ARGS__)

#define ecs_parser_errorv(name, expr, column, fmt, args)\
    _ecs_parser_errorv(name, expr, column, fmt, args)

#endif // FLECS_LEGACY


////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/** Enable or disable tracing.
 * This will enable builtin tracing. For tracing to work, it will have to be
 * compiled in which requires defining one of the following macro's:
 *
 * ECS_TRACE_0 - All tracing is disabled
 * ECS_TRACE_1 - Enable tracing level 1
 * ECS_TRACE_2 - Enable tracing level 2 and below
 * ECS_TRACE_3 - Enable tracing level 3 and below
 *
 * If no tracing level is defined and this is a debug build, ECS_TRACE_3 will
 * have been automatically defined.
 *
 * The provided level corresponds with the tracing level. If -1 is provided as
 * value, warnings are disabled. If -2 is provided, errors are disabled as well.
 *
 * @param level Desired tracing level.
 * @return Previous tracing level.
 */
FLECS_API
int ecs_log_set_level(
    int level);

/** Enable/disable tracing with colors.
 * By default colors are enabled.
 *
 * @param enabled Whether to enable tracing with colors.
 * @return Previous color setting.
 */
FLECS_API
bool ecs_log_enable_colors(
    bool enabled);

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
//// Used when logging with colors is enabled
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

#endif // FLECS_LOG_H
