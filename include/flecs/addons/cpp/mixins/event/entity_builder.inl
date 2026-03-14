/**
 * @file addons/cpp/mixins/event/entity_builder.inl
 * @brief Event entity mixin.
 */

/** Observe an event on an entity.
 *
 * @memberof flecs::entity_builder
 *
 * @param evt The event ID.
 * @param callback The observer callback.
 * @return Reference to self.
 */
template <typename Func>
const Self& observe(flecs::entity_t evt, Func&& callback) const;

/** Observe an event on an entity.
 *
 * @memberof flecs::entity_builder
 *
 * @tparam Evt The event type.
 * @param callback The observer callback.
 * @return Reference to self.
 */
template <typename Evt, typename Func>
const Self& observe(Func&& callback) const;

/** Observe an event on an entity.
 * The event type is deduced from the callback signature.
 *
 * @memberof flecs::entity_builder
 *
 * @param callback The observer callback.
 * @return Reference to self.
 */
template <typename Func>
const Self& observe(Func&& callback) const;
