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

// Binding ctx for component hooks
struct component_binding_ctx {
    void *on_add = nullptr;
    void *on_remove = nullptr;
    void *on_set = nullptr;
    ecs_ctx_free_t free_on_add = nullptr;
    ecs_ctx_free_t free_on_remove = nullptr;
    ecs_ctx_free_t free_on_set = nullptr;

    ~component_binding_ctx() {
        if (on_add && free_on_add) {
            free_on_add(on_add);
        }
        if (on_remove && free_on_remove) {
            free_on_remove(on_remove);
        }
        if (on_set && free_on_set) {
            free_on_set(on_set);
        }
    }
};

// Utility to convert template argument pack to array of term ptrs
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
        populate(iter, 0, static_cast<
            remove_reference_t<
                remove_pointer_t<Components>>
                    *>(nullptr)...);
    }

    void populate_self(const ecs_iter_t *iter) {
        populate_self(iter, 0, static_cast<
            remove_reference_t<
                remove_pointer_t<Components>>
                    *>(nullptr)...);
    }

    array fields_;

private:
    void populate(const ecs_iter_t*, size_t) { }

    template <typename T, typename... Targs, 
        typename A = remove_pointer_t<actual_type_t<T>>,
            if_not_t< is_empty<A>::value > = 0>
    void populate(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
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

        populate(iter, index + 1, comps ...);
    }

    template <typename T, typename... Targs, 
        typename A = remove_pointer_t<actual_type_t<T>>,
            if_t< is_empty<A>::value > = 0>
    void populate(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
        populate(iter, index + 1, comps ...);
    }

    void populate_self(const ecs_iter_t*, size_t) { }

    template <typename T, typename... Targs, 
        typename A = remove_pointer_t<actual_type_t<T>>,
            if_not_t< is_empty<A>::value > = 0>
    void populate_self(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
        fields_[index].ptr = ecs_field_w_size(iter, sizeof(A), 
            static_cast<int8_t>(index));
        populate_self(iter, index + 1, comps ...);
    }

    template <typename T, typename... Targs,
        typename A = remove_pointer_t<actual_type_t<T>>,
            if_t< is_empty<A>::value > = 0>
    void populate_self(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
        populate(iter, index + 1, comps ...);
    }
};

struct delegate { };

// Template that figures out from the template parameters of a query/system
// how to pass the value to the each callback
template <typename T, typename = int>
struct each_field { };

// Base class
struct each_column_base {
    each_column_base(const _::field_ptr& field, size_t row) 
        : field_(field), row_(row) { }

protected:
    const _::field_ptr& field_;
    size_t row_;    
};

// If type is not a pointer, return a reference to the type (default case)
template <typename T>
struct each_field<T, if_t< !is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value && is_actual<T>::value > > 
    : each_column_base 
{
    each_field(const flecs::iter_t*, _::field_ptr& field, size_t row) 
        : each_column_base(field, row) { }

    T& get_row() {
        return static_cast<T*>(this->field_.ptr)[this->row_];
    }  
};

// If argument type is not the same as actual component type, return by value.
// This requires that the actual type can be converted to the type.
// A typical scenario where this happens is when using flecs::pair types.
template <typename T>
struct each_field<T, if_t< !is_pointer<T>::value &&
        !is_empty<actual_type_t<T>>::value && !is_actual<T>::value> > 
    : each_column_base 
{
    each_field(const flecs::iter_t*, _::field_ptr& field, size_t row) 
        : each_column_base(field, row) { }

    T get_row() {
        return static_cast<actual_type_t<T>*>(this->field_.ptr)[this->row_];
    }  
};

// If type is empty (indicating a tag) the query will pass a nullptr. To avoid
// returning nullptr to reference arguments, return a temporary value.
template <typename T>
struct each_field<T, if_t< is_empty<actual_type_t<T>>::value && 
        !is_pointer<T>::value > > 
    : each_column_base 
{
    each_field(const flecs::iter_t*, _::field_ptr& field, size_t row) 
        : each_column_base(field, row) { }

    T get_row() {
        return actual_type_t<T>();
    }
};

// If type is a pointer (indicating an optional value) don't index with row if
// the field is not set.
template <typename T>
struct each_field<T, if_t< is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value > > 
    : each_column_base 
{
    each_field(const flecs::iter_t*, _::field_ptr& field, size_t row) 
        : each_column_base(field, row) { }

    actual_type_t<T> get_row() {
        if (this->field_.ptr) {
            return &static_cast<actual_type_t<T>>(this->field_.ptr)[this->row_];
        } else {
            // optional argument doesn't have a value
            return nullptr;
        }
    }
};

// If the query contains component references to other entities, check if the
// current argument is one.
template <typename T, typename = int>
struct each_ref_field : public each_field<T> {
    each_ref_field(const flecs::iter_t *iter, _::field_ptr& field, size_t row)
        : each_field<T>(iter, field, row) {

        if (field.is_ref) {
            // If this is a reference, set the row to 0 as a ref always is a
            // single value, not an array. This prevents the application from
            // having to do an if-check on whether the column is owned.
            //
            // This check only happens when the current table being iterated
            // over caused the query to match a reference. The check is
            // performed once per iterated table.
            this->row_ = 0;
        }

        if (field.is_row) {
            field.ptr = ecs_field_at_w_size(iter, sizeof(T), field.index, 
                static_cast<int8_t>(row));
        }
    }
};


// Type that handles passing components to each callbacks
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
        field_ptrs<Components...> terms;

        iter->flags |= EcsIterCppEach;

        if (iter->ref_fields | iter->up_fields) {
            terms.populate(iter);
            invoke_unpack< each_ref_field >(iter, func_, 0, terms.fields_);
        } else {
            terms.populate_self(iter);
            invoke_unpack< each_field >(iter, func_, 0, terms.fields_);
        }
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const each_delegate*>(iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    // Create instance of delegate
    static each_delegate* make(const Func& func) {
        return FLECS_NEW(each_delegate)(func);
    }

    // Function that can be used as callback to free delegate
    static void destruct(void *obj) {
        _::free_obj<each_delegate>(static_cast<each_delegate*>(obj));
    }

    // Static function to call for component on_add hook
    static void run_add(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->callback_ctx);
        iter->callback_ctx = ctx->on_add;
        run(iter);
    }

    // Static function to call for component on_remove hook
    static void run_remove(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->callback_ctx);
        iter->callback_ctx = ctx->on_remove;
        run(iter);
    }

    // Static function to call for component on_set hook
    static void run_set(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->callback_ctx);
        iter->callback_ctx = ctx->on_set;
        run(iter);
    }

private:
    // func(flecs::entity, Components...)
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args,
        typename Fn = Func,
        decltype(std::declval<const Fn&>()(
            std::declval<flecs::entity>(),
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...), 0) = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t i, Args... comps) 
    {
        ecs_assert(iter->count > 0, ECS_INVALID_OPERATION,
            "no entities returned, use each() without flecs::entity argument");

        func(flecs::entity(iter->world, iter->entities[i]),
            (ColumnType< remove_reference_t<Components> >(iter, comps, i)
                .get_row())...);
    }

    // func(flecs::iter&, size_t row, Components...)
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args,
        typename Fn = Func,
        decltype(std::declval<const Fn&>()(
            std::declval<flecs::iter&>(),
            std::declval<size_t&>(),
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...), 0) = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t i, Args... comps) 
    {
        flecs::iter it(iter);
        func(it, i, (ColumnType< remove_reference_t<Components> >(iter, comps, i)
            .get_row())...);
    }

    // func(Components...)
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args,
        typename Fn = Func,
        decltype(std::declval<const Fn&>()(
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...), 0) = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t i, Args... comps) 
    {
        func((ColumnType< remove_reference_t<Components> >(iter, comps, i)
            .get_row())...);
    }

    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args)> = 0>
    static void invoke_unpack(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        ECS_TABLE_LOCK(iter->world, iter->table);

        size_t count = static_cast<size_t>(iter->count);
        if (count == 0 && !iter->table) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        for (size_t i = 0; i < count; i ++) {
            invoke_callback<ColumnType>(iter, func, i, comps...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< sizeof...(Components) != sizeof...(Args) > = 0>
    static void invoke_unpack(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Args... comps) 
    {
        invoke_unpack<ColumnType>(
            iter, func, index + 1, columns, comps..., columns[index]);
    }    

public:
    Func func_;
};

template <typename Func, typename ... Components>
struct find_delegate : public delegate {
    using Terms = typename field_ptrs<Components ...>::array;

    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit find_delegate(Func&& func) noexcept 
        : func_(FLECS_MOV(func)) { }

    explicit find_delegate(const Func& func) noexcept 
        : func_(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    flecs::entity invoke(ecs_iter_t *iter) const {
        field_ptrs<Components...> terms;

        iter->flags |= EcsIterCppEach;

        if (iter->ref_fields | iter->up_fields) {
            terms.populate(iter);
            return invoke_callback< each_ref_field >(iter, func_, 0, terms.fields_);
        } else {
            terms.populate_self(iter);
            return invoke_callback< each_field >(iter, func_, 0, terms.fields_);
        }
    }

private:
    // Number of function arguments is one more than number of components, pass
    // entity as argument.
    template <template<typename X, typename = int> class ColumnType,
        typename... Args,
        typename Fn = Func,
        if_t<sizeof...(Components) == sizeof...(Args)> = 0,
        decltype(bool(std::declval<const Fn&>()(
            std::declval<flecs::entity>(),
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...))) = true>
    static flecs::entity invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        ECS_TABLE_LOCK(iter->world, iter->table);

        ecs_world_t *world = iter->world;
        size_t count = static_cast<size_t>(iter->count);
        flecs::entity result;

        ecs_assert(count > 0, ECS_INVALID_OPERATION,
            "no entities returned, use find() without flecs::entity argument");

        for (size_t i = 0; i < count; i ++) {
            if (func(flecs::entity(world, iter->entities[i]),
                (ColumnType< remove_reference_t<Components> >(iter, comps, i)
                    .get_row())...))
            {
                result = flecs::entity(world, iter->entities[i]);
                break;
            }
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);

        return result;
    }

    // Number of function arguments is two more than number of components, pass
    // iter + index as argument.
    template <template<typename X, typename = int> class ColumnType,
        typename... Args,
        typename Fn = Func,
        if_t<sizeof...(Components) == sizeof...(Args)> = 0,
        decltype(bool(std::declval<const Fn&>()(
            std::declval<flecs::iter&>(),
            std::declval<size_t&>(),
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...))) = true>
    static flecs::entity invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);
        flecs::entity result;

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (size_t i = 0; i < count; i ++) {
            if (func(it, i, 
                (ColumnType< remove_reference_t<Components> >(iter, comps, i)
                    .get_row())...))
            {
                result = flecs::entity(iter->world, iter->entities[i]);
                break;
            }
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);

        return result;
    }

    // Number of function arguments is equal to number of components, no entity
    template <template<typename X, typename = int> class ColumnType,
        typename... Args,
        typename Fn = Func,
        if_t<sizeof...(Components) == sizeof...(Args)> = 0,
        decltype(bool(std::declval<const Fn&>()(
            std::declval<ColumnType< remove_reference_t<Components> > >().get_row()...))) = true>
    static flecs::entity invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);
        flecs::entity result;

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (size_t i = 0; i < count; i ++) {
            if (func(
                (ColumnType< remove_reference_t<Components> >(iter, comps, i)
                    .get_row())...))
            {
                result = flecs::entity(iter->world, iter->entities[i]);
                break;
            }
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);

        return result;
    }

    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< sizeof...(Components) != sizeof...(Args) > = 0>
    static flecs::entity invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Args... comps) 
    {
        return invoke_callback<ColumnType>(
            iter, func, index + 1, columns, comps..., columns[index]);
    }

    Func func_;
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

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const run_delegate*>(iter->run_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    Func func_;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke an entity observer delegate
////////////////////////////////////////////////////////////////////////////////

template <typename Func>
struct entity_observer_delegate : delegate {
    explicit entity_observer_delegate(Func&& func) noexcept 
        : func_(FLECS_MOV(func)) { }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        invoke<Func>(iter);
    }

private:
    template <typename F,
        decltype(std::declval<const F&>()(std::declval<flecs::entity>()), 0) = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_observer_delegate*>(iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->func_(flecs::entity(iter->world, ecs_field_src(iter, 0)));
    }

    template <typename F,
        decltype(std::declval<const F&>()(), 0) = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_observer_delegate*>(iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->func_();
    }

    Func func_;
};

template <typename Func, typename Event>
struct entity_payload_observer_delegate : delegate {
    explicit entity_payload_observer_delegate(Func&& func) noexcept 
        : func_(FLECS_MOV(func)) { }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        invoke<Func>(iter);
    }

private:
    template <typename F,
        decltype(std::declval<const F&>()(
            std::declval<Event&>()), 0) = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_payload_observer_delegate*>(
            iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->param != nullptr, ECS_INVALID_OPERATION, 
            "entity observer invoked without payload");

        Event *data = static_cast<Event*>(iter->param);
        self->func_(*data);
    }

    template <typename F,
        decltype(std::declval<const F&>()(
            std::declval<flecs::entity>(),
            std::declval<Event&>()), 0) = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_payload_observer_delegate*>(
            iter->callback_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->param != nullptr, ECS_INVALID_OPERATION, 
            "entity observer invoked without payload");

        Event *data = static_cast<Event*>(iter->param);
        self->func_(flecs::entity(iter->world, ecs_field_src(iter, 0)), *data);
    }

    Func func_;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility to invoke callback on entity if it has components in signature
////////////////////////////////////////////////////////////////////////////////

template<typename ... Args>
struct entity_with_delegate_impl;

template<typename ... Args>
struct entity_with_delegate_impl<arg_list<Args ...>> {
    using ColumnArray = flecs::array<int32_t, sizeof...(Args)>;
    using ArrayType = flecs::array<void*, sizeof...(Args)>;
    using DummyArray = flecs::array<int, sizeof...(Args)>;
    using IdArray = flecs::array<id_t, sizeof...(Args)>;

    static bool const_args() {
        static flecs::array<bool, sizeof...(Args)> is_const_args ({
            flecs::is_const<flecs::remove_reference_t<Args>>::value...
        });

        for (auto is_const : is_const_args) {
            if (!is_const) {
                return false;
            }
        }
        return true;
    }

    static 
    bool get_ptrs(world_t *world, flecs::entity_t e, const ecs_record_t *r, ecs_table_t *table,
        ArrayType& ptrs) 
    {
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        if (!ecs_table_column_count(table) && 
            !ecs_table_has_flags(table, EcsTableHasSparse)) 
        {
            return false;
        }

        /* table_index_of needs real world */
        const flecs::world_t *real_world = ecs_get_world(world);

        IdArray ids ({
            _::type<Args>().id(world)...
        });

        /* Get column indices for components */
        ColumnArray columns ({
            ecs_table_get_column_index(real_world, table, 
                _::type<Args>().id(world))...
        });

        /* Get pointers for columns for entity */
        size_t i = 0;
        for (int32_t column : columns) {
            if (column == -1) {
                /* Component could be sparse */
                void *ptr = ecs_get_mut_id(world, e, ids[i]);
                if (!ptr) {
                    return false;
                }

                ptrs[i ++] = ptr;
                continue;
            }

            ptrs[i ++] = ecs_record_get_by_column(r, column, 0);
        }

        return true;
    }

    static bool ensure_ptrs(world_t *world, ecs_entity_t e, ArrayType& ptrs) {
        /* Get pointers w/ensure */
        size_t i = 0;
        DummyArray dummy ({
            (ptrs[i ++] = ecs_ensure_id(world, e, 
                _::type<Args>().id(world)), 0)...
        });

        return true;
    }    

    template <typename Func>
    static bool invoke_read(world_t *world, entity_t e, const Func& func) {
        const ecs_record_t *r = ecs_read_begin(world, e);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ArrayType ptrs;
        bool has_components = get_ptrs(world, e, r, table, ptrs);
        if (has_components) {
            invoke_callback(func, 0, ptrs);
        }

        ecs_read_end(r);

        return has_components;
    }

    template <typename Func>
    static bool invoke_write(world_t *world, entity_t e, const Func& func) {
        ecs_record_t *r = ecs_write_begin(world, e);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ArrayType ptrs;
        bool has_components = get_ptrs(world, e, r, table, ptrs);
        if (has_components) {
            invoke_callback(func, 0, ptrs);
        }

        ecs_write_end(r);

        return has_components;
    }

    template <typename Func>
    static bool invoke_get(world_t *world, entity_t e, const Func& func) {
        if (const_args()) {
            return invoke_read(world, e, func);
        } else {
            return invoke_write(world, e, func);
        }
    }

    // Utility for storing id in array in pack expansion
    static size_t store_added(IdArray& added, size_t elem, ecs_table_t *prev, 
        ecs_table_t *next, id_t id) 
    {
        // Array should only contain ids for components that are actually added,
        // so check if the prev and next tables are different.
        if (prev != next) {
            added[elem] = id;
            elem ++;
        }
        return elem;
    }

    template <typename Func>
    static bool invoke_ensure(world_t *world, entity_t id, const Func& func) {
        flecs::world w(world);

        ArrayType ptrs;
        ecs_table_t *table = NULL;

        // When not deferred take the fast path.
        if (!w.is_deferred()) {
            // Bit of low level code so we only do at most one table move & one
            // entity lookup for the entire operation.

            // Make sure the object is not a stage. Operations on a stage are
            // only allowed when the stage is in deferred mode, which is when
            // the world is in readonly mode.
            ecs_assert(!w.is_stage(), ECS_INVALID_PARAMETER, NULL);

            // Find table for entity
            ecs_record_t *r = ecs_record_find(world, id);
            if (r) {
                table = r->table;
            }

            // Find destination table that has all components
            ecs_table_t *prev = table, *next;
            size_t elem = 0;
            IdArray added;

            // Iterate components, only store added component ids in added array
            DummyArray dummy_before ({ (
                next = ecs_table_add_id(world, prev, w.id<Args>()),
                elem = store_added(added, elem, prev, next, w.id<Args>()),
                prev = next, 0
            )... });
            (void)dummy_before;

            // If table is different, move entity straight to it
            if (table != next) {
                ecs_type_t ids;
                ids.array = added.ptr();
                ids.count = static_cast<ecs_size_t>(elem);
                ecs_commit(world, id, r, next, &ids, NULL);
                table = next;
            }

            if (!get_ptrs(w, id, r, table, ptrs)) {
                ecs_abort(ECS_INTERNAL_ERROR, NULL);
            }

            ECS_TABLE_LOCK(world, table);

        // When deferred, obtain pointers with regular ensure
        } else {
            ensure_ptrs(world, id, ptrs);
        }

        invoke_callback(func, 0, ptrs);

        if (!w.is_deferred()) {
            ECS_TABLE_UNLOCK(world, table);
        }

        // Call modified on each component
        DummyArray dummy_after ({
            ( ecs_modified_id(world, id, w.id<Args>()), 0)...
        });
        (void)dummy_after;

        return true;
    }    

private:
    template <typename Func, typename ... TArgs, 
        if_t<sizeof...(TArgs) == sizeof...(Args)> = 0>
    static void invoke_callback(
        const Func& f, size_t, ArrayType&, TArgs&& ... comps) 
    {
        f(*static_cast<typename base_arg_type<Args>::type*>(comps)...);
    }

    template <typename Func, typename ... TArgs, 
        if_t<sizeof...(TArgs) != sizeof...(Args)> = 0>
    static void invoke_callback(const Func& f, size_t arg, ArrayType& ptrs, 
        TArgs&& ... comps) 
    {
        invoke_callback(f, arg + 1, ptrs, comps..., ptrs[arg]);
    }
};

template <typename Func, typename U = int>
struct entity_with_delegate {
    static_assert(function_traits<Func>::value, "type is not callable");
};

template <typename Func>
struct entity_with_delegate<Func, if_t< is_callable<Func>::value > >
    : entity_with_delegate_impl< arg_list_t<Func> >
{
    static_assert(function_traits<Func>::arity > 0,
        "function must have at least one argument");
};

} // namespace _

// Experimental: allows using the each delegate for use cases outside of flecs
template <typename Func, typename ... Args>
using delegate = _::each_delegate<typename std::decay<Func>::type, Args...>;

} // namespace flecs
