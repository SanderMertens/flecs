/**
 * @file addons/cpp/mixins/rule/iterable.inl
 * @brief Rule iterable mixin.
 */

/**
 * \memberof flecs::iter
 * \ingroup cpp_addons_rules
 */

iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
    ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
    ecs_iter_set_var(&m_it, var_id, value);
    return *this;
}

iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
    ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_rule_iter_t *rit = &m_it.priv.iter.rule;
    int var_id = ecs_rule_find_var(rit->rule, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    ecs_iter_set_var(&m_it, var_id, value);
    return *this;
}

/** @} */
