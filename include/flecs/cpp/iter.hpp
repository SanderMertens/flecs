


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

namespace flecs 
{

namespace _ 
{
class invoker { };

struct SystemCppContext {
public:
    SystemCppContext() : m_ctx(nullptr) { }

    SystemCppContext(invoker *ctx) : m_ctx(ctx) { }    

    ~SystemCppContext() {
        FLECS_DELETE(m_ctx);
    }

    SystemCppContext& operator=(const SystemCppContext& obj) {
        ecs_os_abort(); // should never be copied
        this->m_ctx = obj.m_ctx;
        return *this;
    }

    SystemCppContext& operator=(SystemCppContext&& obj) {
        this->m_ctx = obj.m_ctx;
        obj.m_ctx = nullptr;
        return *this;
    }

    invoker* ctx() const {
        return m_ctx;
    }

private:
    invoker *m_ctx;
};

template <typename Func, typename ... Components>
class each_invoker : public invoker {
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
                 static_cast<typename std::remove_reference< typename std::remove_pointer<Components>::type >::type*>(comps.ptr), 
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
        const SystemCppContext *ctx = static_cast<const SystemCppContext*>(
            ecs_get_w_id(iter->world, iter->system, 
                _::cpp_type<SystemCppContext>().id()));
                
        const each_invoker *self = static_cast<const each_invoker*>(ctx->ctx());
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
class action_invoker : public invoker {
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
            static_cast<typename std::remove_reference< 
                typename std::remove_pointer<Components>::type >::type*>(comps.ptr), 
                    iter->count, comps.is_shared))...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, int index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const SystemCppContext *ctx = static_cast<const SystemCppContext*>(
            ecs_get_w_id(iter->world, iter->system, 
                _::cpp_type<SystemCppContext>().id()));
                
        const action_invoker *self = static_cast<const action_invoker*>(ctx->ctx());
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
class iter_invoker : public invoker {
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
        func(iter_wrapper, (
            static_cast<typename std::remove_reference< 
                typename std::remove_pointer<Components>::type >::type*>(comps.ptr))...);
    }

    /** Add components one by one to parameter pack */
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void call_system(ecs_iter_t *iter, const Func& func, size_t index, Columns& columns, Targs... comps) {
        call_system(iter, func, index + 1, columns, comps..., columns[index]);
    }

    /** Callback provided to flecs */
    static void run(ecs_iter_t *iter) {
        const SystemCppContext *ctx = static_cast<const SystemCppContext*>(
            ecs_get_w_id(iter->world, iter->system, 
                _::cpp_type<SystemCppContext>().id()));

        const iter_invoker *self = static_cast<const iter_invoker*>(ctx->ctx());
        column_args<Components...> columns(iter);
        call_system(iter, self->m_func, 0, columns.m_columns);
    }

private:
    Func m_func;
};

} // namespace _

} // namespace flecs
