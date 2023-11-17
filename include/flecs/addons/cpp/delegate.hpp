/**
 * @file addons/cpp/delegate.hpp
 * @brief Wrappers around C++ functions that provide callbacks for C APIs.
 */

#pragma once

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
struct term_ptr {
    void *ptr;
    bool is_ref;
};

template <typename ... Components>
struct term_ptrs {
    using array = flecs::array<_::term_ptr, sizeof...(Components)>;

    bool populate(const ecs_iter_t *iter) {
        return populate(iter, 0, static_cast<
            remove_reference_t<
                remove_pointer_t<Components>>
                    *>(nullptr)...);
    }

    array m_terms;

private:
    /* Populate terms array without checking for references */
    bool populate(const ecs_iter_t*, size_t) { return false; }

    template <typename T, typename... Targs>
    bool populate(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
        m_terms[index].ptr = iter->ptrs[index];
        bool is_ref = iter->sources && iter->sources[index] != 0;
        m_terms[index].is_ref = is_ref;
        is_ref |= populate(iter, index + 1, comps ...);
        return is_ref;
    }  
};    

struct delegate { };

// Template that figures out from the template parameters of a query/system
// how to pass the value to the each callback
template <typename T, typename = int>
struct each_column { };

// Base class
struct each_column_base {
    each_column_base(const _::term_ptr& term, size_t row) 
        : m_term(term), m_row(row) { }

protected:
    const _::term_ptr& m_term;
    size_t m_row;    
};

// If type is not a pointer, return a reference to the type (default case)
template <typename T>
struct each_column<T, if_t< !is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value && is_actual<T>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T& get_row() {
        return static_cast<T*>(this->m_term.ptr)[this->m_row];
    }  
};

// If argument type is not the same as actual component type, return by value.
// This requires that the actual type can be converted to the type.
// A typical scenario where this happens is when using flecs::pair types.
template <typename T>
struct each_column<T, if_t< !is_pointer<T>::value &&
        !is_empty<actual_type_t<T>>::value && !is_actual<T>::value> > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        return static_cast<actual_type_t<T>*>(this->m_term.ptr)[this->m_row];
    }  
};


// If type is empty (indicating a tag) the query will pass a nullptr. To avoid
// returning nullptr to reference arguments, return a temporary value.
template <typename T>
struct each_column<T, if_t< is_empty<actual_type_t<T>>::value && 
        !is_pointer<T>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        return actual_type_t<T>();
    }
};


// If type is a pointer (indicating an optional value) return the type as is
template <typename T>
struct each_column<T, if_t< is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    actual_type_t<T> get_row() {
        if (this->m_term.ptr) {
            return &static_cast<actual_type_t<T>>(this->m_term.ptr)[this->m_row];
        } else {
            // optional argument doesn't hava a value
            return nullptr;
        }
    }
};

// If the query contains component references to other entities, check if the
// current argument is one.
template <typename T, typename = int>
struct each_ref_column : public each_column<T> {
    each_ref_column(const _::term_ptr& term, size_t row) 
        : each_column<T>(term, row) {

        if (term.is_ref) {
            // If this is a reference, set the row to 0 as a ref always is a
            // single value, not an array. This prevents the application from
            // having to do an if-check on whether the column is owned.
            //
            // This check only happens when the current table being iterated
            // over caused the query to match a reference. The check is
            // performed once per iterated table.
            this->m_row = 0;
        }
    }
};

template <typename Func, typename ... Components>
struct each_delegate : public delegate {
    // If the number of arguments in the function signature is one more than the
    // number of components in the query, an extra entity arg is required.
    static constexpr bool PassEntity = 
        (sizeof...(Components) + 1) == (arity<Func>::value);

    // If the number of arguments in the function is two more than the number of
    // components in the query, extra iter + index arguments are required.
    static constexpr bool PassIter = 
        (sizeof...(Components) + 2) == (arity<Func>::value);

    static_assert(arity<Func>::value > 0, 
        "each() must have at least one argument");

    using Terms = typename term_ptrs<Components ...>::array;

    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit each_delegate(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    explicit each_delegate(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;

        if (terms.populate(iter)) {
            invoke_callback< each_ref_column >(iter, m_func, 0, terms.m_terms);
        } else {
            invoke_callback< each_column >(iter, m_func, 0, terms.m_terms);
        }   
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const each_delegate*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    // Create instance of delegate
    static each_delegate* make(const Func& func) {
        return FLECS_NEW(each_delegate)(func);
    }

    // Function that can be used as callback to free delegate
    static void free(void *obj) {
        _::free_obj<each_delegate>(static_cast<each_delegate*>(obj));
    }

    // Static function to call for component on_add hook
    static void run_add(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_add;
        run(iter);
    }

    // Static function to call for component on_remove hook
    static void run_remove(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_remove;
        run(iter);
    }

    // Static function to call for component on_set hook
    static void run_set(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_set;
        run(iter);
    }

    // Each delegates always use instanced iterators
    static bool instanced() {
        return true;
    }

private:
    // Number of function arguments is one more than number of components, pass
    // entity as argument.
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && PassEntity> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        ECS_TABLE_LOCK(iter->world, iter->table);

        ecs_world_t *world = iter->world;
        size_t count = static_cast<size_t>(iter->count);

        ecs_assert(count > 0, ECS_INVALID_OPERATION,
            "no entities returned, use each() without flecs::entity argument");

        for (size_t i = 0; i < count; i ++) {
            func(flecs::entity(world, iter->entities[i]),
                (ColumnType< remove_reference_t<Components> >(comps, i)
                    .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    // Number of function arguments is two more than number of components, pass
    // iter + index as argument.
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, int Enabled = PassIter, if_t< 
            sizeof...(Components) == sizeof...(Args) && Enabled> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (size_t i = 0; i < count; i ++) {
            func(it, i, (ColumnType< remove_reference_t<Components> >(comps, i)
                .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    // Number of function arguments is equal to number of components, no entity
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && !PassEntity && !PassIter> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (size_t i = 0; i < count; i ++) {
            func( (ColumnType< remove_reference_t<Components> >(comps, i)
                .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< sizeof...(Components) != sizeof...(Args) > = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Args... comps) 
    {
        invoke_callback<ColumnType>(
            iter, func, index + 1, columns, comps..., columns[index]);
    }    

    Func m_func;
};

template <typename Func, typename ... Components>
struct find_delegate : public delegate {
    // If the number of arguments in the function signature is one more than the
    // number of components in the query, an extra entity arg is required.
    static constexpr bool PassEntity = 
        (sizeof...(Components) + 1) == (arity<Func>::value);

    // If the number of arguments in the function is two more than the number of
    // components in the query, extra iter + index arguments are required.
    static constexpr bool PassIter = 
        (sizeof...(Components) + 2) == (arity<Func>::value);

    static_assert(arity<Func>::value > 0, 
        "each() must have at least one argument");

    using Terms = typename term_ptrs<Components ...>::array;

    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit find_delegate(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    explicit find_delegate(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    flecs::entity invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;

        if (terms.populate(iter)) {
            return invoke_callback< each_ref_column >(iter, m_func, 0, terms.m_terms);
        } else {
            return invoke_callback< each_column >(iter, m_func, 0, terms.m_terms);
        }   
    }

    // Find delegates always use instanced iterators
    static bool instanced() {
        return true;
    }

private:
    // Number of function arguments is one more than number of components, pass
    // entity as argument.
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && PassEntity> = 0>
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
                (ColumnType< remove_reference_t<Components> >(comps, i)
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
        typename... Args, int Enabled = PassIter, if_t< 
            sizeof...(Components) == sizeof...(Args) && Enabled> = 0>
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
            if (func(it, i, (ColumnType< remove_reference_t<Components> >(comps, i)
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
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && !PassEntity && !PassIter> = 0>
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
            if (func( (ColumnType< remove_reference_t<Components> >(comps, i)
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

    Func m_func;
};

////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
struct iter_delegate : delegate {
private:
    static constexpr bool IterOnly = arity<Func>::value == 1;

    using Terms = typename term_ptrs<Components ...>::array;

public:
    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
    explicit iter_delegate(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    explicit iter_delegate(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the delegate, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;
        terms.populate(iter);
        invoke_callback(iter, m_func, 0, terms.m_terms);
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const iter_delegate*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    // Instancing needs to be enabled explicitly for iter delegates
    static bool instanced() {
        return false;
    }

private:
    template <typename... Args, if_t<!sizeof...(Args) && IterOnly> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t, Terms&, Args...) 
    {
        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        func(it);

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <typename... Targs, if_t<!IterOnly &&
        (sizeof...(Targs) == sizeof...(Components))> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t, 
        Terms&, Targs... comps) 
    {
        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        func(it, ( static_cast< 
            remove_reference_t< 
                remove_pointer_t< 
                    actual_type_t<Components> > >* >
                        (comps.ptr))...);

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <typename... Targs, if_t<!IterOnly &&
        (sizeof...(Targs) != sizeof...(Components)) > = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Targs... comps) 
    {
        invoke_callback(iter, func, index + 1, columns, comps..., 
            columns[index]);
    }

    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke an entity observer delegate
////////////////////////////////////////////////////////////////////////////////

template <typename Func>
struct entity_observer_delegate : delegate {
    explicit entity_observer_delegate(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        invoke<Func>(iter);
    }
private:
    template <typename F, if_t<arity<F>::value == 1> = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_observer_delegate*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->m_func(flecs::entity(iter->world, ecs_field_src(iter, 1)));
    }

    template <typename F, if_t<arity<F>::value == 0> = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_observer_delegate*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->m_func();
    }

    Func m_func;
};

template <typename Func, typename Event>
struct entity_payload_observer_delegate : delegate {
    explicit entity_payload_observer_delegate(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        invoke<Func>(iter);
    }

private:
    template <typename F, if_t<arity<F>::value == 1> = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_payload_observer_delegate*>(
            iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->param != nullptr, ECS_INVALID_OPERATION, 
            "entity observer invoked without payload");

        Event *data = static_cast<Event*>(iter->param);
        self->m_func(*data);
    }

    template <typename F, if_t<arity<F>::value == 2> = 0>
    static void invoke(ecs_iter_t *iter) {
        auto self = static_cast<const entity_payload_observer_delegate*>(
            iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->param != nullptr, ECS_INVALID_OPERATION, 
            "entity observer invoked without payload");

        Event *data = static_cast<Event*>(iter->param);
        self->m_func(flecs::entity(iter->world, ecs_field_src(iter, 1)), *data);
    }

    Func m_func;
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
    bool get_ptrs(world_t *world, const ecs_record_t *r, ecs_table_t *table,
        ArrayType& ptrs) 
    {
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        if (!ecs_table_column_count(table)) {
            return false;
        }

        /* table_index_of needs real world */
        const flecs::world_t *real_world = ecs_get_world(world);

        /* Get column indices for components */
        ColumnArray columns ({
            ecs_table_get_column_index(real_world, table, 
                _::cpp_type<Args>().id(world))...
        });

        /* Get pointers for columns for entity */
        size_t i = 0;
        for (int32_t column : columns) {
            if (column == -1) {
                return false;
            }

            ptrs[i ++] = ecs_record_get_column(r, column, 0);
        }

        return true;
    }

    static bool get_mut_ptrs(world_t *world, ecs_entity_t e, ArrayType& ptrs) {
        /* Get pointers w/get_mut */
        size_t i = 0;
        DummyArray dummy ({
            (ptrs[i ++] = ecs_get_mut_id(world, e, 
                _::cpp_type<Args>().id(world)), 0)...
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
        bool has_components;
        if ((has_components = get_ptrs(world, r, table, ptrs))) {
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
        bool has_components;
        if ((has_components = get_ptrs(world, r, table, ptrs))) {
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
    static bool invoke_get_mut(world_t *world, entity_t id, const Func& func) {
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

            if (!get_ptrs(w, r, table, ptrs)) {
                ecs_abort(ECS_INTERNAL_ERROR, NULL);
            }

            ECS_TABLE_LOCK(world, table);

        // When deferred, obtain pointers with regular get_mut
        } else {
            get_mut_ptrs(world, id, ptrs);
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
