/**
 * @file addons/cpp/log.hpp
 * @brief Logging functions.
 */

#pragma once

namespace flecs {
namespace log {

/**
 * @defgroup cpp_log Logging
 * @brief Logging functions.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Set log level */
inline void set_level(int level) {
    ecs_log_set_level(level);
}

/** Enable colors in logging */
inline void enable_colors(bool enabled = true) {
    ecs_log_enable_colors(enabled);
}

/** Enable timestamps in logging */
inline void enable_timestamp(bool enabled = true) {
    ecs_log_enable_timestamp(enabled);
}

/** Enable time delta in logging */
inline void enable_timedelta(bool enabled = true) {
    ecs_log_enable_timedelta(enabled);
}

/** Debug trace (level 1) */
inline void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(1, fmt, args);
    va_end(args);
}

/** Trace (level 0) */
inline void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
}

/** Trace (level -2) */
inline void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-2, fmt, args);
    va_end(args);
}

/** Trace (level -3) */
inline void err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-3, fmt, args);
    va_end(args);
}

/** Increase log indentation */
inline void push(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
    ecs_log_push();
}

/** Increase log indentation */
inline void push() {
    ecs_log_push();
}

/** Increase log indentation */
inline void pop() {
    ecs_log_pop();
}

/** @} */

}
}
