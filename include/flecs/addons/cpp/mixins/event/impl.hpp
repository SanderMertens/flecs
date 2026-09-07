/**
 * @file addons/cpp/mixins/event/impl.hpp
 * @brief Event implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

// Mixin implementation

inline flecs::event_builder world::event(flecs::entity_t evt) const {
    return flecs::event_builder(world_, evt);
}

template <typename E>
inline flecs::event_builder_typed<E> world::event() const {
    return flecs::event_builder_typed<E>(world_, _::type<E>().id(world_));
}

namespace _ {
    template <typename Event = void, typename Func>
    void entity_observer_create(world_t *world, entity_t event, entity_t entity, Func&& func) {
        using Delegate = entity_observer_delegate<decay_t<Func>, Event>;
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ecs_observer_desc_t desc = {};
        desc.events[0] = event;
        desc.query.terms[0].id = EcsAny;
        desc.query.terms[0].src.id = entity;
        desc.callback = Delegate::run;
        desc.callback_ctx = ctx;
        desc.callback_ctx_free = _::free_obj<Delegate>;
        ecs_add_pair(world, ecs_observer_init(world, &desc), EcsChildOf, entity);
    }
}

template <typename Self>
template <typename Func>
inline const Self& entity_builder<Self>::observe(flecs::entity_t evt, Func&& f) const {
    _::entity_observer_create(world_, evt, id_, FLECS_FWD(f));
    return to_base();
}

template <typename Self>
template <typename Evt, typename Func>
inline const Self& entity_builder<Self>::observe(Func&& f) const {
    _::entity_observer_create<Evt>(world_, _::type<Evt>::id(world_), id_, FLECS_FWD(f));
    return to_base();
}

template <typename Self>
template <typename Func>
inline const Self& entity_builder<Self>::observe(Func&& f) const {
    return this->observe<_::event_from_func_t<Func>>(FLECS_FWD(f));
}

inline void entity_view::emit(flecs::entity evt) const {
    this->emit(evt.id());
}

inline void entity_view::enqueue(flecs::entity evt) const {
    this->enqueue(evt.id());
}

} // namespace flecs
