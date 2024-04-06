/**
 * @file addons/cpp/mixins/event/entity_view.inl
 * @brief Event entity mixin.
 */

/** Emit event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @param evt The event to emit.
 */
void emit(flecs::entity_t evt) {
    flecs::world(world_)
        .event(evt)
        .entity(id_)
        .emit();
}

/** Emit event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @param evt The event to emit.
 */
void emit(flecs::entity evt);

/** Emit event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @tparam Evt The event to emit.
 */
template <typename Evt, if_t<is_empty<Evt>::value> = 0>
void emit() {
    this->emit(_::type<Evt>::id(world_));
}

/** Emit event with payload for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @tparam Evt The event to emit.
 */
template <typename Evt, if_not_t<is_empty<Evt>::value> = 0>
void emit(const Evt& payload) {
    flecs::world(world_)
        .event(_::type<Evt>::id(world_))
        .entity(id_)
        .ctx(&payload)
        .emit();
}


/** Enqueue event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @param evt The event to enqueue.
 */
void enqueue(flecs::entity_t evt) {
    flecs::world(world_)
        .event(evt)
        .entity(id_)
        .enqueue();
}

/** Enqueue event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @param evt The event to enqueue.
 */
void enqueue(flecs::entity evt);

/** Enqueue event for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @tparam Evt The event to enqueue.
 */
template <typename Evt, if_t<is_empty<Evt>::value> = 0>
void enqueue() {
    this->enqueue(_::type<Evt>::id(world_));
}

/** Enqueue event with payload for entity.
 * 
 * @memberof flecs::entity_view
 * 
 * @tparam Evt The event to enqueue.
 */
template <typename Evt, if_not_t<is_empty<Evt>::value> = 0>
void enqueue(const Evt& payload) {
    flecs::world(world_)
        .event(_::type<Evt>::id(world_))
        .entity(id_)
        .ctx(&payload)
        .enqueue();
}
