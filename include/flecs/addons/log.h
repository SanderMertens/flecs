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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FLECS_LOG

////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_deprecated(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Increase log stack.
 * This operation increases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 * 
 * @param level The log level.
 */
FLECS_API
void _ecs_log_push(int32_t level);

/** Decrease log stack.
 * This operation decreases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 * 
 * @param level The log level.
 */
FLECS_API
void _ecs_log_pop(int32_t level);

/** Should current level be logged.
 * This operation returns true when the specified log level should be logged 
 * with the current log level.
 *
 * @param level The log level to check for.
 * @return Whether logging is enabled for the current level.
 */
FLECS_API
bool ecs_should_log(int32_t level);

////////////////////////////////////////////////////////////////////////////////
//// Error reporting
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_API
const char* ecs_strerror(
    int32_t error_code);

#else // FLECS_LOG

////////////////////////////////////////////////////////////////////////////////
//// Dummy macros for when logging is disabled
////////////////////////////////////////////////////////////////////////////////

#define _ecs_deprecated(file, line, msg)\
    (void)file;\
    (void)line;\
    (void)msg

#define _ecs_log_push(level)
#define _ecs_log_pop(level)
#define ecs_should_log(level) false

#define ecs_strerror(error_code)\
    (void)error_code

#endif // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Logging functions (do nothing when logging is enabled)
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_log(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_logv(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

FLECS_API
void _ecs_abort(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
bool _ecs_assert(
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


////////////////////////////////////////////////////////////////////////////////
//// Logging macros
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macros */

/* Base logging function. Accepts a custom level */
#define ecs_log(level, ...)\
    _ecs_log(level, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_logv(level, fmt, args)\
    _ecs_logv(level, __FILE__, __LINE__, fmt, args)

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
#define _ecs_fatal(file, line, ...) _ecs_log(-4, file, line, __VA_ARGS__)
#define ecs_fatal(...) _ecs_fatal(__FILE__, __LINE__, __VA_ARGS__)

/* Optionally include warnings about using deprecated features */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
#define ecs_deprecated(...)\
    _ecs_deprecated(__FILE__, __LINE__, __VA_ARGS__)
#else
#define ecs_deprecated(...)
#endif // FLECS_NO_DEPRECATED_WARNINGS

/* If no tracing verbosity is defined, pick default based on build config */
#if !(defined(FLECS_LOG_0) || defined(FLECS_LOG_1) || defined(FLECS_LOG_2) || defined(FLECS_LOG_3))
#if !defined(FLECS_NDEBUG)
#define FLECS_LOG_3 /* Enable all tracing in debug mode. May slow things down */
#else
#define FLECS_LOG_0 /* Only enable infrequent tracing in release mode */
#endif // !defined(FLECS_NDEBUG)
#endif // !(defined(FLECS_LOG_0) || defined(FLECS_LOG_1) || defined(FLECS_LOG_2) || defined(FLECS_LOG_3))


/* Define/undefine macros based on compiled-in tracing level. This can optimize
 * out tracing statements from a build, which improves performance. */

#if defined(FLECS_LOG_3) /* All debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2() _ecs_log_push(2);
#define ecs_log_push_3() _ecs_log_push(3);

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2() _ecs_log_pop(2);
#define ecs_log_pop_3() _ecs_log_pop(3);

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() ecs_should_log(2)
#define ecs_should_log_3() ecs_should_log(3)

#define FLECS_LOG_2
#define FLECS_LOG_1
#define FLECS_LOG_0

#elif defined(FLECS_LOG_2) /* Level 2 and below debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...)

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2() _ecs_log_push(2);
#define ecs_log_push_3()

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2() _ecs_log_pop(2);
#define ecs_log_pop_3()

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() ecs_should_log(2)
#define ecs_should_log_3() false

#define FLECS_LOG_1
#define FLECS_LOG_0

#elif defined(FLECS_LOG_1) /* Level 1 debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() false
#define ecs_should_log_3() false

#define FLECS_LOG_0

#elif defined(FLECS_LOG_0) /* No debug tracing enabled */
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1()
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1()
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#define ecs_should_log_1() false
#define ecs_should_log_2() false
#define ecs_should_log_3() false

#else /* No tracing enabled */
#undef ecs_trace
#define ecs_trace(...)
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1()
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1()
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#endif // defined(FLECS_LOG_3)

/* Default debug tracing is at level 1 */
#define ecs_dbg ecs_dbg_1

/* Default level for push/pop is 0 */
#define ecs_log_push() _ecs_log_push(0)
#define ecs_log_pop() _ecs_log_pop(0)

/** Abort 
 * Unconditionally aborts process. */
#define ecs_abort(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    ecs_os_abort(); abort(); /* satisfy compiler/static analyzers */

/** Assert 
 * Aborts if condition is false, disabled in debug mode. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    if (!_ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */
#endif // FLECS_NDEBUG

#define ecs_assert_var(var, error_code, ...)\
    ecs_assert(var, error_code, __VA_ARGS__);\
    (void)var

/** Debug assert 
 * Assert that is only valid in debug mode (ignores FLECS_KEEP_ASSERT) */
#ifndef FLECS_NDEBUG
#define ecs_dbg_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_dbg_assert(condition, error_code, ...)
#endif

/* Silence dead code/unused label warnings when compiling without checks. */
#define ecs_dummy_check\
    if ((false)) {\
        goto error;\
    }

/** Check
 * goto error if condition is false. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_check(condition, error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!_ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        goto error;\
    }
#else // FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    ecs_assert(condition, error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Panic
 * goto error when FLECS_SOFT_ASSERT is defined, otherwise abort */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_throw(error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_throw(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    goto error;
#else
#define ecs_throw(error_code, ...)\
    ecs_abort(error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Parser error */
#define ecs_parser_error(name, expr, column, ...)\
    _ecs_parser_error(name, expr, column, __VA_ARGS__)

#define ecs_parser_errorv(name, expr, column, fmt, args)\
    _ecs_parser_errorv(name, expr, column, fmt, args)

#endif // FLECS_LEGACY


////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable tracing.
 * This will enable builtin tracing. For tracing to work, it will have to be
 * compiled in which requires defining one of the following macros:
 *
 * FLECS_LOG_0 - All tracing is disabled
 * FLECS_LOG_1 - Enable tracing level 1
 * FLECS_LOG_2 - Enable tracing level 2 and below
 * FLECS_LOG_3 - Enable tracing level 3 and below
 *
 * If no tracing level is defined and this is a debug build, FLECS_LOG_3 will
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

/** Get last logged error code.
 * Calling this operation resets the error code.
 *
 * @return Last error, 0 if none was logged since last call to last_error.
 */
FLECS_API
int ecs_log_last_error(void);


////////////////////////////////////////////////////////////////////////////////
//// Error codes
////////////////////////////////////////////////////////////////////////////////

#define ECS_INVALID_OPERATION (1)
#define ECS_INVALID_PARAMETER (2)
#define ECS_CONSTRAINT_VIOLATED (3)
#define ECS_OUT_OF_MEMORY (4)
#define ECS_OUT_OF_RANGE (5)
#define ECS_UNSUPPORTED (6)
#define ECS_INTERNAL_ERROR (7)
#define ECS_ALREADY_DEFINED (8)
#define ECS_MISSING_OS_API (9)
#define ECS_OPERATION_FAILED (10)
#define ECS_INVALID_CONVERSION (11)
#define ECS_ID_IN_USE (12)
#define ECS_CYCLE_DETECTED (13)

#define ECS_INCONSISTENT_NAME (20)
#define ECS_NAME_IN_USE (21)
#define ECS_NOT_A_COMPONENT (22)
#define ECS_INVALID_COMPONENT_SIZE (23)
#define ECS_INVALID_COMPONENT_ALIGNMENT (24)
#define ECS_COMPONENT_NOT_REGISTERED (25)
#define ECS_INCONSISTENT_COMPONENT_ID (26)
#define ECS_INCONSISTENT_COMPONENT_ACTION (27)
#define ECS_MODULE_UNDEFINED (28)
#define ECS_MISSING_SYMBOL (29)
#define ECS_ALREADY_IN_USE (30)

#define ECS_ACCESS_VIOLATION (40)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
#define ECS_COLUMN_IS_NOT_SHARED (42)
#define ECS_COLUMN_IS_SHARED (43)
#define ECS_COLUMN_TYPE_MISMATCH (45)

#define ECS_INVALID_WHILE_READONLY (70)
#define ECS_LOCKED_STORAGE (71)
#define ECS_INVALID_FROM_WORKER (72)


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

#ifdef __cplusplus
}
#endif

#endif // FLECS_LOG_H
