


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

namespace flecs 
{

namespace _ 
{

template <typename Func, typename ... Components>
class each_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit each_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit each_invoker(const Func& func) noexcept : m_func(func) { }

    // Invoke system
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        flecs::iter iter_wrapper(iter);
        (void)index;
        (void)columns;

        // Use any_column so we can transparently use shared components
        for (auto row : iter_wrapper) {
            func(iter_wrapper.entity(row), (_::any_column<typename std::remove_reference<Components>::type>(
                 (typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr, 
                    static_cast<size_t>(iter->count), comps.is_shared))[row]...);
        }
    }

    // Add components one by one to parameter pack
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        each_invoker::call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    // Callback provided to flecs system
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        each_invoker *self = (each_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class action_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit action_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit action_invoker(const Func& func) noexcept : m_func(func) { }

    /* Invoke system */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, int index, Columns& columns, Targs... comps) {
        (void)index;
        (void)columns;

        flecs::iter iter_wrapper(iter);
        
        func(iter_wrapper, (column<typename std::remove_reference< typename std::remove_pointer<Components>::type >::type>(
            (typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr, iter->count, comps.is_shared))...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, int index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        action_invoker *self = (action_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class iter_invoker {
    using Columns = typename column_args<Components ...>::Columns;

public:
    explicit iter_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit iter_invoker(const Func& func) noexcept : m_func(func) { }

    /* Invoke system */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        (void)index;
        (void)columns;
        flecs::iter iter_wrapper(iter);
        func(iter_wrapper, ((typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*)comps.ptr)...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const Context *ctx = ecs_get(iter->world, iter->system, EcsContext);
        iter_invoker *self = (iter_invoker*)ctx->ctx;
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};

} // namespace _

} // namespace flecs
