/**
 * @file addons/cpp/iter.hpp
 * @brief Wrapper classes for ecs_iter_t and component arrays.
 */

#pragma once

/**
 * @defgroup cpp_iterator Iterators
 * @ingroup cpp_core
 * Iterator operations.
 *
 * @{
 */

namespace flecs
{

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
        : value_(value){}

    bool operator!=(range_iterator const& other) const
    {
        return value_ != other.value_;
    }

    T const& operator*() const
    {
        return value_;
    }

    range_iterator& operator++()
    {
        ++value_;
        return *this;
    }

private:
    T value_;
};

} // namespace _

} // namespace flecs

namespace flecs
{

////////////////////////////////////////////////////////////////////////////////

/** Class for iterating over query results.
 *
 * @ingroup cpp_iterator
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
    iter(ecs_iter_t *it) : iter_(it) { }

    row_iterator begin() const {
        return row_iterator(0);
    }

    row_iterator end() const {
        return row_iterator(static_cast<size_t>(iter_->count));
    }

    flecs::entity system() const;

    flecs::entity event() const;

    flecs::id event_id() const;

    flecs::world world() const;

    const flecs::iter_t* c_ptr() const {
        return iter_;
    }

    size_t count() const {
        ecs_check(iter_->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
            "operation invalid before calling next()");
        return static_cast<size_t>(iter_->count);
    error:
        return 0;
    }

    ecs_ftime_t delta_time() const {
        return iter_->delta_time;
    }

    ecs_ftime_t delta_system_time() const {
        return iter_->delta_system_time;
    }

    flecs::type type() const;

    flecs::table table() const;

    flecs::table_range range() const;

    /** Access ctx.
     * ctx contains the context pointer assigned to a system.
     */
    void* ctx() {
        return iter_->ctx;
    }

    /** Access ctx.
     * ctx contains the context pointer assigned to a system.
     */
    template <typename T>
    T* ctx() {
        return static_cast<T*>(iter_->ctx);
    }

    /** Access param.
     * param contains the pointer passed to the param argument of system::run
     */
    void* param() {
        return iter_->param;
    }

    /** Access param.
     * param contains the pointer passed to the param argument of system::run
     */
    template <typename T>
    T* param() {
        /* TODO: type check */
        return static_cast<T*>(iter_->param);
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
    bool is_self(int8_t index) const {
        return ecs_field_is_self(iter_, index);
    }

    /** Returns whether field is set.
     *
     * @param index The field index.
     */
    bool is_set(int8_t index) const {
        return ecs_field_is_set(iter_, index);
    }

    /** Returns whether field is readonly.
     *
     * @param index The field index.
     */
    bool is_readonly(int8_t index) const {
        return ecs_field_is_readonly(iter_, index);
    }

    /** Number of fields in iterator.
     */
    int32_t field_count() const {
        return iter_->field_count;
    }

    /** Size of field data type.
     *
     * @param index The field id.
     */
    size_t size(int8_t index) const {
        return ecs_field_size(iter_, index);
    }

    /** Obtain field source (0 if This).
     *
     * @param index The field index.
     */
    flecs::entity src(int8_t index) const;

    /** Obtain id matched for field.
     *
     * @param index The field index.
     */
    flecs::id id(int8_t index) const;

    /** Obtain pair id matched for field.
     * This operation will fail if the id is not a pair.
     *
     * @param index The field index.
     */
    flecs::id pair(int8_t index) const;

    /** Obtain column index for field.
     *
     * @param index The field index.
     */
    int32_t column_index(int8_t index) const {
        return ecs_field_column(iter_, index);
    }

    /** Obtain term that triggered an observer
     */
    int8_t term_index() const {
        return iter_->term_index;
    }

    /** Convert current iterator result to string.
     */
    flecs::string str() const {
        char *s = ecs_iter_str(iter_);
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
    flecs::field<A> field(int8_t index) const;

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
    flecs::field<A> field(int8_t index) const;

    /** Get unchecked access to field data.
     * Unchecked access is required when a system does not know the type of a
     * field at compile time.
     *
     * @param index The field index.
     */
    flecs::untyped_field field(int8_t index) const {
        ecs_assert(!(iter_->flags & EcsIterCppEach), ECS_INVALID_OPERATION,
            "cannot .field from .each, use .field_at(%d, row) instead", index);
        return get_unchecked_field(index);
    }

    /** Get pointer to field at row. */
    void* field_at(int8_t index, size_t row) const {
        if (iter_->row_fields & (1llu << index)) {
            return get_unchecked_field_at(index, row)[0];
        } else {
            return get_unchecked_field(index)[row];
        }
    }

    /** Get reference to field at row. */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    const A& field_at(int8_t index, size_t row) const {
        if (iter_->row_fields & (1llu << index)) {
            return get_field_at<A>(index, row)[0];
        } else {
            return get_field<A>(index)[row];
        }
    }

    /** Get reference to field at row. */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<
            std::is_const<T>::value == false, void>::type* = nullptr>
    A& field_at(int8_t index, size_t row) const {
        ecs_assert(!ecs_field_is_readonly(iter_, index),
            ECS_ACCESS_VIOLATION, NULL);
        if (iter_->row_fields & (1llu << index)) {
            return get_field_at<A>(index, row)[0];
        } else {
            return get_field<A>(index)[row];
        }
    }

    /** Get readonly access to entity ids.
     *
     * @return The entity ids.
     */
    flecs::field<const flecs::entity_t> entities() const {
        return flecs::field<const flecs::entity_t>(
            iter_->entities, static_cast<size_t>(iter_->count), false);
    }

    /** Check if the current table has changed since the last iteration.
     * Can only be used when iterating queries and/or systems. */
    bool changed() {
        return ecs_iter_changed(iter_);
    }

    /** Skip current table.
     * This indicates to the query that the data in the current table is not
     * modified. By default, iterating a table with a query will mark the
     * iterated components as dirty if they are annotated with InOut or Out.
     *
     * When this operation is invoked, the components of the current table will
     * not be marked dirty. */
    void skip() {
        ecs_iter_skip(iter_);
    }

    /* Return group id for current table (grouped queries only) */
    uint64_t group_id() const {
        return iter_->group_id;
    }

    /** Get value of variable by id.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(int var_id) const;

    /** Get value of variable by name.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(const char *name) const;

    /** Progress iterator.
     * This operation should only be called from a context where the iterator is
     * not being progressed automatically. An example of a valid context is
     * inside of a run() callback. An example of an invalid context is inside of
     * an each() callback.
     */
    bool next() {
        if (iter_->flags & EcsIterIsValid && iter_->table) {
            ECS_TABLE_UNLOCK(iter_->world, iter_->table);
        }
        bool result = iter_->next(iter_);
        iter_->flags |= EcsIterIsValid;
        if (result && iter_->table) {
            ECS_TABLE_LOCK(iter_->world, iter_->table);
        }
        return result;
    }

    /** Forward to each.
     * If a system has an each callback registered, this operation will forward
     * the current iterator to the each callback.
     */
    void each() {
        iter_->callback(iter_);
    }

    /** Free iterator resources.
     * This operation only needs to be called when the iterator is not iterated
     * until completion (e.g. the last call to next() did not return false).
     * 
     * Failing to call this operation on an unfinished iterator will throw a
     * fatal LEAK_DETECTED error.
     * 
     * @see ecs_iter_fini()
     */
    void fini() {
        if (iter_->flags & EcsIterIsValid && iter_->table) {
            ECS_TABLE_UNLOCK(iter_->world, iter_->table);
        }
        ecs_iter_fini(iter_);
    }

private:
    /* Get field, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::field<T> get_field(int8_t index) const {

#ifndef FLECS_NDEBUG
        ecs_entity_t term_id = ecs_field_id(iter_, index);
        ecs_assert(ECS_HAS_ID_FLAG(term_id, PAIR) ||
            term_id == _::type<T>::id(iter_->world),
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_field_is_self(iter_, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(iter_->count);
        }

        return flecs::field<A>(
            static_cast<T*>(ecs_field_w_size(iter_, sizeof(A), index)),
            count, is_shared);
    }

    /* Get field, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::field<T> get_field_at(int8_t index, int32_t row) const {

#ifndef FLECS_NDEBUG
        ecs_entity_t term_id = ecs_field_id(iter_, index);
        ecs_assert(ECS_HAS_ID_FLAG(term_id, PAIR) ||
            term_id == _::type<T>::id(iter_->world),
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        return flecs::field<A>(
            static_cast<T*>(ecs_field_at_w_size(iter_, sizeof(A), index, row)),
                1, false);
    }

    flecs::untyped_field get_unchecked_field(int8_t index) const {
        size_t count;
        size_t size = ecs_field_size(iter_, index);
        bool is_shared = !ecs_field_is_self(iter_, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(iter_->count);
        }

        return flecs::untyped_field(
            ecs_field_w_size(iter_, 0, index), size, count, is_shared);
    }

    flecs::untyped_field get_unchecked_field_at(int8_t index, size_t row) const {
        size_t size = ecs_field_size(iter_, index);
        return flecs::untyped_field(
            ecs_field_at_w_size(iter_, 0, index, static_cast<int32_t>(row)), 
                size, 1, false);
    }

    flecs::iter_t *iter_;
};

} // namespace flecs

/** @} */
