/**
 * @file addons/cpp/mixins/event/entity_builder.hpp
 * @brief Event entity mixin.
 */

/** Observe event on entity
 * 
 * \memberof flecs::entity_builder
 * 
 * @param evt The event id.
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Func>
Self& observe(flecs::entity_t evt, Func&& callback);

/** Observe event on entity
 * 
 * \memberof flecs::entity_builder
 * 
 * @tparam Evt The event type.
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Evt, typename Func>
Self& observe(Func&& callback);

/** Observe event on entity
 * 
 * \memberof flecs::entity_builder
 *
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Func>
Self& observe(Func&& callback);


