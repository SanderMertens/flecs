/**
 * @file addons/cpp/log.hpp
 * @brief Logging functions.
 */

#pragma once

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L))
#if defined(__has_include)
#if __has_include(<source_location>)
#include <source_location>
#if defined(__cpp_lib_source_location) && (__cpp_lib_source_location >= 201907L)
#define FLECS_CPP_SOURCE_LOCATION
#endif
#endif
#endif
#endif

namespace flecs {
namespace log {

namespace _ {

struct fmt_str {
#ifdef FLECS_CPP_SOURCE_LOCATION
    fmt_str(const char *fmt_,
        std::source_location loc = std::source_location::current())
        : fmt(fmt_)
        , file(loc.file_name())
        , line(static_cast<int32_t>(loc.line())) { }
#else
    fmt_str(const char *fmt_)
        : fmt(fmt_)
        , file(__FILE__)
        , line(__LINE__) { }
#endif

    const char *fmt;
    const char *file;
    int32_t line;
};

}

/**
 * @defgroup cpp_log Logging
 * @ingroup cpp_addons
 * Logging functions.
 *
 * @{
 */

/** Set the log level.
 *
 * @param level The log level to set.
 */
inline void set_level(int level) {
    ecs_log_set_level(level);
}

/** Get the log level.
 *
 * @return The current log level.
 */
inline int get_level() {
    return ecs_log_get_level();
}

/** Enable colors in logging.
 *
 * @param enabled Whether to enable colors (default true).
 */
inline void enable_colors(bool enabled = true) {
    ecs_log_enable_colors(enabled);
}

/** Enable timestamps in logging.
 *
 * @param enabled Whether to enable timestamps (default true).
 */
inline void enable_timestamp(bool enabled = true) {
    ecs_log_enable_timestamp(enabled);
}

/** Enable time delta in logging.
 *
 * @param enabled Whether to enable time delta (default true).
 */
inline void enable_timedelta(bool enabled = true) {
    ecs_log_enable_timedelta(enabled);
}

/** Debug trace (level 1).
 *
 * @param fmt The format string.
 */
inline void dbg(_::fmt_str fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv_(1, fmt.file, fmt.line, fmt.fmt, args);
    va_end(args);
}

/** Trace (level 0).
 *
 * @param fmt The format string.
 */
inline void trace(_::fmt_str fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv_(0, fmt.file, fmt.line, fmt.fmt, args);
    va_end(args);
}

/** Warning (level -2).
 *
 * @param fmt The format string.
 */
inline void warn(_::fmt_str fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv_(-2, fmt.file, fmt.line, fmt.fmt, args);
    va_end(args);
}

/** Error (level -3).
 *
 * @param fmt The format string.
 */
inline void err(_::fmt_str fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv_(-3, fmt.file, fmt.line, fmt.fmt, args);
    va_end(args);
}

/** Trace and increase log indentation.
 *
 * @param fmt The format string.
 */
inline void push(_::fmt_str fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv_(0, fmt.file, fmt.line, fmt.fmt, args);
    va_end(args);
    ecs_log_push();
}

/** Increase log indentation. */
inline void push() {
    ecs_log_push();
}

/** Decrease log indentation. */
inline void pop() {
    ecs_log_pop();
}

/** @} */

}
}
