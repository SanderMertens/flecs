/**
 * @file addons/cpp/mixins/filter/mixin.inl
 * @brief Query world mixin.
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_core_filters
 *
 * @{
 */

/** Create a filter.
 * 
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::filter<Comps...> filter(Args &&... args) const;

/** Create a filter builder.
 * 
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::filter_builder<Comps...> filter_builder(Args &&... args) const;

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
