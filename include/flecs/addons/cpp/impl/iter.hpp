/**
 * @file addons/cpp/impl/iter.hpp
 * @brief Iterator implementation.
 */

#pragma once

namespace flecs
{

inline flecs::entity iter::system() const {
    return flecs::entity(m_iter->world, m_iter->system);
}

inline flecs::entity iter::event() const {
    return flecs::entity(m_iter->world, m_iter->event);
}

inline flecs::id iter::event_id() const {
    return flecs::id(m_iter->world, m_iter->event_id);
}

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < static_cast<size_t>(m_iter->count), 
        ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(m_iter->world, m_iter->entities[row]);
}

inline flecs::entity iter::src(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_field_src(m_iter, index));
}

inline flecs::id iter::id(int32_t index) const {
    return flecs::id(m_iter->world, ecs_field_id(m_iter, index));
}

inline flecs::id iter::pair(int32_t index) const {
    flecs::id_t id = ecs_field_id(m_iter, index);
    ecs_check(ECS_HAS_ID_FLAG(id, PAIR), ECS_INVALID_PARAMETER, NULL);
    return flecs::id(m_iter->world, id);
error:
    return flecs::id();
}

inline flecs::type iter::type() const {
    return flecs::type(m_iter->world, ecs_table_get_type(m_iter->table));
}

inline flecs::table iter::table() const {
    return flecs::table(m_iter->real_world, m_iter->table);
}

inline flecs::table_range iter::range() const {
    return flecs::table_range(m_iter->real_world, m_iter->table, 
        m_iter->offset, m_iter->count);
}

template <typename T, typename A,
    typename std::enable_if<std::is_const<T>::value, void>::type*>
inline flecs::field<A> iter::field(int32_t index) const {
    ecs_assert(!(m_iter->flags & EcsIterCppEach), ECS_INVALID_OPERATION,
        "cannot .field from .each, use .field_at<const %s>(%d, row) instead",
            _::type_name<T>(), index);
    return get_field<A>(index);
}

template <typename T, typename A,
    typename std::enable_if<
        std::is_const<T>::value == false, void>::type*>
inline flecs::field<A> iter::field(int32_t index) const {
    ecs_assert(!(m_iter->flags & EcsIterCppEach), ECS_INVALID_OPERATION,
        "cannot .field from .each, use .field_at<%s>(%d, row) instead",
            _::type_name<T>(), index);
    ecs_assert(!ecs_field_is_readonly(m_iter, index),
        ECS_ACCESS_VIOLATION, NULL);
    return get_field<A>(index);
}

#ifdef FLECS_RULES
inline flecs::entity iter::get_var(int var_id) const {
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
    return flecs::entity(m_iter->world, ecs_iter_get_var(m_iter, var_id));
}

/** Get value of variable by name.
 * Get value of a query variable for current result.
 */
inline flecs::entity iter::get_var(const char *name) const {
    ecs_rule_iter_t *rit = &m_iter->priv.iter.rule;
    const flecs::rule_t *r = rit->rule;
    int var_id = ecs_rule_find_var(r, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    return flecs::entity(m_iter->world, ecs_iter_get_var(m_iter, var_id));
}
#endif

} // namespace flecs
