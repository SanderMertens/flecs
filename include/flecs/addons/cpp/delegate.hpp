/**
 * @file addons/cpp/delegate.hpp
 * @brief Wrappers around C++ functions that provide callbacks for C APIs.
 */

#pragma once

#include <utility> // std::declval

namespace flecs
{

namespace _ 
{

struct component_binding_ctx {
    enum Hook { OnAdd, OnRemove, OnSet, OnReplace, OnValidate, Count };
    void *callbacks[Count] = {};
    ecs_ctx_free_t free[Count] = {};

    ~component_binding_ctx() {
        for (size_t i = 0; i < Count; i ++) {
            if (callbacks[i] && free[i]) {
                free[i](callbacks[i]);
            }
        }
    }
};

// Utility to convert a template argument pack to an array of term pointers.
struct field_ptr {
    void *ptr = nullptr;
    int8_t index = 0;
    bool is_ref = false;
    bool is_row = false;
};

template <typename ... Components>
struct field_ptrs {
    using array = flecs::array<_::field_ptr, sizeof...(Components)>;

    void populate(const ecs_iter_t *iter) {
        populate_impl(iter, std::index_sequence_for<Components...>{});
    }

    void populate_self(const ecs_iter_t *iter) {
        populate_self_impl(iter, std::index_sequence_for<Components...>{});
    }

    array fields_;

private:
    template <typename T>
    void populate_field(const ecs_iter_t *iter, size_t index) {
        using A = remove_pointer_t<actual_type_t<T>>;
        if constexpr (!is_empty_v<A>) {
            if (iter->row_fields & (1llu << index)) {
                /* Need to fetch the value with ecs_field_at() */
                fields_[index].is_row = true;
                fields_[index].is_ref = true;
                fields_[index].index = static_cast<int8_t>(index);
            } else {
                fields_[index].ptr = ecs_field_w_size(iter, sizeof(A), 
                    static_cast<int8_t>(index));
                fields_[index].is_ref = iter->sources[index] != 0;
            }
        }
    }

    template <typename T>
    void populate_self_field(const ecs_iter_t *iter, size_t index) {
        (void)iter; (void)index;

        using A = remove_pointer_t<actual_type_t<T>>;
        if constexpr (!is_empty_v<A>) {
            fields_[index].ptr = ecs_field_w_size(iter, sizeof(A), 
                static_cast<int8_t>(index));
            fields_[index].is_ref = false;
        }
    }

    template <size_t... Is>
    void populate_impl(const ecs_iter_t *iter, std::index_sequence<Is...>) {
        (void)iter;
        (populate_field<Components>(iter, Is), ...);
    }

    template <size_t... Is>
    void populate_self_impl(const ecs_iter_t *iter, std::index_sequence<Is...>) {
        (void)iter;
        (populate_self_field<Components>(iter, Is), ...);
    }
};

struct delegate { };

template <typename T, bool Ref = false>
struct each_field {
    const flecs::iter_t *iter_;
    _::field_ptr& field_;
    size_t row_;

    each_field(const flecs::iter_t *iter, _::field_ptr& field, size_t row)
        : iter_(iter), field_(field), row_(row) { }

    decltype(auto) get_row() {
        using A = actual_type_t<T>;
        if constexpr (is_empty<A>::value && !is_pointer<T>::value) {
            return T(A());
        } else {
            size_t row = row_;
            if constexpr (Ref) {
                if (field_.is_ref) {
                    row = 0;
                }
                if (field_.is_row) {
                    field_.ptr = ecs_field_at_w_size(iter_,
                        sizeof(remove_pointer_t<A>), field_.index,
                        static_cast<int32_t>(row_));
                }
            }
            if constexpr (is_pointer<T>::value) {
                return field_.ptr ? &static_cast<A>(field_.ptr)[row] : nullptr;
            } else if constexpr (is_actual<T>::value) {
                return static_cast<T*>(field_.ptr)[row];
            } else {
                return T(static_cast<A*>(field_.ptr)[row]);
            }
        }
    }
};

// Type that handles passing components to each callbacks.
template <typename Func, typename ... Components>
struct each_delegate : public delegate {
    using Terms = typename field_ptrs<Components ...>::array;

    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit each_delegate(Func&& func) noexcept 
        : func_(FLECS_MOV(func)) { }

    explicit each_delegate(const Func& func) noexcept 
        : func_(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        invoke_until<false>(iter);
    }

    // Static function that can be used as callback for systems/observers.
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const each_delegate*>(iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        self->invoke(iter);
    }

    // Static function that can be used as callback for systems/observers.
    // Different from run() in that it loops the iterator.
    static void run_each(ecs_iter_t *iter) {
        auto self = static_cast<const each_delegate*>(iter->run_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        while (iter->next(iter)) {
            self->invoke(iter);
        }
    }

    // Create instance of delegate.
    static each_delegate* make(const Func& func) {
        return FLECS_NEW(each_delegate)(func);
    }

    // Function that can be used as callback to free delegate.
    static void destruct(void *obj) {
        _::free_obj<each_delegate>(obj);
    }

    template <component_binding_ctx::Hook Hook>
    static void run_hook(ecs_iter_t *iter) {
        auto ctx = static_cast<component_binding_ctx*>(iter->callback_ctx);
        auto self = static_cast<const each_delegate*>(ctx->callbacks[Hook]);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        iter->callback_ctx = ctx->callbacks[Hook];
        self->template invoke_until<false,
            Hook != component_binding_ctx::OnReplace>(iter);
    }

protected:
    template <bool Find, bool Shared = true>
    flecs::entity invoke_until(ecs_iter_t *iter) const {
        field_ptrs<Components...> terms;
        iter->flags |= EcsIterCppEach;
        if (Shared && (iter->ref_fields | iter->up_fields)) {
            terms.populate(iter);
            return invoke_rows<Find, true>(iter, terms.fields_,
                std::index_sequence_for<Components...>{});
        } else {
            terms.populate_self(iter);
            return invoke_rows<Find, false>(iter, terms.fields_,
                std::index_sequence_for<Components...>{});
        }
    }

private:
    template <typename... Args>
    static decltype(auto) invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t i, Args&&... args)
    {
        if constexpr (std::is_invocable_v<const Func&, flecs::entity, Args...>) {
            ecs_assert(iter->entities != nullptr, ECS_INVALID_PARAMETER,
                "query does not return entities ($this variable is not populated)");
            return func(flecs::entity(iter->world, iter->entities[i]),
                FLECS_FWD(args)...);
        } else if constexpr (std::is_invocable_v<
            const Func&, flecs::iter&, size_t&, Args...>)
        {
            flecs::iter it(iter);
            return func(it, i, FLECS_FWD(args)...);
        } else {
            return func(FLECS_FWD(args)...);
        }
    }

    template <bool Find, bool Ref, size_t... I>
    flecs::entity invoke_rows(ecs_iter_t *iter, Terms& terms,
        std::index_sequence<I...>) const
    {
        ECS_TABLE_LOCK(iter->world, iter->table);
        size_t count = static_cast<size_t>(iter->count);
        if constexpr (Find) {
            if constexpr (!std::is_invocable_v<const Func&, flecs::entity,
                decltype(std::declval<each_field<
                    remove_reference_t<Components>, Ref>>().get_row())...>)
            {
                if (!count) {
                    count = 1;
                }
            }
        } else if (!count && !iter->table) {
            count = 1;
        }

        flecs::entity result;
        for (size_t i = 0; i < count; i ++) {
            if constexpr (Find) {
                if (invoke_callback(iter, func_, i,
                    each_field<remove_reference_t<Components>, Ref>(
                        iter, terms[I], i).get_row()...))
                {
                    result = flecs::entity(iter->world, iter->entities[i]);
                    break;
                }
            } else {
                invoke_callback(iter, func_, i,
                    each_field<remove_reference_t<Components>, Ref>(
                        iter, terms[I], i).get_row()...);
            }
        }
        ECS_TABLE_UNLOCK(iter->world, iter->table);
        return result;
    }

public:
    Func func_;
};

template <typename Func, typename T>
struct validate_delegate : public delegate {
    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit validate_delegate(Func&& func) noexcept
        : func_(FLECS_MOV(func)) { }

    explicit validate_delegate(const Func& func) noexcept
        : func_(func) { }

    static bool run(ecs_world_t *world, ecs_entity_t entity, void *ptr) {
        const ecs_type_hooks_t *h = ecs_get_hooks_id(
            world, _::type<T>::id(world));
        ecs_assert(h != nullptr, ECS_INTERNAL_ERROR, nullptr);
        auto ctx = static_cast<component_binding_ctx*>(h->binding_ctx);
        ecs_assert(ctx != nullptr, ECS_INTERNAL_ERROR, nullptr);
        auto self = static_cast<const validate_delegate*>(ctx->callbacks[component_binding_ctx::OnValidate]);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        return self->func_(
            flecs::entity(world, entity), *static_cast<T*>(ptr));
    }

    Func func_;
};

template <typename Func, typename ... Components>
struct find_delegate : each_delegate<Func, Components...> {
    using each_delegate<Func, Components...>::each_delegate;

    flecs::entity invoke(ecs_iter_t *iter) const {
        return this->template invoke_until<true>(iter);
    }
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func>
struct run_delegate : delegate {
    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit run_delegate(Func&& func) noexcept 
        : func_(FLECS_MOV(func)) { }

    explicit run_delegate(const Func& func) noexcept 
        : func_(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        flecs::iter it(iter);
        iter->flags &= ~EcsIterIsValid;
        func_(it);
    }

    // Static function that can be used as callback for systems/observers.
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const run_delegate*>(iter->run_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        self->invoke(iter);
    }

    Func func_;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke an entity observer delegate
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename Event = void>
struct entity_observer_delegate : delegate {
    explicit entity_observer_delegate(Func&& func) noexcept
        : func_(FLECS_FWD(func)) { }

    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const entity_observer_delegate*>(iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, nullptr);
        if constexpr (std::is_void_v<Event> || is_empty_v<Event>) {
            self->invoke(iter);
        } else {
            ecs_assert(iter->param != nullptr, ECS_INVALID_OPERATION,
                "entity observer invoked without payload");
            self->invoke(iter, *static_cast<Event*>(iter->param));
        }
    }

private:
    template <typename... Args>
    void invoke(ecs_iter_t *iter, Args&... args) const {
        if constexpr (std::is_invocable_v<const Func&, flecs::entity, Args&...>) {
            func_(flecs::entity(iter->world, ecs_field_src(iter, 0)), args...);
        } else {
            func_(args...);
        }
    }

    Func func_;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility to invoke callback on entity if it has components in signature
////////////////////////////////////////////////////////////////////////////////

template <typename ArgList>
struct entity_with_delegate_impl;

template <typename... Args>
struct entity_with_delegate_impl<arg_list<Args...>> {
    using ArrayType = flecs::array<void*, sizeof...(Args)>;
    using IdArray = flecs::array<id_t, sizeof...(Args)>;

    static bool get_ptrs(world_t *world, entity_t entity, const ecs_record_t *record,
        ecs_table_t *table, IdArray& ids, ArrayType& ptrs)
    {
        ecs_assert(table != nullptr, ECS_INTERNAL_ERROR, nullptr);
        const world_t *real_world = ecs_get_world(world);
        size_t i = 0;
        for (auto id : ids) {
            int32_t column = ecs_table_get_column_index(real_world, table, id);
            void *ptr = column == -1 ? ecs_get_mut_id(world, entity, id) :
                ecs_record_get_by_column(record, column, 0);
            if (!ptr) {
                return false;
            }
            ptrs[i ++] = ptr;
        }
        return true;
    }

    template <typename Func>
    static bool invoke_get(world_t *world, entity_t entity, const Func& func) {
        ecs_record_t *record = ecs_record_find(world, entity);
        if (!record || !record->table) {
            return false;
        }
        auto table = record->table;
        ECS_TABLE_LOCK(world, table);
        IdArray ids ({ _::type<Args>::id(world)... });
        ArrayType ptrs;
        bool found = get_ptrs(world, entity, record, table, ids, ptrs);
        if (found) {
            invoke_callback(func, ptrs, std::index_sequence_for<Args...>{});
        }
        ECS_TABLE_UNLOCK(world, table);
        return found;
    }

    template <typename Func>
    static bool invoke_ensure(world_t *world, entity_t entity, const Func& func) {
        IdArray ids ({ _::type<Args>::id(world)... });
        ArrayType ptrs;
        ecs_table_t *table = nullptr;
        if (!ecs_is_deferred(world)) {
            ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INVALID_PARAMETER, nullptr);
            ecs_record_t *record = ecs_record_find(world, entity);
            ecs_assert(record != nullptr, ECS_INVALID_PARAMETER, nullptr);
            flecs_add_ids(world, entity, ids.ptr(), static_cast<int32_t>(sizeof...(Args)));
            table = record->table;
            if (!get_ptrs(world, entity, record, table, ids, ptrs)) {
                ecs_abort(ECS_INTERNAL_ERROR, nullptr);
            }
            ECS_TABLE_LOCK(world, table);
        } else {
            size_t i = 0;
            ((ptrs[i] = ecs_ensure_id(world, entity, ids[i], sizeof(Args)), i ++), ...);
        }
        invoke_callback(func, ptrs, std::index_sequence_for<Args...>{});
        if (table) {
            ECS_TABLE_UNLOCK(world, table);
        }
        for (auto id : ids) {
            ecs_modified_id(world, entity, id);
        }
        return true;
    }

private:
    template <typename Func, size_t... I>
    static void invoke_callback(const Func& func, ArrayType& ptrs, std::index_sequence<I...>) {
        func(*static_cast<base_arg_type_t<Args>*>(ptrs[I])...);
    }
};

template <typename Func, typename = int>
struct entity_with_delegate : entity_with_delegate_impl<arg_list_t<Func>> {
    static_assert(arity<Func>::value > 0, "function must have at least one argument");
};

/** Strip references from each-callback argument types. */
template <typename ArgList>
struct each_normalize_args;

template <typename ... Args>
struct each_normalize_args<arg_list<Args...>> {
    using type = arg_list<remove_reference_t<Args>...>;
};

/** Extract the component argument list from an each-callback signature.
 * Skips a leading flecs::entity or flecs::iter argument when present. */
template <typename ArgList, typename = int>
struct each_callback_args {
    using type = typename each_normalize_args<ArgList>::type;
};

template <typename First, typename ... Args>
struct each_callback_args<arg_list<First, Args...>,
    if_t<is_same<decay_t<First>, flecs::entity>::value>> {
    using type = typename each_normalize_args<arg_list<Args...>>::type;
};

template <typename First, typename Second, typename ... Args>
struct each_callback_args<arg_list<First, Second, Args...>,
    if_t<is_same<decay_t<First>, flecs::iter>::value>> {
    using type = typename each_normalize_args<arg_list<Args...>>::type;
};

} // namespace _

/** Delegate type for each callbacks.
 * Experimental: allows using the each delegate for use cases outside of Flecs.
 *
 * @tparam Func The callback function type.
 * @tparam Args The component argument types.
 */
template <typename Func, typename ... Args>
using delegate = _::each_delegate<typename std::decay<Func>::type, Args...>;

} // namespace flecs
