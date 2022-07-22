
namespace flecs {
namespace log {

inline void set_level(int level) {
    ecs_log_set_level(level);
}

inline void enable_colors(bool enabled = true) {
    ecs_log_enable_colors(enabled);
}

inline void enable_timestamp(bool enabled = true) {
    ecs_log_enable_timestamp(enabled);
}

inline void enable_timedelta(bool enabled = true) {
    ecs_log_enable_timedelta(enabled);
}

inline void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(1, fmt, args);
    va_end(args);
}

inline void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
}

inline void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-2, fmt, args);
    va_end(args);
}

inline void err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-3, fmt, args);
    va_end(args);
}

inline void push(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
    ecs_log_push();
}

inline void push() {
    ecs_log_push();
}

inline void pop() {
    ecs_log_pop();
}

}
}
