#pragma once

#include "../filter/builder_i.hpp"

namespace flecs {

// Query builder interface
template<typename Base, typename ... Components>
struct query_builder_i : filter_builder_i<Base, Components ...> {
private:
    using BaseClass = filter_builder_i<Base, Components ...>;
    
public:
    query_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr) { }

    query_builder_i(ecs_query_desc_t *desc, int32_t term_index = 0) 
        : BaseClass(&desc->filter, term_index)
        , m_desc(desc) { }

    /** Sort the output of a query.
     * This enables sorting of entities across matched tables. As a result of this
     * operation, the order of entities in the matched tables may be changed. 
     * Resorting happens when a query iterator is obtained, and only if the table
     * data has changed.
     *
     * If multiple queries that match the same (sub)set of tables specify different 
     * sorting functions, resorting is likely to happen every time an iterator is
     * obtained, which can significantly slow down iterations.
     *
     * The sorting function will be applied to the specified component. Resorting
     * only happens if that component has changed, or when the entity order in the
     * table has changed. If no component is provided, resorting only happens when
     * the entity order changes.
     *
     * @tparam T The component used to sort.
     * @param compare The compare function used to sort the components.
     */      
    template <typename T>
    Base& order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_order_by_action_t cmp = reinterpret_cast<ecs_order_by_action_t>(compare);
        return this->order_by(_::cpp_type<T>::id(this->world_v()), cmp);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */    
    Base& order_by(flecs::entity_t component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        m_desc->order_by = reinterpret_cast<ecs_order_by_action_t>(compare);
        m_desc->order_by_component = component;
        return *this;
    }

    /** Group and sort matched tables.
     * Similar yo ecs_query_order_by, but instead of sorting individual entities, this
     * operation only sorts matched tables. This can be useful of a query needs to
     * enforce a certain iteration order upon the tables it is iterating, for 
     * example by giving a certain component or tag a higher priority.
     *
     * The sorting function assigns a "rank" to each type, which is then used to
     * sort the tables. Tables with higher ranks will appear later in the iteration.
     * 
     * Resorting happens when a query iterator is obtained, and only if the set of
     * matched tables for a query has changed. If table sorting is enabled together
     * with entity sorting, table sorting takes precedence, and entities will be
     * sorted within each set of tables that are assigned the same rank.
     *
     * @tparam T The component used to determine the group rank.
     * @param rank The rank action.
     */
    template <typename T>
    Base& group_by(uint64_t(*rank)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        ecs_group_by_action_t rnk = reinterpret_cast<ecs_group_by_action_t>(rank);
        return this->group_by(_::cpp_type<T>::id(this->world_v()), rnk);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    Base& group_by(flecs::entity_t component, uint64_t(*rank)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        m_desc->group_by = reinterpret_cast<ecs_group_by_action_t>(rank);
        m_desc->group_by_id = component;
        return *this;
    } 

    /** Specify parent query (creates subquery) */
    Base& parent(const query_base& parent);
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_query_desc_t *m_desc;
};

}
