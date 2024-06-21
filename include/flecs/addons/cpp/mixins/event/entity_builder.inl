/**
 * @file addons/cpp/mixins/event/entity_builder.inl
 * @brief Event entity mixin.
 */

/** Observe event on entity
 * 
 * @memberof flecs::entity_builder
 * 
 * @param evt The event id.
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Func>
const Self& observe(flecs::entity_t evt, Func&& callback) const;

/** Observe event on entity
 * 
 * @memberof flecs::entity_builder
 * 
 * @tparam Evt The event type.
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Evt, typename Func>
const Self& observe(Func&& callback) const;

/** Observe event on entity
 * 
 * @memberof flecs::entity_builder
 *
 * @param callback The observer callback.
 * @return Event builder.
 */
template <typename Func>
const Self& observe(Func&& callback) const;


