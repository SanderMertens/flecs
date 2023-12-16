/**
 * @file addons/cpp/mixins/event/mixin.inl
 * @brief Event world mixin.
 */

/**
 * @defgroup cpp_addons_event Events
 * @brief API for emitting events.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Create a new event.
 * 
 * \memberof flecs::world
 * 
 * @param evt The event id.
 * @return Event builder.
 */
flecs::event_builder event(flecs::entity_t evt) const;

/** Create a new event.
 * 
 * \memberof flecs::world
 * 
 * @tparam E The event type.
 * @return Event builder.
 */
template <typename E>
flecs::event_builder_typed<E> event() const;

/** @} */
