
/** Create a new event.
 * 
 * @param evt The event id.
 * @return Event builder.
 */
flecs::event_builder event(flecs::entity_t evt) const;

/** Create a new event.
 * 
 * @tparam E The event type.
 * @return Event builder.
 */
template <typename E>
flecs::event_builder_typed<E> event() const;
