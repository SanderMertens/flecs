/**
 * @file addons/cpp/mixins/query/mixin.inl
 * @brief Query world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_core_queries
 */

/** Create a query.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query<Comps...> query(Args &&... args) const;

/** Create a subquery.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query<Comps...> query(flecs::query_base& parent, Args &&... args) const;

/** Create a query builder.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query_builder<Comps...> query_builder(Args &&... args) const;

/** @} */
