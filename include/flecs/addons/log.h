/**
 * @file addons/log.h
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
 * Applications can change the logging level at runtime with ecs_log_set_level(),
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

/**
 * @defgroup c_addons_log Log
 * @ingroup c_addons
 * Logging functions.
 *
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

/** Log message indicating an operation is deprecated.
 *
 * @param file The source file.
 * @param line The source line.
 * @param msg The deprecation message.
 */
FLECS_API
void ecs_deprecated_(
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
void ecs_log_push_(int32_t level);

/** Decrease log stack.
 * This operation decreases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 *
 * @param level The log level.
 */
FLECS_API
void ecs_log_pop_(int32_t level);

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

/** Get description for error code.
 *
 * @param error_code The error code.
 * @return String describing the error code.
 */
FLECS_API
const char* ecs_strerror(
    int32_t error_code);

#else // FLECS_LOG

////////////////////////////////////////////////////////////////////////////////
//// Dummy macros for when logging is disabled
////////////////////////////////////////////////////////////////////////////////

#define ecs_deprecated_(file, line, msg)\
    (void)file;\
    (void)line;\
    (void)msg

#define ecs_log_push_(level)
#define ecs_log_pop_(level)
#define ecs_should_log(level) false

#define ecs_strerror(error_code)\
    (void)error_code

#endif // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Logging functions (do nothing when logging is enabled)
////////////////////////////////////////////////////////////////////////////////

/** Print at the provided log level.
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_print_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Print at the provided log level (va_list).
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_printv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

/** Log at the provided level.
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_log_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log at the provided level (va_list).
 *
 * @param level The log level.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_logv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

/** Abort with error code.
 *
 * @param error_code The error code.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_abort_(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log an assertion failure.
 *
 * @param error_code The error code.
 * @param condition_str The condition that was not met.
 * @param file The source file.
 * @param line The source line.
 * @param fmt The format string.
 */
FLECS_API
void ecs_assert_log_(
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

/** Log a parser error.
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 */
FLECS_API
void ecs_parser_error_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    ...);

/** Log a parser error (va_list).
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_parser_errorv_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    va_list args);

/** Log a parser warning.
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 */
FLECS_API
void ecs_parser_warning_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    ...);

/** Log a parser warning (va_list).
 *
 * @param name The name of the expression.
 * @param expr The expression string.
 * @param column The column at which the error occurred.
 * @param fmt The format string.
 * @param args The format argument list.
 */
FLECS_API
void ecs_parser_warningv_(
    const char *name,
    const char *expr,
    int64_t column,
    const char *fmt,
    va_list args);


////////////////////////////////////////////////////////////////////////////////
//// Logging macros
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macros */

/** Base logging function. Accepts a custom level. */
#define ecs_print(level, ...)\
    ecs_print_(level, __FILE__, __LINE__, __VA_ARGS__)

/** Base logging function with va_list. */
#define ecs_printv(level, fmt, args)\
    ecs_printv_(level, __FILE__, __LINE__, fmt, args)

/** Log at the provided level. */
#define ecs_log(level, ...)\
    ecs_log_(level, __FILE__, __LINE__, __VA_ARGS__)

/** Log at the provided level with va_list. */
#define ecs_logv(level, fmt, args)\
    ecs_logv_(level, __FILE__, __LINE__, fmt, args)

/** Tracing. Used for logging of infrequent events. */
#define ecs_trace_(file, line, ...) ecs_log_(0, file, line, __VA_ARGS__)
/** Tracing macro. */
#define ecs_trace(...) ecs_trace_(__FILE__, __LINE__, __VA_ARGS__)

/** Warning. Used when an issue occurs, but operation is successful. */
#define ecs_warn_(file, line, ...) ecs_log_(-2, file, line, __VA_ARGS__)
/** Warning macro. */
#define ecs_warn(...) ecs_warn_(__FILE__, __LINE__, __VA_ARGS__)

/** Error. Used when an issue occurs, and operation failed. */
#define ecs_err_(file, line, ...) ecs_log_(-3, file, line, __VA_ARGS__)
/** Error macro. */
#define ecs_err(...) ecs_err_(__FILE__, __LINE__, __VA_ARGS__)

/** Fatal. Used when an issue occurs, and the application cannot continue. */
#define ecs_fatal_(file, line, ...) ecs_log_(-4, file, line, __VA_ARGS__)
/** Fatal macro. */
#define ecs_fatal(...) ecs_fatal_(__FILE__, __LINE__, __VA_ARGS__)

/** Optionally include warnings about using deprecated features. */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
/** Emit deprecation warning. */
#define ecs_deprecated(...)\
    ecs_deprecated_(__FILE__, __LINE__, __VA_ARGS__)
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
/** Debug trace at level 1. */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
/** Debug trace at level 2. */
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
/** Debug trace at level 3. */
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

/** Push log indentation at level 1. */
#define ecs_log_push_1() ecs_log_push_(1);
/** Push log indentation at level 2. */
#define ecs_log_push_2() ecs_log_push_(2);
/** Push log indentation at level 3. */
#define ecs_log_push_3() ecs_log_push_(3);

/** Pop log indentation at level 1. */
#define ecs_log_pop_1() ecs_log_pop_(1);
/** Pop log indentation at level 2. */
#define ecs_log_pop_2() ecs_log_pop_(2);
/** Pop log indentation at level 3. */
#define ecs_log_pop_3() ecs_log_pop_(3);

/** Test if level 1 logging is enabled. */
#define ecs_should_log_1() ecs_should_log(1)
/** Test if level 2 logging is enabled. */
#define ecs_should_log_2() ecs_should_log(2)
/** Test if level 3 logging is enabled. */
#define ecs_should_log_3() ecs_should_log(3)

#define FLECS_LOG_2
#define FLECS_LOG_1
#define FLECS_LOG_0

#elif defined(FLECS_LOG_2) /* Level 2 and below debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...)

#define ecs_log_push_1() ecs_log_push_(1);
#define ecs_log_push_2() ecs_log_push_(2);
#define ecs_log_push_3()

#define ecs_log_pop_1() ecs_log_pop_(1);
#define ecs_log_pop_2() ecs_log_pop_(2);
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

#define ecs_log_push_1() ecs_log_push_(1);
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1() ecs_log_pop_(1);
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

/** Default debug tracing is at level 1. */
#define ecs_dbg ecs_dbg_1

/** Push log indentation at the default level. */
#define ecs_log_push() ecs_log_push_(0)
/** Pop log indentation at the default level. */
#define ecs_log_pop() ecs_log_pop_(0)

/** Abort.
 * Unconditionally aborts process. */
#define ecs_abort(error_code, ...)\
    ecs_abort_(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    ecs_os_abort(); abort(); /* satisfy compiler/static analyzers */

/** Assert.
 * Aborts if condition is false, disabled in debug mode. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */
#endif // FLECS_NDEBUG

/** Debug assert.
 * Assert that is only valid in debug mode (ignores FLECS_KEEP_ASSERT) */
#ifndef FLECS_NDEBUG
#define ecs_dbg_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_dbg_assert(condition, error_code, ...)
#endif

/** Sanitize assert.
 * Assert that is only valid in sanitized mode (ignores FLECS_KEEP_ASSERT) */
#ifdef FLECS_SANITIZE
#define ecs_san_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_san_assert(condition, error_code, ...)
#endif


/** Silence dead code/unused label warnings when compiling without checks. */
#define ecs_dummy_check\
    if ((false)) {\
        goto error;\
    }

/** Check.
 * goto error if condition is false. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_check(condition, error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        goto error;\
    }\
    ecs_dummy_check
#else // FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!(condition)) {\
        ecs_assert_log_(error_code, #condition, __FILE__, __LINE__, __VA_ARGS__);\
        ecs_os_abort();\
    }\
    assert(condition); /* satisfy compiler/static analyzers */ \
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Panic.
 * goto error when FLECS_SOFT_ASSERT is defined, otherwise abort */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_throw(error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_throw(error_code, ...)\
    ecs_abort_(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    goto error;
#else
#define ecs_throw(error_code, ...)\
    ecs_abort(error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Parser error */
#define ecs_parser_error(name, expr, column, ...)\
    ecs_parser_error_(name, expr, column, __VA_ARGS__)

/** Parser error with va_list. */
#define ecs_parser_errorv(name, expr, column, fmt, args)\
    ecs_parser_errorv_(name, expr, column, fmt, args)

/** Parser warning. */
#define ecs_parser_warning(name, expr, column, ...)\
    ecs_parser_warning_(name, expr, column, __VA_ARGS__)

/** Parser warning with va_list. */
#define ecs_parser_warningv(name, expr, column, fmt, args)\
    ecs_parser_warningv_(name, expr, column, fmt, args)

#endif // FLECS_LEGACY

////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable log.
 * This will enable builtin log. For log to work, it will have to be
 * compiled in which requires defining one of the following macros:
 *
 * FLECS_LOG_0 - All log is disabled
 * FLECS_LOG_1 - Enable log level 1
 * FLECS_LOG_2 - Enable log level 2 and below
 * FLECS_LOG_3 - Enable log level 3 and below
 *
 * If no log level is defined and this is a debug build, FLECS_LOG_3 will
 * have been automatically defined.
 *
 * The provided level corresponds with the log level. If -1 is provided as
 * value, warnings are disabled. If -2 is provided, errors are disabled as well.
 *
 * @param level Desired tracing level.
 * @return Previous log level.
 */
FLECS_API
int ecs_log_set_level(
    int level);

/** Get current log level.
 *
 * @return Previous log level.
 */
FLECS_API
int ecs_log_get_level(void);

/** Enable/disable tracing with colors.
 * By default colors are enabled.
 *
 * @param enabled Whether to enable tracing with colors.
 * @return Previous color setting.
 */
FLECS_API
bool ecs_log_enable_colors(
    bool enabled);

/** Enable/disable logging timestamp.
 * By default timestamps are disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 *
 * @param enabled Whether to enable tracing with timestamps.
 * @return Previous timestamp setting.
 */
FLECS_API
bool ecs_log_enable_timestamp(
    bool enabled);

/** Enable/disable logging time since last log.
 * By default deltatime is disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 *
 * When enabled, this logs the amount of time in seconds passed since the last
 * log, when this amount is non-zero. The format is a '+' character followed by
 * the number of seconds:
 *
 *     +1 trace: log message
 *
 * @param enabled Whether to enable tracing with timestamps.
 * @return Previous timestamp setting.
 */
FLECS_API
bool ecs_log_enable_timedelta(
    bool enabled);

/** Get last logged error code.
 * Calling this operation resets the error code.
 *
 * @return Last error, 0 if none was logged since last call to last_error.
 */
FLECS_API
int ecs_log_last_error(void);

/** Start capturing log output.
 *
 * @param capture_try If true, also capture messages from ecs_log_try blocks.
 */
FLECS_API
void ecs_log_start_capture(bool capture_try);

/** Stop capturing log output.
 *
 * @return The captured log output, or NULL if no output was captured.
 */
FLECS_API
char* ecs_log_stop_capture(void);

////////////////////////////////////////////////////////////////////////////////
//// Error codes
////////////////////////////////////////////////////////////////////////////////

/** Invalid operation error code. */
#define ECS_INVALID_OPERATION (1)
/** Invalid parameter error code. */
#define ECS_INVALID_PARAMETER (2)
/** Constraint violated error code. */
#define ECS_CONSTRAINT_VIOLATED (3)
/** Out of memory error code. */
#define ECS_OUT_OF_MEMORY (4)
/** Out of range error code. */
#define ECS_OUT_OF_RANGE (5)
/** Unsupported error code. */
#define ECS_UNSUPPORTED (6)
/** Internal error code. */
#define ECS_INTERNAL_ERROR (7)
/** Already defined error code. */
#define ECS_ALREADY_DEFINED (8)
/** Missing OS API error code. */
#define ECS_MISSING_OS_API (9)
/** Operation failed error code. */
#define ECS_OPERATION_FAILED (10)
/** Invalid conversion error code. */
#define ECS_INVALID_CONVERSION (11)
/** Cycle detected error code. */
#define ECS_CYCLE_DETECTED (13)
/** Leak detected error code. */
#define ECS_LEAK_DETECTED (14)
/** Double free error code. */
#define ECS_DOUBLE_FREE (15)

/** Inconsistent name error code. */
#define ECS_INCONSISTENT_NAME (20)
/** Name in use error code. */
#define ECS_NAME_IN_USE (21)
/** Invalid component size error code. */
#define ECS_INVALID_COMPONENT_SIZE (23)
/** Invalid component alignment error code. */
#define ECS_INVALID_COMPONENT_ALIGNMENT (24)
/** Component not registered error code. */
#define ECS_COMPONENT_NOT_REGISTERED (25)
/** Inconsistent component id error code. */
#define ECS_INCONSISTENT_COMPONENT_ID (26)
/** Inconsistent component action error code. */
#define ECS_INCONSISTENT_COMPONENT_ACTION (27)
/** Module undefined error code. */
#define ECS_MODULE_UNDEFINED (28)
/** Missing symbol error code. */
#define ECS_MISSING_SYMBOL (29)
/** Already in use error code. */
#define ECS_ALREADY_IN_USE (30)

/** Access violation error code. */
#define ECS_ACCESS_VIOLATION (40)
/** Column index out of range error code. */
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
/** Column is not shared error code. */
#define ECS_COLUMN_IS_NOT_SHARED (42)
/** Column is shared error code. */
#define ECS_COLUMN_IS_SHARED (43)
/** Column type mismatch error code. */
#define ECS_COLUMN_TYPE_MISMATCH (45)

/** Invalid while readonly error code. */
#define ECS_INVALID_WHILE_READONLY (70)
/** Locked storage error code. */
#define ECS_LOCKED_STORAGE (71)
/** Invalid from worker error code. */
#define ECS_INVALID_FROM_WORKER (72)


////////////////////////////////////////////////////////////////////////////////
//// Used when logging with colors is enabled
////////////////////////////////////////////////////////////////////////////////

/** Black ANSI color escape code. */
#define ECS_BLACK   "\033[1;30m"
/** Red ANSI color escape code. */
#define ECS_RED     "\033[0;31m"
/** Green ANSI color escape code. */
#define ECS_GREEN   "\033[0;32m"
/** Yellow ANSI color escape code. */
#define ECS_YELLOW  "\033[0;33m"
/** Blue ANSI color escape code. */
#define ECS_BLUE    "\033[0;34m"
/** Magenta ANSI color escape code. */
#define ECS_MAGENTA "\033[0;35m"
/** Cyan ANSI color escape code. */
#define ECS_CYAN    "\033[0;36m"
/** White ANSI color escape code. */
#define ECS_WHITE   "\033[1;37m"
/** Grey ANSI color escape code. */
#define ECS_GREY    "\033[0;37m"
/** Normal ANSI color escape code. */
#define ECS_NORMAL  "\033[0;49m"
/** Bold ANSI escape code. */
#define ECS_BOLD    "\033[1;49m"

#ifdef __cplusplus
}
#endif

/** @} */

#endif // FLECS_LOG_H
