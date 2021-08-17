
////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

namespace flecs
{

namespace _ 
{

// Utility to convert template argument pack to array of term ptrs
struct term_ptr {
    void *ptr;
    bool is_ref;
};

template <typename ... Components>
class term_ptrs {
public:
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
        bool is_ref = iter->subjects && iter->subjects[index] != 0;
        m_terms[index].is_ref = is_ref;
        is_ref |= populate(iter, index + 1, comps ...);
        return is_ref;
    }  
};    

class invoker { };

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
struct each_column<T, if_t< !is_pointer<T>::value && is_actual<T>::value > > 
    : public each_column_base 
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
struct each_column<T, if_t< !is_pointer<T>::value && !is_actual<T>::value> > 
    : public each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        return static_cast<actual_type_t<T>*>(this->m_term.ptr)[this->m_row];
    }  
};

// If type is a pointer (indicating an optional value) return the type as is
template <typename T>
struct each_column<T, if_t< is_pointer<T>::value > > 
    : public each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
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
class each_invoker : public invoker {
public:    
    // If the number of arguments in the function signature is one more than the
    // number of components in the query, an extra entity arg is required.
    static constexpr bool PassEntity = 
        sizeof...(Components) == (arity<Func>::value - 1);

    static_assert(arity<Func>::value > 0, 
        "each() must have at least one argument");

    using Terms = typename term_ptrs<Components ...>::array;

    explicit each_invoker(Func&& func) noexcept 
        : m_func(std::move(func)) { }

    explicit each_invoker(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
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
        auto self = static_cast<const each_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
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
#ifndef NDEBUG
        ecs_table_t *table = iter->table;
        if (table) {
            ecs_table_lock(iter->world, table);
        }
#endif

        flecs::iter it(iter);
        for (auto row : it) {
            func(it.entity(row),
                (ColumnType< remove_reference_t<Components> >(comps, row)
                    .get_row())...);
        }

#ifndef NDEBUG
        if (table) {
            ecs_table_unlock(iter->world, table);
        }
#endif        
    }

    // Number of function arguments is equal to number of components, no entity
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && !PassEntity> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        flecs::iter it(iter);
        for (auto row : it) {
            func( (ColumnType< remove_reference_t<Components> >(comps, row)
                .get_row())...);
        }
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


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
class iter_invoker : public invoker {
    static constexpr bool IterOnly = arity<Func>::value == 1;

    using Terms = typename term_ptrs<Components ...>::array;

public:
    explicit iter_invoker(Func&& func) noexcept 
        : m_func(std::move(func)) { }

    explicit iter_invoker(const Func& func) noexcept 
        : m_func(func) { }

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
    template <typename... Args, if_t<!sizeof...(Args) && IterOnly> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t, Terms&, Args...) 
    {
        flecs::iter it(iter);
        func(it);
    }

    template <typename... Targs, if_t<!IterOnly &&
        (sizeof...(Targs) == sizeof...(Components))> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t, 
        Terms&, Targs... comps) 
    {
        flecs::iter it(iter);

#ifndef NDEBUG
        ecs_table_t *table = iter->table;
        if (table) {
            ecs_table_lock(iter->world, table);
        }
#endif

        func(it, ( static_cast< 
            remove_reference_t< 
                remove_pointer_t< 
                    actual_type_t<Components> > >* >
                        (comps.ptr))...);

#ifndef NDEBUG
        if (table) {
            ecs_table_unlock(iter->world, table);
        }
#endif                        
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
//// Utility to invoke callback on entity if it has components in signature
////////////////////////////////////////////////////////////////////////////////

template<typename ... Args>
class entity_with_invoker_impl;

template<typename ... Args>
class entity_with_invoker_impl<arg_list<Args ...>> {
public:
    using ColumnArray = flecs::array<int32_t, sizeof...(Args)>;
    using ConstPtrArray = flecs::array<const void*, sizeof...(Args)>;
    using PtrArray = flecs::array<void*, sizeof...(Args)>;
    using DummyArray = flecs::array<int, sizeof...(Args)>;
    using IdArray = flecs::array<id_t, sizeof...(Args)>;

    template <typename ArrayType>
    static bool get_ptrs(world& w, ecs_record_t *r, ecs_table_t *table, 
        ArrayType& ptrs) 
    {
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_type_t type = ecs_table_get_type(table);
        if (!type) {
            return false;
        }

        /* Get column indices for components */
        ColumnArray columns ({
            ecs_type_index_of(type, 0, w.id<Args>())...
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

    template <typename ArrayType>
    static bool get_mut_ptrs(world& w, ecs_entity_t e, ArrayType& ptrs) {        
        world_t *world = w.c_ptr();

        /* Get pointers w/get_mut */
        size_t i = 0;
        DummyArray dummy ({
            (ptrs[i ++] = ecs_get_mut_id(world, e, w.id<Args>(), NULL), 0)...
        });

        return true;
    }    

    template <typename Func>
    static bool invoke_get(world_t *world, entity_t id, const Func& func) {
        flecs::world w(world);

        ecs_record_t *r = ecs_record_find(world, id);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ConstPtrArray ptrs;
        if (!get_ptrs(w, r, table, ptrs)) {
            return false;
        }

        invoke_callback(func, 0, ptrs);

        return true;
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

        PtrArray ptrs;

        // When not deferred take the fast path.
        if (!w.is_deferred()) {
            // Bit of low level code so we only do at most one table move & one
            // entity lookup for the entire operation.

            // Find table for entity
            ecs_record_t *r = ecs_record_find(world, id);
            ecs_table_t *table = NULL;
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
                ecs_ids_t ids;
                ids.array = added.ptr();
                ids.count = static_cast<ecs_size_t>(elem);
                ecs_commit(world, id, r, next, &ids, NULL);
                table = next;
            }

            if (!get_ptrs(w, r, table, ptrs)) {
                ecs_abort(ECS_INTERNAL_ERROR, NULL);
            }

        // When deferred, obtain pointers with regular get_mut
        } else {
            get_mut_ptrs(w, id, ptrs);
        }

        invoke_callback(func, 0, ptrs);

        // Call modified on each component
        DummyArray dummy_after ({
            ( ecs_modified_id(world, id, w.id<Args>()), 0)...
        });
        (void)dummy_after;

        return true;
    }    

private:
    template <typename Func, typename ArrayType, typename ... TArgs, 
        if_t<sizeof...(TArgs) == sizeof...(Args)> = 0>
    static void invoke_callback(
        const Func& f, size_t, ArrayType&, TArgs&& ... comps) 
    {
        f(*static_cast<typename base_arg_type<Args>::type*>(comps)...);
    }

    template <typename Func, typename ArrayType, typename ... TArgs, 
        if_t<sizeof...(TArgs) != sizeof...(Args)> = 0>
    static void invoke_callback(const Func& f, size_t arg, ArrayType& ptrs, 
        TArgs&& ... comps) 
    {
        invoke_callback(f, arg + 1, ptrs, comps..., ptrs[arg]);
    }
};

template <typename Func, typename U = int>
class entity_with_invoker {
    static_assert(function_traits<Func>::value, "type is not callable");
};

template <typename Func>
class entity_with_invoker<Func, if_t< is_callable<Func>::value > >
    : public entity_with_invoker_impl< arg_list_t<Func> >
{
    static_assert(function_traits<Func>::arity > 0,
        "function must have at least one argument");
};

} // namespace _

} // namespace flecs
