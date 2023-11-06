/**
 * @file addons/cpp/mixins/event/entity_builder.hpp
 * @brief Event entity mixin.
 */

/** Emit event for entity
 * 
 * \memberof flecs::entity_view
 * 
 * @param evt The event to emit.
 */
void emit(flecs::entity_t evt) {
    flecs::world(m_world)
        .event(evt)
        .entity(m_id)
        .emit();
}

/** Emit event for entity
 * 
 * \memberof flecs::entity_view
 * 
 * @tparam Evt The event to emit.
 */
template <typename Evt>
void emit() {
    this->emit(_::cpp_type<Evt>::id(m_world));
}
