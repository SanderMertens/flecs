/**
 * @file addons/cpp/iter.hpp
 * @brief Wrapper classes for ecs_iter_t and component arrays.
 */

#pragma once

/**
 * @defgroup cpp_iterator Iterators
 * @brief Iterator operations.
 * 
 * \ingroup cpp_core
 * @{
 */

namespace flecs 
{

/** Unsafe wrapper class around a column.
 * This class can be used when a system does not know the type of a column at
 * compile time.
 * 
 * \ingroup cpp_iterator
 */
struct untyped_column {
    untyped_column(void* array, size_t size, size_t count, bool is_shared = false)
        : m_array(array)
        , m_size(size)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    void* operator[](size_t index) const {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!m_is_shared, ECS_INVALID_PARAMETER, NULL);
        return ECS_OFFSET(m_array, m_size * index);
    }

protected:
    void* m_array;
    size_t m_size;
    size_t m_count;
    bool m_is_shared;        
};

/** Wrapper class around a column.
 * 
 * @tparam T component type of the column.
 * 
 * \ingroup cpp_iterator
 */
template <typename T>
struct column {
    static_assert(std::is_empty<T>::value == false, 
        "invalid type for column, cannot iterate empty type");

    /** Create column from component array.
     *
     * @param array Pointer to the component array.
     * @param count Number of elements in component array.
     * @param is_shared Is the component shared or not.
     */
    column(T* array, size_t count, bool is_shared = false)
        : m_array(array)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Create column from iterator.
     *
     * @param iter Iterator object.
     * @param column Index of the signature of the query being iterated over.
     */
    column(iter &iter, int column);

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    T& operator[](size_t index) const {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!index || !m_is_shared, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array[index];
    }
  
    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     *
     * @return Reference to the first element.
     */
    T& operator*() const {
      ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
      return *m_array;
    }

    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     * 
     * @return Pointer to the first element.
     */
    T* operator->() const {
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array;
    }

protected:
    T* m_array;
    size_t m_count;
    bool m_is_shared;
};


////////////////////////////////////////////////////////////////////////////////

namespace _ {

////////////////////////////////////////////////////////////////////////////////

/** Iterate over an integer range (used to iterate over entity range).
 *
 * @tparam T of the iterator
 */
template <typename T>
struct range_iterator
{
    explicit range_iterator(T value)
        : m_value(value){}

    bool operator!=(range_iterator const& other) const
    {
        return m_value != other.m_value;
    }

    T const& operator*() const
    {
        return m_value;
    }

    range_iterator& operator++()
    {
        ++m_value;
        return *this;
    }

private:
    T m_value;
};

} // namespace _

} // namespace flecs

namespace flecs
{

////////////////////////////////////////////////////////////////////////////////

/** Class for iterating over query results.
 * 
 * \ingroup cpp_iterator
 */
struct iter {
private:
    using row_iterator = _::range_iterator<size_t>;
    
public:
    /** Construct iterator from C iterator object.
     * This operation is typically not invoked directly by the user.
     *
     * @param it Pointer to C iterator.
     */
    iter(ecs_iter_t *it) : m_iter(it) { 
        m_begin = 0;
        m_end = static_cast<std::size_t>(it->count);
    }

    row_iterator begin() const {
        return row_iterator(m_begin);
    }

    row_iterator end() const {
        return row_iterator(m_end);
    }

    flecs::entity system() const;

    flecs::entity event() const;

    flecs::id event_id() const;

    flecs::world world() const;

    const flecs::iter_t* c_ptr() const {
        return m_iter;
    }

    size_t count() const {
        return static_cast<size_t>(m_iter->count);
    }

    ecs_ftime_t delta_time() const {
        return m_iter->delta_time;
    }

    ecs_ftime_t delta_system_time() const {
        return m_iter->delta_system_time;
    }

    flecs::type type() const;

    flecs::table table() const;

    flecs::table_range range() const;

    /** Is current type a module or does it contain module contents? */
    bool has_module() const {
        return ecs_table_has_module(m_iter->table);
    }

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    void* ctx() {
        return m_iter->ctx;
    }

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    template <typename T>
    T* ctx() {
        return static_cast<T*>(m_iter->ctx);
    }

    /** Access param. 
     * param contains the pointer passed to the param argument of system::run
     */
    void* param() {
        return m_iter->param;
    }

    /** Access param. 
     * param contains the pointer passed to the param argument of system::run
     */
    template <typename T>
    T* param() {
        /* TODO: type check */
        return static_cast<T*>(m_iter->param);
    }

    /** Obtain mutable handle to entity being iterated over.
     *
     * @param row Row being iterated over.
     */
    flecs::entity entity(size_t row) const;

    /** Returns whether field is matched on self.
     * 
     * @param index The field index.
     */
    bool is_self(int32_t index) const {
        return ecs_field_is_self(m_iter, index);
    }

    /** Returns whether field is set.
     * 
     * @param index The field index.
     */
    bool is_set(int32_t index) const {
        return ecs_field_is_set(m_iter, index);
    }

    /** Returns whether field is readonly.
     *
     * @param index The field index.
     */
    bool is_readonly(int32_t index) const {
        return ecs_field_is_readonly(m_iter, index);
    }

    /** Number of fields in iteator.
     */
    int32_t field_count() const {
        return m_iter->field_count;
    }

    /** Size of field data type.
     *
     * @param index The field id.
     */
    size_t size(int32_t index) const {
        return ecs_field_size(m_iter, index);
    }

    /** Obtain field source (0 if This).
     *
     * @param index The field index.
     */    
    flecs::entity src(int32_t index) const;

    /** Obtain id matched for field.
     *
     * @param index The field index.
     */
    flecs::entity id(int32_t index) const;

    /** Obtain pair id matched for field.
     * This operation will fail if the id is not a pair.
     * 
     * @param index The field index.
     */
    flecs::id pair(int32_t index) const;

    /** Convert current iterator result to string.
     */
    flecs::string str() const {
        char *s = ecs_iter_str(m_iter);
        return flecs::string(s);
    }

    /** Get readonly access to field data.
     * If the specified field index does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    flecs::column<A> field(int32_t index) const {
        return get_field<A>(index);
    }

    /** Get read/write access to field data.
     * If the matched id for the specified field does not match with the provided 
     * type or if the field is readonly, the function will assert.
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<
            std::is_const<T>::value == false, void>::type* = nullptr>
    flecs::column<A> field(int32_t index) const {
        ecs_assert(!ecs_field_is_readonly(m_iter, index), 
            ECS_ACCESS_VIOLATION, NULL);
        return get_field<A>(index);
    }

    /** Get unchecked access to field data.
     * Unchecked access is required when a system does not know the type of a
     * field at compile time.
     *
     * @param index The field index. 
     */
    flecs::untyped_column field(int32_t index) const {
        return get_unchecked_field(index);
    }

    /** Obtain the total number of tables the iterator will iterate over. */
    int32_t table_count() const {
        return m_iter->table_count;
    }

    /** Check if the current table has changed since the last iteration.
     * Can only be used when iterating queries and/or systems. */
    bool changed() {
        return ecs_query_changed(nullptr, m_iter);
    }

    /** Skip current table.
     * This indicates to the query that the data in the current table is not
     * modified. By default, iterating a table with a query will mark the 
     * iterated components as dirty if they are annotated with InOut or Out.
     * 
     * When this operation is invoked, the components of the current table will
     * not be marked dirty. */
    void skip() {
        ecs_query_skip(m_iter);
    }

    /* Return group id for current table (grouped queries only) */
    uint64_t group_id() const {
        return m_iter->group_id;
    }

#ifdef FLECS_RULES
    /** Get value of variable by id.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(int var_id) const;

    /** Get value of variable by name.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(const char *name) const;
#endif

private:
    /* Get field, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> get_field(int32_t index) const {

#ifndef FLECS_NDEBUG
        ecs_entity_t term_id = ecs_field_id(m_iter, index);
        ecs_assert(ECS_HAS_ID_FLAG(term_id, PAIR) ||
            term_id == _::cpp_type<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_field_is_self(m_iter, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }
        
        return flecs::column<A>(
            static_cast<T*>(ecs_field_w_size(m_iter, sizeof(A), index)), 
            count, is_shared);
    }

    flecs::untyped_column get_unchecked_field(int32_t index) const {
        size_t count;
        size_t size = ecs_field_size(m_iter, index);
        bool is_shared = !ecs_field_is_self(m_iter, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }

        return flecs::untyped_column(
            ecs_field_w_size(m_iter, 0, index), size, count, is_shared);
    }     

    flecs::iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

} // namespace flecs

/** @} */