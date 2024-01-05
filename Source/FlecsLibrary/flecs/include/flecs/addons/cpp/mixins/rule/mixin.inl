/**
 * @file addons/cpp/mixins/rule/mixin.inl
 * @brief Rule world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_rules
 */

/** Create a rule.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule<Comps...> rule(Args &&... args) const;

/** Create a subrule.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule<Comps...> rule(flecs::rule_base& parent, Args &&... args) const;

/** Create a rule builder.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule_builder<Comps...> rule_builder(Args &&... args) const;

/** @} */
