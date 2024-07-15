/**
 * @file addons/cpp/mixins/query/mixin.inl
 * @brief Query world mixin.
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_core_queries
 *
 * @{
 */

/** Create a query.
 * 
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query<Comps...> query(Args &&... args) const;

/** Create a query from entity.
 * 
 * @see ecs_query_init
 */
flecs::query<> query(flecs::entity query_entity) const;

/** Create a query builder.
 * 
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query_builder<Comps...> query_builder(Args &&... args) const;

/** Iterate over all entities with components in argument list of function.
 * The function parameter must match the following signature:
 *
 * @code
 * void(*)(T&, U&, ...)
 * @endcode
 *
 * or:
 *
 * @code
 * void(*)(flecs::entity, T&, U&, ...)
 * @endcode
 * 
 */
template <typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided component.
 * The function parameter must match the following signature:
 *
 * @code
 * void(*)(T&)
 * @endcode
 *
 * or:
 *
 * @code
 * void(*)(flecs::entity, T&)
 * @endcode
 * 
 */
template <typename T, typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided (component) id. */
template <typename Func>
void each(flecs::id_t term_id, Func&& func) const;

/** @} */
