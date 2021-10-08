
namespace flecs 
{

/** Unsafe wrapper class around a column.
 * This class can be used when a system does not know the type of a column at
 * compile time.
 */
class unsafe_column {
public:
    unsafe_column(void* array, size_t size, size_t count, bool is_shared = false)
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
    void* operator[](size_t index) {
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
 */
template <typename T>
class column {
public:
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
    T& operator[](size_t index) {
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
    T& operator*() {
      ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
      return *m_array;
    }

    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     * 
     * @return Pointer to the first element.
     */
    T* operator->() {
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
 * @tparam Type of the iterator
 */
template <typename T>
class range_iterator
{
public:
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

#ifdef FLECS_DEPRECATED
#include "../addons/deprecated/iter.hpp"
#else
namespace flecs
{
template <typename Base>
class iter_deprecated { };
}
#endif

namespace flecs
{

////////////////////////////////////////////////////////////////////////////////

/** Class that enables iterating over table columns.
 */
class iter : public iter_deprecated<iter> {
    using row_iterator = _::range_iterator<size_t>;
public:
    /** Construct iterator from C iterator object.
     * This operation is typically not invoked directly by the user.
     *
     * @param it Pointer to C iterator.
     */
    iter(const ecs_iter_t *it) : m_iter(it) { 
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

    FLECS_FLOAT delta_time() const {
        return m_iter->delta_time;
    }

    FLECS_FLOAT delta_system_time() const {
        return m_iter->delta_system_time;
    }

    FLECS_FLOAT world_time() const {
        return m_iter->world_time;
    }

    flecs::type type() const;

    /** Is current type a module or does it contain module contents? */
    bool has_module() const {
        return ecs_table_has_module(m_iter->table);
    }

    /** Access self.
     * 'self' is an entity that can be associated with a trigger, observer or
     * system when they are created. */
    flecs::entity self() const;

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    void* ctx() {
        return m_iter->ctx;
    }

    /** Access param. 
     * param contains the pointer passed to the param argument of system::run
     */
    void* param() {
        return m_iter->param;
    }

    /** Obtain mutable handle to entity being iterated over.
     *
     * @param row Row being iterated over.
     */
    flecs::entity entity(size_t row) const;

    /** Returns whether term is owned.
     * 
     * @param index The term index.
     */
    bool is_owned(int32_t index) const {
        return ecs_term_is_owned(m_iter, index);
    }

    /** Returns whether term is set.
     * 
     * @param index The term index.
     */
    bool is_set(int32_t index) const {
        return ecs_term_is_set(m_iter, index);
    }

    /** Returns whether term is readonly.
     *
     * @param index The term index.
     */
    bool is_readonly(int32_t index) const {
        return ecs_term_is_readonly(m_iter, index);
    }

    /** Number of terms in iteator.
     */
    int32_t term_count() const {
        return m_iter->term_count;
    }

    /** Size of term data type.
     *
     * @param index The term id.
     */
    size_t term_size(int32_t index) const {
        return ecs_term_size(m_iter, index);
    }

    /** Obtain term source (0 if self)
     *
     * @param index The term index.
     */    
    flecs::entity term_source(int32_t index) const;

    /** Obtain component/tag entity of term.
     *
     * @param index The term index.
     */
    flecs::entity term_id(int32_t index) const;

    /** Obtain term with const type.
     * If the specified term index does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
        
    flecs::column<A> term(int32_t index) const {
        return get_term<A>(index);
    }

    /** Obtain term with non-const type.
     * If the specified term id does not match with the provided type or if
     * the term is readonly, the function will assert.
     *
     * @tparam T Type of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<
            std::is_const<T>::value == false, void>::type* = nullptr>

    flecs::column<A> term(int32_t index) const {
        ecs_assert(!ecs_term_is_readonly(m_iter, index), 
            ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_term<A>(index);
    }

    /** Obtain unsafe term.
     * Unsafe terms are required when a system does not know at compile time
     * which component will be passed to it. 
     *
     * @param index The term index. 
     */
    flecs::unsafe_column term(int32_t index) const {
        return get_unsafe_term(index);
    }

    /** Obtain owned term.
     * Same as iter::term, but ensures that term is owned.
     *
     * @tparam Type of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<A> term_owned(int32_t index) const {
        ecs_assert(!!ecs_term_is_owned(m_iter, index), ECS_COLUMN_IS_SHARED, NULL);
        return this->term<A>(index);
    }

    /** Obtain shared term.
     * Same as iter::term, but ensures that term is shared.
     *
     * @tparam Type of the term.
     * @param index The term index.
     * @return The component term.
     */
    template <typename T, typename A = actual_type_t<T>>
    const T& term_shared(int32_t index) const {
        ecs_assert(
            ecs_term_id(m_iter, index) == 
                _::cpp_type<T>::id(m_iter->world), 
                    ECS_COLUMN_TYPE_MISMATCH, NULL);

        ecs_assert(!ecs_term_is_owned(m_iter, index), 
            ECS_COLUMN_IS_NOT_SHARED, NULL);

        return *static_cast<A*>(ecs_term_w_size(m_iter, sizeof(A), index));
    }

    /** Obtain the total number of tables the iterator will iterate over.
     */
    int32_t table_count() const {
        return m_iter->table_count;
    }

    /** Obtain untyped pointer to table column.
     *
     * @param table_column Id of table column (corresponds with location in table type).
     * @return Pointer to table column.
     */
    void* table_column(int32_t col) const {
        return ecs_iter_column_w_size(m_iter, 0, col);
    }

    /** Obtain typed pointer to table column.
     * If the table does not contain a column with the specified type, the
     * function will assert.
     *
     * @tparam T Type of the table column.
     */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> table_column() const {
        auto col = ecs_iter_find_column(m_iter, _::cpp_type<T>::id());
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);

        return flecs::column<A>(static_cast<A*>(ecs_iter_column_w_size(m_iter,
            sizeof(A), col)), static_cast<std::size_t>(m_iter->count), false);
    }

    template <typename T>
    flecs::column<T> table_column(flecs::id_t obj) const {
        auto col = ecs_iter_find_column(m_iter, 
            ecs_pair(_::cpp_type<T>::id(), obj));
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);

        return flecs::column<T>(static_cast<T*>(ecs_iter_column_w_size(m_iter,
            sizeof(T), col)), static_cast<std::size_t>(m_iter->count), false);
    }    

private:
    /* Get term, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> get_term(int32_t index) const {

#ifndef NDEBUG
        ecs_entity_t term_id = ecs_term_id(m_iter, index);
        ecs_assert(term_id & ECS_PAIR || term_id & ECS_SWITCH || 
            term_id & ECS_CASE ||
            term_id == _::cpp_type<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_term_is_owned(m_iter, index);

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
            static_cast<T*>(ecs_term_w_size(m_iter, sizeof(A), index)), 
            count, is_shared);
    }

    flecs::unsafe_column get_unsafe_term(int32_t index) const {
        size_t count;
        size_t size = ecs_term_size(m_iter, index);
        bool is_shared = !ecs_term_is_owned(m_iter, index);

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

        return flecs::unsafe_column(
            ecs_term_w_size(m_iter, 0, index), size, count, is_shared);
    }     

    const flecs::iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

} // namespace flecs
