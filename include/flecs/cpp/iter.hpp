
////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

namespace flecs 
{

namespace _ 
{
class invoker { };

template <typename T, typename = void>
struct each_column { };

struct each_column_base {
    each_column_base(const _::TermPtr& term, int32_t row) : m_term(term), m_row(row) { }
protected:
    const _::TermPtr& m_term;
    int32_t m_row;    
};

template <typename T>
struct each_column<T, typename std::enable_if<std::is_pointer<T>::value == false>::type> : public each_column_base {
    each_column(const _::TermPtr& term, int32_t row) : each_column_base(term, row) { }
    T& get_row() {
        return static_cast<T*>(this->m_term.ptr)[this->m_row];
    }
};

template <typename T>
struct each_column<T, typename std::enable_if<std::is_pointer<T>::value == true>::type> : public each_column_base {
    each_column(const _::TermPtr& term, int32_t row) : each_column_base(term, row) { }
    T get_row() {
        if (this->m_term.ptr) {
            return &static_cast<T>(this->m_term.ptr)[this->m_row];
        } else {
            return nullptr;
        }
    }
};

template <typename T, typename = void>
struct each_ref_column : public each_column<T> {
    each_ref_column(const _::TermPtr& term, int32_t row) : each_column<T>(term, row) {
        if (term.is_ref) {
            this->m_row = 0;
        }
    }
};

template <typename Func, typename ... Components>
class each_invoker : public invoker {
    using Terms = typename term_ptrs<Components ...>::Terms;

public:
    explicit each_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit each_invoker(const Func& func) noexcept : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;

        if (terms.populate_w_refs(iter)) {
            invoke_callback<each_ref_column>(iter, m_func, 0, terms.m_terms);
        } else {
            invoke_callback<each_column>(iter, m_func, 0, terms.m_terms);
        }   
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const each_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

private:
    template <template<typename Ta, typename = void> class ColumnType, typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t index, Terms& columns, Targs... comps) {
        (void)index;
        (void)columns;
        flecs::iter it(iter);
        for (auto row : it) {
            func(it.entity(row),
                (ColumnType<typename std::remove_reference<Components>::type>(comps, row)
                    .get_row())...);
        }
    }

    template <template<typename Ta, typename = void> class ColumnType, typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t index, Terms& columns, Targs... comps) {
        each_invoker::invoke_callback<ColumnType>(iter, func, index + 1, columns, comps..., columns[index]);
    }    

    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class action_invoker : public invoker {
    using Terms = typename term_ptrs<Components ...>::Terms;
public:
    explicit action_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit action_invoker(const Func& func) noexcept : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;
        terms.populate_w_refs(iter);
        invoke_callback(iter, m_func, 0, terms.m_terms);
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const action_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

private:
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, int index, Terms& columns, Targs... comps) {
        (void)index;
        (void)columns;
        flecs::iter iter_wrapper(iter);
        func(iter_wrapper, (column<typename std::remove_reference<
            typename std::remove_pointer<Components>::type >::type>(
                static_cast<typename std::remove_reference< 
                    typename std::remove_pointer<Components>::type >::type*>(comps.ptr), 
                        iter->count, comps.is_ref))...);
    }

    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, int index, Terms& columns, Targs... comps) {
        invoke_callback(iter, func, index + 1, columns, comps..., columns[index]);
    }

    Func m_func;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class iter_invoker : public invoker {
    using Terms = typename term_ptrs<Components ...>::Terms;

public:
    explicit iter_invoker(Func&& func) noexcept : m_func(std::move(func)) { }
    explicit iter_invoker(const Func& func) noexcept : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;
        terms.populate(iter);
        invoke_callback(iter, m_func, 0, terms.m_terms);
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const iter_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

private:
    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) == sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t index, Terms& columns, Targs... comps) {
        (void)index;
        (void)columns;
        flecs::iter iter_wrapper(iter);
        func(iter_wrapper, (
            static_cast<typename std::remove_reference< 
                typename std::remove_pointer<Components>::type >::type*>(comps.ptr))...);
    }

    template <typename... Targs,
        typename std::enable_if<sizeof...(Targs) != sizeof...(Components), void>::type* = nullptr>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t index, Terms& columns, Targs... comps) {
        invoke_callback(iter, func, index + 1, columns, comps..., columns[index]);
    }

    Func m_func;
};

} // namespace _

} // namespace flecs
