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

struct table {
    table() : world_(nullptr), table_(nullptr) { }

    table(world_t *world, table_t *t)
        : world_(world)
        , table_(t) { }

    virtual ~table() { }

    /** Convert table type to string. */
    flecs::string str() const {
        return flecs::string(ecs_table_str(world_, table_));
    }

    /** Get table type. */
    flecs::type type() const {
        return flecs::type(world_, ecs_table_get_type(table_));
    }

    /** Get table count. */
    int32_t count() const {
        return ecs_table_count(table_);
    }

    /** Find type index for (component) id.
     *
     * @param id The (component) id.
     * @return The index of the id in the table type, -1 if not found/
     */
    int32_t type_index(flecs::id_t id) const {
        return ecs_table_get_type_index(world_, table_, id);
    }

    /** Find type index for type.
     *
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    int32_t type_index() const {
        return type_index(_::type<T>::id(world_));
    }

    /** Find type index for pair.
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    int32_t type_index(flecs::entity_t first, flecs::entity_t second) const {
        return type_index(ecs_pair(first, second));
    }

    /** Find type index for pair.
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    int32_t type_index(flecs::entity_t second) const {
        return type_index(_::type<First>::id(world_), second);
    }

    /** Find type index for pair.
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    int32_t type_index() const {
        return type_index<First>(_::type<Second>::id(world_));
    }

    /** Find column index for (component) id.
     *
     * @param id The (component) id.
     * @return The index of the id in the table type, -1 if not found/
     */
    int32_t column_index(flecs::id_t id) const {
        return ecs_table_get_column_index(world_, table_, id);
    }

    /** Find column index for type.
     *
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    int32_t column_index() const {
        return column_index(_::type<T>::id(world_));
    }

    /** Find column index for pair.
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    int32_t column_index(flecs::entity_t first, flecs::entity_t second) const {
        return column_index(ecs_pair(first, second));
    }

    /** Find column index for pair.
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    int32_t column_index(flecs::entity_t second) const {
        return column_index(_::type<First>::id(world_), second);
    }

    /** Find column index for pair.
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    int32_t column_index() const {
        return column_index<First>(_::type<Second>::id(world_));
    }

    /** Test if table has (component) id.
     *
     * @param id The (component) id.
     * @return True if the table has the id, false if not.
     */
    bool has(flecs::id_t id) const {
        return type_index(id) != -1;
    }

    /** Test if table has the type.
     *
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    bool has() const {
        return type_index<T>() != -1;
    }

    /** Test if table has the pair.
     *
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    bool has(flecs::entity_t first, flecs::entity_t second) const {
        return type_index(first, second) != -1;
    }

    /** Test if table has the pair.
     *
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    bool has(flecs::entity_t second) const {
        return type_index<First>(second) != -1;
    }

    /** Test if table has the pair.
     *
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    bool has() const {
        return type_index<First, Second>() != -1;
    }

    /** Get pointer to component array by column index.
     *
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    virtual void* get_column(int32_t index) const {
        return ecs_table_get_column(table_, index, 0);
    }

    /** Get pointer to component array by component.
     *
     * @param id The component id.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::id_t id) const {
        int32_t index = column_index(id);
        if (index == -1) {
            return NULL;
        }
        return get_column(index);
    }

    /** Get pointer to component array by pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::entity_t first, flecs::entity_t second) const {
        return get(ecs_pair(first, second));
    }

    /** Get pointer to component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    T* get() const {
        return static_cast<T*>(get(_::type<T>::id(world_)));
    }

    /** Get pointer to component array by (enum) component.
     *
     * @tparam T The (enum) component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, if_t< is_enum<T>::value > = 0>
    T* get() const {
        return static_cast<T*>(get(_::type<T>::id(world_)));
    }

    /** Get pointer to component array by component.
     *
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, typename A = actual_type_t<T>,
        if_t< flecs::is_pair<T>::value > = 0>
    A* get() const {
        return static_cast<A*>(get(_::type<T>::id(world_)));
    }

    /** Get pointer to component array by pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First>
    First* get(flecs::entity_t second) const {
        return static_cast<First*>(get(_::type<First>::id(world_), second));
    }

    /** Get pointer to component array by pair.
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

    /** Get column size */
    size_t column_size(int32_t index) {
        return ecs_table_get_column_size(table_, index);
    }

    /** Get depth for given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */
    int32_t depth(flecs::entity_t rel) {
        return ecs_table_get_depth(world_, table_, rel);
    }

    /** Get depth for given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template <typename Rel>
    int32_t depth() {
        return depth(_::type<Rel>::id(world_));
    }

    /** Get table.
     *
     * @return The table.
     */
    table_t* get_table() const {
        return table_;
    }

    /* Implicit conversion to table_t */
    operator table_t*() const {
        return table_;
    }

protected:
    world_t *world_;
    table_t *table_;
};

struct table_range : table {
    table_range()
        : table()
        , offset_(0)
        , count_(0) { }

    table_range(world_t *world, table_t *t, int32_t offset, int32_t count)
        : table(world, t)
        , offset_(offset)
        , count_(count) { }

    int32_t offset() const {
        return offset_;
    }

    int32_t count() const {
        return count_;
    }

    /** Get pointer to component array by column index.
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
