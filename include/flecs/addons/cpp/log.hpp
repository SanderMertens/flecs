/**
 * @file addons/cpp/log.hpp
 * @brief Logging functions.
 */

#pragma once

namespace flecs {
namespace log {

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
inline void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(1, fmt, args);
    va_end(args);
}

/** Trace (level 0).
 *
 * @param fmt The format string.
 */
inline void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
}

/** Warning (level -2).
 *
 * @param fmt The format string.
 */
inline void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-2, fmt, args);
    va_end(args);
}

/** Error (level -3).
 *
 * @param fmt The format string.
 */
inline void err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-3, fmt, args);
    va_end(args);
}

/** Trace and increase log indentation.
 *
 * @param fmt The format string.
 */
inline void push(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
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
