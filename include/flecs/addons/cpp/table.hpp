/**
 * @file addons/cpp/table.hpp
 * @brief Direct access to table data.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_tables Tables
 * @ingroup cpp_core
 * Table operations.
 *
 * @{
 */

/** Table.
 * A table stores entities with the same set of components.
 *
 * @ingroup cpp_tables
 */
struct table {
    /** Default constructor. */
    table() : world_(nullptr), table_(nullptr) { }

    /** Construct a table from a world and C table pointer.
     *
     * @param world The world.
     * @param t Pointer to the C table.
     */
    table(world_t *world, table_t *t)
        : world_(world)
        , table_(t) { }

    /** Destructor. */
    virtual ~table() { }

    /** Convert the table type to a string. */
    flecs::string str() const {
        return flecs::string(ecs_table_str(world_, table_));
    }

    /** Get the table type. */
    flecs::type type() const {
        return flecs::type(world_, ecs_table_get_type(table_));
    }

    /** Get the table count. */
    int32_t count() const {
        return ecs_table_count(table_);
    }

    /** Get the number of allocated elements in the table. */
    int32_t size() const {
        return ecs_table_size(table_);
    }

    /** Get the array of entity IDs. */
    const flecs::entity_t* entities() const {
        return ecs_table_entities(table_);
    }

    /** Delete entities in the table. */
    void clear_entities() const {
        ecs_table_clear_entities(world_, table_);
    }

    /** Find the type index for a (component) ID.
     *
     * @param id The (component) ID.
     * @return The index of the ID in the table type, -1 if not found.
     */
    int32_t type_index(flecs::id_t id) const {
        return ecs_table_get_type_index(world_, table_, id);
    }

    /** Find the type index for a type.
     *
     * @tparam T The type.
     * @return The index of the ID in the table type, -1 if not found.
     */
    template <typename T>
    int32_t type_index() const {
        return type_index(_::type<T>::id(world_));
    }

    /** Find the type index for a pair.
     *
     * @param first First element of the pair.
     * @param second Second element of the pair.
     * @return The index of the ID in the table type, -1 if not found.
     */
    int32_t type_index(flecs::entity_t first, flecs::entity_t second) const {
        return type_index(ecs_pair(first, second));
    }

    /** Find the type index for a pair.
     *
     * @tparam First First element of the pair.
     * @param second Second element of the pair.
     * @return The index of the ID in the table type, -1 if not found.
     */
    template <typename First>
    int32_t type_index(flecs::entity_t second) const {
        return type_index(_::type<First>::id(world_), second);
    }

    /** Find the type index for a pair.
     *
     * @tparam First First element of the pair.
     * @tparam Second Second element of the pair.
     * @return The index of the ID in the table type, -1 if not found.
     */
    template <typename First, typename Second>
    int32_t type_index() const {
        return type_index<First>(_::type<Second>::id(world_));
    }

    /** Find the column index for a (component) ID.
     *
     * @param id The (component) ID.
     * @return The column index of the ID in the table, -1 if not found.
     */
    int32_t column_index(flecs::id_t id) const {
        return ecs_table_get_column_index(world_, table_, id);
    }

    /** Find the column index for a type.
     *
     * @tparam T The type.
     * @return The column index of the ID in the table type, -1 if not found.
     */
    template <typename T>
    int32_t column_index() const {
        return column_index(_::type<T>::id(world_));
    }

    /** Find the column index for a pair.
     *
     * @param first First element of the pair.
     * @param second Second element of the pair.
     * @return The column index of the ID in the table type, -1 if not found.
     */
    int32_t column_index(flecs::entity_t first, flecs::entity_t second) const {
        return column_index(ecs_pair(first, second));
    }

    /** Find the column index for a pair.
     *
     * @tparam First First element of the pair.
     * @param second Second element of the pair.
     * @return The column index of the ID in the table type, -1 if not found.
     */
    template <typename First>
    int32_t column_index(flecs::entity_t second) const {
        return column_index(_::type<First>::id(world_), second);
    }

    /** Find the column index for a pair.
     *
     * @tparam First First element of the pair.
     * @tparam Second Second element of the pair.
     * @return The column index of the ID in the table type, -1 if not found.
     */
    template <typename First, typename Second>
    int32_t column_index() const {
        return column_index<First>(_::type<Second>::id(world_));
    }

    /** Test if the table has a (component) ID.
     *
     * @param id The (component) ID.
     * @return True if the table has the ID, false if not.
     */
    bool has(flecs::id_t id) const {
        return type_index(id) != -1;
    }

    /** Test if the table has the type.
     *
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    bool has() const {
        return type_index<T>() != -1;
    }

    /** Test if the table has the pair.
     *
     * @param first First element of the pair.
     * @param second Second element of the pair.
     * @return True if the table has the pair, false if not.
     */
    bool has(flecs::entity_t first, flecs::entity_t second) const {
        return type_index(first, second) != -1;
    }

    /** Test if the table has the pair.
     *
     * @tparam First First element of the pair.
     * @param second Second element of the pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    bool has(flecs::entity_t second) const {
        return type_index<First>(second) != -1;
    }

    /** Test if the table has the pair.
     *
     * @tparam First First element of the pair.
     * @tparam Second Second element of the pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    bool has() const {
        return type_index<First, Second>() != -1;
    }

    /** Get a pointer to the component array by column index.
     *
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    virtual void* get_column(int32_t index) const {
        return ecs_table_get_column(table_, index, 0);
    }


    /* get */

    /** Get a pointer to the component array by component.
     *
     * @param id The component ID.
     * @return Pointer to the column, NULL if not found.
     */
    void* try_get(flecs::id_t id) const {
        int32_t index = column_index(id);
        if (index == -1) {
            return NULL;
        }
        return get_column(index);
    }

    /** Get a pointer to the component array by pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    void* try_get(flecs::entity_t first, flecs::entity_t second) const {
        return try_get(ecs_pair(first, second));
    }

    /** Get a pointer to the component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    T* try_get() const {
        return static_cast<T*>(try_get(_::type<T>::id(world_)));
    }

    /** Get a pointer to the component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, typename A = actual_type_t<T>,
        if_t< flecs::is_pair<T>::value > = 0>
    A* try_get() const {
        return static_cast<A*>(try_get(_::type<T>::id(world_)));
    }

    /** Get a pointer to the component array by pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First>
    First* try_get(flecs::entity_t second) const {
        return static_cast<First*>(try_get(_::type<First>::id(world_), second));
    }


    /* get */

    /** Get a pointer to the component array by component.
     *
     * @param id The component ID.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::id_t id) const {
        int32_t index = column_index(id);
        if (index == -1) {
            return NULL;
        }
        void *r = get_column(index);
        ecs_assert(r != nullptr, ECS_INVALID_OPERATION,
            "invalid get: table does not have component (use try_get())");
        return r;
    }

    /** Get a pointer to the component array by pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::entity_t first, flecs::entity_t second) const {
        return get(ecs_pair(first, second));
    }

    /** Get a pointer to the component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    T* get() const {
        return static_cast<T*>(get(_::type<T>::id(world_)));
    }

    /** Get a pointer to the component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, typename A = actual_type_t<T>,
        if_t< flecs::is_pair<T>::value > = 0>
    A* get() const {
        return static_cast<A*>(get(_::type<T>::id(world_)));
    }

    /** Get a pointer to the component array by pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First>
    First* get(flecs::entity_t second) const {
        return static_cast<First*>(get(_::type<First>::id(world_), second));
    }


    /** Get a pointer to the component array by pair.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>,
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    A* get() const {
        return static_cast<A*>(get<First>(_::type<Second>::id(world_)));
    }

    /** Get the column size.
     *
     * @param index The column index.
     * @return The size of the column's component type.
     */
    size_t column_size(int32_t index) const {
        return ecs_table_get_column_size(table_, index);
    }

    /** Get the depth for a given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */
    int32_t depth(flecs::entity_t rel) const  {
        return ecs_table_get_depth(world_, table_, rel);
    }

    /** Get the depth for a given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template <typename Rel>
    int32_t depth() const {
        return depth(_::type<Rel>::id(world_));
    }

    /** Get the table records array.
     *
     * @return The table records.
     */
    ecs_table_records_t records() const {
        return flecs_table_records(table_);
    }

    /** Get the table ID.
     *
     * @return The table ID.
     */
    uint64_t id() const {
        return flecs_table_id(table_);
    }

    /** Lock the table. */
    void lock() const {
        ecs_table_lock(world_, table_);
    }

    /** Unlock the table. */
    void unlock() const {
        ecs_table_unlock(world_, table_);
    }

    /** Check if the table has flags.
     *
     * @param flags The flags to check for.
     * @return True if the table has the specified flags.
     */
    bool has_flags(ecs_flags32_t flags) const {
        return ecs_table_has_flags(table_, flags);
    }

    /** Get the table.
     *
     * @return The table.
     */
    table_t* get_table() const {
        return table_;
    }

    /** Implicit conversion to table_t*. */
    operator table_t*() const {
        return table_;
    }

protected:
    world_t *world_;
    table_t *table_;
};

/** Table range.
 * A table range represents a contiguous range of entities in a table.
 *
 * @ingroup cpp_tables
 */
struct table_range : table {
    /** Default constructor. */
    table_range()
        : table()
        , offset_(0)
        , count_(0) { }

    /** Construct a table range from a world, table, offset, and count.
     *
     * @param world The world.
     * @param t Pointer to the C table.
     * @param offset The starting row offset.
     * @param count The number of rows in the range.
     */
    table_range(world_t *world, table_t *t, int32_t offset, int32_t count)
        : table(world, t)
        , offset_(offset)
        , count_(count) { }

    /** Get the offset of the range. */
    int32_t offset() const {
        return offset_;
    }

    /** Get the number of entities in the range. */
    int32_t count() const {
        return count_;
    }

    /** Get a pointer to the component array by column index.
     *
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    void* get_column(int32_t index) const override {
        return ecs_table_get_column(table_, index, offset_);
    }

private:
    int32_t offset_ = 0;
    int32_t count_ = 0;
};

/** @} */

}
