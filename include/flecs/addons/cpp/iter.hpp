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
 * @tparam T The value type of the iterator.
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

    /** Get an iterator to the beginning of the entity range. */
    row_iterator begin() const {
        return row_iterator(0);
    }

    /** Get an iterator to the end of the entity range. */
    row_iterator end() const {
        return row_iterator(static_cast<size_t>(iter_->count));
    }

    /** Get the system entity associated with the iterator. */
    flecs::entity system() const;

    /** Get the event entity associated with the iterator. */
    flecs::entity event() const;

    /** Get the event ID associated with the iterator. */
    flecs::id event_id() const;

    /** Get the world associated with the iterator. */
    flecs::world world() const;

    /** Get a pointer to the underlying C iterator object. */
    const flecs::iter_t* c_ptr() const {
        return iter_;
    }

    /** Get the number of entities to iterate over.
     *
     * @return The number of entities in the current result.
     */
    size_t count() const {
        ecs_check(iter_->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
            "operation invalid before calling next()");
        return static_cast<size_t>(iter_->count);
    error:
        return 0;
    }

    /** Get the time elapsed since the last frame.
     *
     * @return The delta time.
     */
    ecs_ftime_t delta_time() const {
        return iter_->delta_time;
    }

    /** Get the time elapsed since the last system invocation.
     *
     * @return The delta system time.
     */
    ecs_ftime_t delta_system_time() const {
        return iter_->delta_system_time;
    }

    /** Get the type of the iterated table. */
    flecs::type type() const;

    /** Get the table for the current iterator result. */
    flecs::table table() const;

    /** Get the other table for the current iterator result.
     * This is used for move operations where data is moved from one table
     * to another.
     */
    flecs::table other_table() const;

    /** Get the table range for the current iterator result. */
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
     * param contains the pointer passed to the param argument of system::run().
     */
    void* param() {
        return iter_->param;
    }

    /** Access param.
     * param contains the pointer passed to the param argument of system::run().
     */
    template <typename T>
    T* param() {
        /* TODO: type check */
        return static_cast<T*>(iter_->param);
    }

    /** Obtain a mutable handle to the entity being iterated over.
     *
     * @param row Row being iterated over.
     * @return The entity at the specified row.
     */
    flecs::entity entity(size_t row) const;

    /** Return whether the field is matched on self.
     *
     * @param index The field index.
     * @return True if the field is matched on self, false if not.
     */
    bool is_self(int8_t index) const {
        return ecs_field_is_self(iter_, index);
    }

    /** Return whether the field is set.
     *
     * @param index The field index.
     * @return True if the field is set, false if not.
     */
    bool is_set(int8_t index) const {
        return ecs_field_is_set(iter_, index);
    }

    /** Return whether the field is readonly.
     *
     * @param index The field index.
     * @return True if the field is readonly, false if not.
     */
    bool is_readonly(int8_t index) const {
        return ecs_field_is_readonly(iter_, index);
    }

    /** Number of fields in the iterator.
     *
     * @return The number of fields.
     */
    int32_t field_count() const {
        return iter_->field_count;
    }

    /** Size of the field data type.
     *
     * @param index The field index.
     * @return The size of the field data type.
     */
    size_t size(int8_t index) const {
        return ecs_field_size(iter_, index);
    }

    /** Obtain the field source (0 if This).
     *
     * @param index The field index.
     * @return The source entity for the field.
     */
    flecs::entity src(int8_t index) const;

    /** Obtain the ID matched for the field.
     *
     * @param index The field index.
     * @return The ID matched for the field.
     */
    flecs::id id(int8_t index) const;

    /** Obtain the pair ID matched for the field.
     * This operation will fail if the ID is not a pair.
     *
     * @param index The field index.
     * @return The pair ID matched for the field.
     */
    flecs::id pair(int8_t index) const;

    /** Obtain the column index for the field.
     *
     * @param index The field index.
     * @return The column index for the field.
     */
    int32_t column_index(int8_t index) const {
        return ecs_field_column(iter_, index);
    }

    /** Obtain the term that triggered an observer.
     *
     * @return The index of the term that triggered the observer.
     */
    int8_t term_index() const {
        return iter_->term_index;
    }

    /** Convert current iterator result to string.
     *
     * @return String representation of the current iterator result.
     */
    flecs::string str() const {
        char *s = ecs_iter_str(iter_);
        return flecs::string(s);
    }

    /** Get read-only access to field data.
     * If the specified field index does not match with the provided type, the
     * function will assert.
     * 
     * This function should not be used in each() callbacks, unless it is to
     * access a shared field. For access to non-shared fields in each(), use
     * field_at().
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>, if_t<is_const_v<T>> = 0>
    flecs::field<A> field(int8_t index) const;

    /** Get read/write access to field data.
     * If the matched ID for the specified field does not match with the provided
     * type or if the field is readonly, the function will assert.
     * 
     * This function should not be used in each() callbacks, unless it is to
     * access a shared field. For access to non-shared fields in each(), use
     * field_at().
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>, if_not_t<is_const_v<T>> = 0>
    flecs::field<A> field(int8_t index) const;

    /** Get unchecked access to field data.
     * Unchecked access is required when a system does not know the type of a
     * field at compile time.
     * 
     * This function should not be used in each() callbacks, unless it is to
     * access a shared field. For access to non-shared fields in each(), use
     * field_at().
     *
     * @param index The field index.
     */
    flecs::untyped_field field(int8_t index) const {
        ecs_assert(!(iter_->flags & EcsIterCppEach) || 
               ecs_field_src(iter_, index) != 0, ECS_INVALID_OPERATION,
            "cannot .field from .each, use .field_at(%d, row) instead", index);
        return get_unchecked_field(index);
    }

    /** Get pointer to field at row.
     * This function may be used to access shared fields when row is set to 0.
     *
     * @param index The field index.
     * @param row The row index.
     * @return Pointer to the field value at the specified row.
     */
    void* field_at(int8_t index, size_t row) const {
        if (iter_->row_fields & (1llu << index)) {
            return get_unchecked_field_at(index, row)[0];
        } else {
            return get_unchecked_field(index)[row];
        }
    }

    /** Get const reference to field at row.
     * This function may be used to access shared fields when row is set to 0.
     *
     * @tparam T Type of the field (must be const-qualified).
     * @param index The field index.
     * @param row The row index.
     * @return Const reference to the field value at the specified row.
     */
    template <typename T, typename A = actual_type_t<T>, if_t< is_const_v<T> > = 0>
    const A& field_at(int8_t index, size_t row) const {
        if (iter_->row_fields & (1llu << index)) {
            return get_field_at<A>(index, row)[0];
        } else {
            return get_field<A>(index)[row];
        }
    }

    /** Get mutable reference to field at row.
     * This function may be used to access shared fields when row is set to 0.
     *
     * @tparam T Type of the field (must not be const-qualified).
     * @param index The field index.
     * @param row The row index.
     * @return Mutable reference to the field value at the specified row.
     */
    template <typename T, typename A = actual_type_t<T>, if_not_t< is_const_v<T> > = 0>
    A& field_at(int8_t index, size_t row) const {
        ecs_assert(!ecs_field_is_readonly(iter_, index),
            ECS_ACCESS_VIOLATION, NULL);
        if (iter_->row_fields & (1llu << index)) {
            return get_field_at<A>(index, row)[0];
        } else {
            return get_field<A>(index)[row];
        }
    }

    /** Get read-only access to entity IDs.
     *
     * @return The entity IDs.
     */
    flecs::field<const flecs::entity_t> entities() const {
        return flecs::field<const flecs::entity_t>(
            iter_->entities, static_cast<size_t>(iter_->count), false);
    }

    /** Check if the current table has changed since the last iteration.
     * Can only be used when iterating queries and/or systems.
     *
     * @return True if the table has changed.
     */
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

    /** Return the group ID for the current table (grouped queries only).
     *
     * @return The group ID.
     */
    uint64_t group_id() const {
        return ecs_iter_get_group(iter_);
    }

    /** Get value of variable by ID.
     * Get value of a query variable for current result.
     *
     * @param var_id The variable ID.
     * @return The variable value.
     */
    flecs::entity get_var(int var_id) const;

    /** Get value of variable by name.
     * Get value of a query variable for current result.
     *
     * @param name The variable name.
     * @return The variable value.
     */
    flecs::entity get_var(const char *name) const;

    /** Progress iterator.
     * This operation should only be called from a context where the iterator is
     * not being progressed automatically. An example of a valid context is
     * inside of a run() callback. An example of an invalid context is inside of
     * an each() callback.
     *
     * @return True if there is more data to iterate, false if not.
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

    /** Forward to each().
     * If a system has an each() callback registered, this operation will forward
     * the current iterator to the each() callback.
     */
    void each() {
        iter_->callback(iter_);
    }

    /** Iterate targets for pair field.
     * 
     * @param index The field index.
     * @param func Callback invoked for each target.
     */
    template <typename Func>
    void targets(int8_t index, const Func& func);

    /** Free iterator resources.
     * This operation only needs to be called when the iterator is not iterated
     * until completion (e.g., the last call to next() did not return false).
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
    /* Get field, check if correct type is used. */
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

        /* If a shared field is retrieved with field(), there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If field is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(iter_->count);
        }

        return flecs::field<A>(
            static_cast<T*>(ecs_field_w_size(iter_, sizeof(A), index)),
            count, is_shared);
    }

    /* Get field, check if correct type is used. */
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

        /* If a shared field is retrieved with field(), there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If field is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(iter_->count);
        }

        return flecs::untyped_field(
            ecs_field_w_size(iter_, size, index), size, count, is_shared);
    }

    flecs::untyped_field get_unchecked_field_at(int8_t index, size_t row) const {
        size_t size = ecs_field_size(iter_, index);
        return flecs::untyped_field(
            ecs_field_at_w_size(iter_, size, index, static_cast<int32_t>(row)), 
                size, 1, false);
    }

    flecs::iter_t *iter_;
};

} // namespace flecs

/** @} */
