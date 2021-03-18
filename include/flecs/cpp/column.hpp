
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

    /** Return whether component is set.
     * If the column is optional, this method may return false.
     * 
     * @return True if component is set, false if component is not set.
     */
    bool is_set() const {
        return m_array != nullptr;
    }

    /** Return whether component is shared.
     * If the column is shared, this method returns true.
     * 
     * @return True if component is shared, false if component is owned.
     */
    bool is_shared() const {
        return m_is_shared;
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
    T* operator->() {
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array;
    }

    /** Return whether component is set.
     * If the column is optional, this method may return false.
     * 
     * @return True if component is set, false if component is not set.
     */
    bool is_set() const {
        return m_array != nullptr;
    }

    /** Return whether component is shared.
     * If the column is shared, this method returns true.
     * 
     * @return True if component is shared, false if component is owned.
     */
    bool is_shared() const {
        return m_is_shared;
    }

protected:
    T* m_array;
    size_t m_count;
    bool m_is_shared;
};


////////////////////////////////////////////////////////////////////////////////

namespace _ {

/** Similar to flecs::column, but abstracts away from shared / owned columns.
 * 
 * @tparam T component type of the column.
 */
template <typename T, typename = void>
class any_column { };

template <typename T>
class any_column<T, typename std::enable_if<std::is_pointer<T>::value == true>::type > final : public column<typename std::remove_pointer<T>::type> {
public:
    any_column(T array, std::size_t count, bool is_shared = false)
        : column<typename std::remove_pointer<T>::type>(array, count, is_shared) { }

    T operator[](size_t index) {
        if (!this->m_is_shared) {
            ecs_assert(index < this->m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
            if (this->m_array) {
                return &this->m_array[index];
            } else {
                return nullptr;
            }
        } else {
            return &this->m_array[0];
        }
    }   
};

template <typename T>
class any_column<T, typename std::enable_if<std::is_pointer<T>::value == false>::type> final : public column<T> {
public:
    any_column(T* array, std::size_t count, bool is_shared = false)
        : column<T>(array, count, is_shared) { }

    T& operator[](size_t index) {
        if (!this->m_is_shared) {
            ecs_assert(index < this->m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
            return this->m_array[index];
        } else {
            return this->m_array[0];
        }
    }   
};

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


////////////////////////////////////////////////////////////////////////////////

/** Class that enables iterating over table columns.
 */
class iter final {
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

    /** Obtain handle to current system. 
     */
    flecs::entity system() const;

    /** Obtain current world. 
     */
    flecs::world world() const;

    /** Number of entities to iterate over. 
     */
    size_t count() const {
        return static_cast<size_t>(m_iter->count);
    }

    /** Number of columns in iteator.
     */
    int32_t column_count() const {
        return m_iter->column_count;
    }

    /** Size of column data type.
     *
     * @param col The column id.
     */
    size_t column_size(int32_t col) const {
        return ecs_column_size(m_iter, col);
    }    

    /** Return delta_time of current frame. 
     */
    FLECS_FLOAT delta_time() const {
        return m_iter->delta_time;
    }

    /** Return time elapsed since last time system was invoked.
     */
    FLECS_FLOAT delta_system_time() const {
        return m_iter->delta_system_time;
    }

    /** Return total time passed in simulation.
     */
    FLECS_FLOAT world_time() const {
        return m_iter->world_time;
    }

    /** Returns whether column is shared.
     * 
     * @param col The column id.
     */
    bool is_shared(int32_t col) const {
        return !ecs_is_owned(m_iter, col);
    }

    /** Returns whether column is owned.
     * 
     * @param col The column id.
     */
    bool is_owned(int32_t col) const {
        return ecs_is_owned(m_iter, col);
    }    

    /** Returns whether column is set.
     * 
     * @param col The column id.
     */
    bool is_set(int32_t col) const {
        return ecs_column_w_size(m_iter, 0, col) != NULL;
    }
    
    /** Access param field. 
     * The param field contains the value assigned to flecs::Context, or the
     * value passed to the `param` argument when invoking system::run.
     */
    void* param() {
        return m_iter->param;
    }

    /** Returns whether column is readonly.
     *
     * @param col The column id.
     */
    bool is_readonly(int32_t col) const {
        return ecs_is_readonly(m_iter, col);
    }

    /** Obtain column source (0 if self)
     *
     * @param col The column id.
     */    
    flecs::entity column_source(int32_t col) const;

    /** Obtain component/tag entity of column.
     *
     * @param col The column id.
     */
    flecs::entity column_entity(int32_t col) const;

    /** Obtain type of column 
     *
     * @param col The column id.
     */
    flecs::type column_type(int32_t col) const;

    /** Obtain mutable handle to entity being iterated over.
     *
     * @param row Row being iterated over.
     */
    flecs::entity entity(size_t row) const;

    /** Obtain type of table being iterated over.
     */
    type table_type() const;

    /** Obtain the total number of tables the iterator will iterate over.
     */
    int32_t table_count() const {
        return m_iter->table_count;
    }

    /** Obtain the total number of inactive tables the query is matched with.
     */
    int32_t inactive_table_count() const {
        return m_iter->inactive_table_count;
    }

    /** Obtain untyped pointer to table column.
     *
     * @param table_column Id of table column (corresponds with location in table type).
     * @return Pointer to table column.
     */
    void* table_column(int32_t col) const {
        return ecs_table_column(m_iter, col);
    }

    /** Obtain typed pointer to table column.
     * If the table does not contain a column with the specified type, the
     * function will assert.
     *
     * @tparam T Type of the table column.
     */
    template <typename T>
    flecs::column<T> table_column() const {
        auto type = ecs_iter_type(m_iter);
        auto col = ecs_type_index_of(type, _::component_info<T>::id());
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);
        return flecs::column<T>(static_cast<T*>(ecs_table_column(m_iter, col)), 
            static_cast<std::size_t>(m_iter->count), false);
    }

    /** Obtain column with const type.
     * If the specified column id does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    flecs::column<T> column(int32_t col) const {
        return get_column<T>(col);
    }

    /** Obtain column with non-const type.
     * If the specified column id does not match with the provided type or if
     * the column is readonly, the function will assert.
     *
     * @tparam T Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    flecs::column<T> column(int32_t col) const {
        ecs_assert(!ecs_is_readonly(m_iter, col), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_column<T>(col);
    }  

    /** Obtain unsafe column.
     * Unsafe columns are required when a system does not know at compile time
     * which component will be passed to it. 
     *
     * @param col The column id. 
     */
    flecs::unsafe_column column(int32_t col) const {
        return get_unsafe_column(col);
    }

    /** Obtain owned column.
     * Same as iter::column, but ensures that column is owned.
     *
     * @tparam Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T>
    flecs::column<T> owned(int32_t col) const {
        ecs_assert(!!ecs_is_owned(m_iter, col), ECS_COLUMN_IS_SHARED, NULL);
        return this->column<T>(col);
    }

    /** Obtain shared column.
     * Same as iter::column, but ensures that column is shared.
     *
     * @tparam Type of the column.
     * @param col The column id.
     * @return The component column.
     */
    template <typename T>
    const T& shared(int32_t col) const {
        ecs_assert(ecs_column_entity(m_iter, col) == _::component_info<T>::id(m_iter->world), ECS_COLUMN_TYPE_MISMATCH, NULL);
        ecs_assert(!ecs_is_owned(m_iter, col), ECS_COLUMN_IS_NOT_SHARED, NULL);
        return *static_cast<T*>(ecs_column_w_size(m_iter, sizeof(T), col));
    }

    /** Obtain single const element of owned or shared column.
     * If the specified column id does not match with the provided type the
     * function will assert.    
     *
     * @tparam Type of the element.
     * @param col The column id.
     * @param row The current row.
     * @return The component element.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>    
    T& element(int32_t col, int32_t row) const {
        return get_element<T>(col, row);
    }

    /** Obtain single const element of owned or shared column.
     * If the specified column id does not match with the provided type or if
     * the column is readonly, the function will assert.
     *
     * @tparam Type of the element.
     * @param col The column id.
     * @param row The current row.
     * @return The component element.
     */
    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false, void>::type* = nullptr>
    T& element(int32_t col, int32_t row) const {
        ecs_assert(!ecs_is_readonly(m_iter, col), ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_element<T>(col, row);
    }

private:
    /* Get column, check if correct type is used */
    template <typename T>
    flecs::column<T> get_column(int32_t column_id) const {
#ifndef NDEBUG
        ecs_entity_t column_entity = ecs_column_entity(m_iter, column_id);
        ecs_assert(column_entity & ECS_TRAIT || column_entity & ECS_SWITCH || 
            column_entity & ECS_CASE ||
            column_entity == _::component_info<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_is_owned(m_iter, column_id);

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
        
        return flecs::column<T>(
            static_cast<T*>(ecs_column_w_size(m_iter, sizeof(T), column_id)), 
            count, is_shared);
    } 

    flecs::unsafe_column get_unsafe_column(int32_t column_id) const {
        size_t count;
        size_t size = ecs_column_size(m_iter, column_id);
        bool is_shared = !ecs_is_owned(m_iter, column_id);

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

        return flecs::unsafe_column(ecs_column_w_size(m_iter, 0, column_id), size, count, is_shared);
    }       

    /* Get single field, check if correct type is used */
    template <typename T>
    T& get_element(int32_t col, int32_t row) const {
        ecs_assert(ecs_column_entity(m_iter, col) == _::component_info<T>::id(m_iter->world), ECS_COLUMN_TYPE_MISMATCH, NULL);
        return *static_cast<T*>(ecs_element_w_size(m_iter, sizeof(T), col, row));
    }       

    const ecs_iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

template <typename T>
inline column<T>::column(iter &iter, int32_t col) {
    *this = iter.column<T>(col);
}

} // namespace flecs
