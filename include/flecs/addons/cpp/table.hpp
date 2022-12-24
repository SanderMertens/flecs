/**
 * @file addons/cpp/table.hpp
 * @brief Direct access to table data.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_tables Tables
 * @brief Table operations.
 * 
 * \ingroup cpp_core
 * @{
 */

struct table {
    table() : m_world(nullptr), m_table(nullptr) { }

    table(world_t *world, table_t *t)
        : m_world(world)
        , m_table(t) { }

    virtual ~table() { }

    /** Convert table type to string. */
    flecs::string str() const {
        return flecs::string(ecs_table_str(m_world, m_table));
    }

    /** Get table type. */
    flecs::type type() const {
        return flecs::type(m_world, ecs_table_get_type(m_table));
    }

    /** Get table count. */
    int32_t count() const {
        return ecs_table_count(m_table);
    }

    /** Find index for (component) id. 
     * 
     * @param id The (component) id.
     * @return The index of the id in the table type, -1 if not found/
     */
    int32_t search(flecs::id_t id) const {
        return ecs_search(m_world, m_table, id, 0);
    }

    /** Find index for type. 
     * 
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    int32_t search() const {
        return search(_::cpp_type<T>::id(m_world));
    }

    /** Find index for pair. 
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    int32_t search(flecs::entity_t first, flecs::entity_t second) const {
        return search(ecs_pair(first, second));
    }

    /** Find index for pair. 
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    int32_t search(flecs::entity_t second) const {
        return search(_::cpp_type<First>::id(m_world), second);
    }

    /** Find index for pair. 
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    int32_t search() const {
        return search<First>(_::cpp_type<Second>::id(m_world));
    }

    /** Test if table has (component) id. 
     * 
     * @param id The (component) id.
     * @return True if the table has the id, false if not.
     */
    bool has(flecs::id_t id) const {
        return search(id) != -1;
    }

    /** Test if table has the type. 
     * 
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    bool has() const {
        return search<T>() != -1;
    }

    /** Test if table has the pair.
     * 
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    bool has(flecs::entity_t first, flecs::entity_t second) const {
        return search(first, second) != -1;
    }

    /** Test if table has the pair.
     * 
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    bool has(flecs::entity_t second) const {
        return search<First>(second) != -1;
    }

    /** Test if table has the pair.
     * 
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    bool has() const {
        return search<First, Second>() != -1;
    }

    /** Get pointer to component array by column index. 
     * 
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    virtual void* get_by_index(int32_t index) const {
        return ecs_table_get_column(m_table, index, 0);
    }

    /** Get pointer to component array by component.
     * 
     * @param id The component id.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::id_t id) const {
        int32_t index = search(id);
        if (index == -1) {
            return NULL;
        }
        return get_by_index(index);
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
        return static_cast<T*>(get(_::cpp_type<T>::id(m_world)));
    }

    /** Get pointer to component array by component.
     * 
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, typename A = actual_type_t<T>,
        if_t< flecs::is_pair<T>::value > = 0>
    A* get() const {
        return static_cast<A*>(get(_::cpp_type<T>::id(m_world)));
    }

    /** Get pointer to component array by pair.
     * 
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First>
    First* get(flecs::entity_t second) const {
        return static_cast<First*>(get(_::cpp_type<First>::id(m_world), second));
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
        return static_cast<A*>(get<First>(_::cpp_type<Second>::id(m_world)));
    }

    /** Get depth for given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */    
    int32_t depth(flecs::entity_t rel) {
        return ecs_table_get_depth(m_world, m_table, rel);
    }

    /** Get depth for given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template <typename Rel>
    int32_t depth() {
        return depth(_::cpp_type<Rel>::id(m_world));
    }

    /* Implicit conversion to table_t */
    operator table_t*() const {
        return m_table;
    }

protected:
    world_t *m_world;
    table_t *m_table;
};

struct table_range : table {
    table_range() 
        : table()
        , m_offset(0)
        , m_count(0) { }

    table_range(world_t *world, table_t *t, int32_t offset, int32_t count)
        : table(world, t)
        , m_offset(offset)
        , m_count(count) { }

    int32_t offset() const {
        return m_offset;
    }

    int32_t count() const {
        return m_count;
    }

private:
    /** Get pointer to component array by column index. 
     * 
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    void* get_by_index(int32_t index) const override {
        return ecs_table_get_column(m_table, index, m_offset);
    }

    int32_t m_offset = 0;
    int32_t m_count = 0;
};

/** @} */

}
