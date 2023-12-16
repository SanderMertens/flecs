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
    return flecs::event_builder(m_world, evt);
}

template <typename E>
inline flecs::event_builder_typed<E> world::event() const {
    return flecs::event_builder_typed<E>(m_world, _::cpp_type<E>().id(m_world));
}

namespace _ {
    inline void entity_observer_create(
        flecs::world_t *world,
        flecs::entity_t event,
        flecs::entity_t entity,
        ecs_iter_action_t callback,
        void *binding_ctx,
        ecs_ctx_free_t binding_ctx_free) 
    {
        ecs_observer_desc_t desc = {};
        desc.events[0] = event;
        desc.filter.terms[0].id = EcsAny;
        desc.filter.terms[0].src.id = entity;
        desc.callback = callback;
        desc.binding_ctx = binding_ctx;
        desc.binding_ctx_free = binding_ctx_free;

        flecs::entity_t o = ecs_observer_init(world, &desc);
        ecs_add_pair(world, o, EcsChildOf, entity);
    }

    template <typename Func>
    struct entity_observer_factory {
        template <typename Evt, if_t<is_empty<Evt>::value> = 0>
        static void create(
            flecs::world_t *world,
            flecs::entity_t entity,
            Func&& f)
        {
            using Delegate = _::entity_observer_delegate<Func>;
            auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(f));
            entity_observer_create(world, _::cpp_type<Evt>::id(world), entity, Delegate::run, ctx, 
                reinterpret_cast<ecs_ctx_free_t>(_::free_obj<Delegate>));
        }

        template <typename Evt, if_not_t<is_empty<Evt>::value> = 0>
        static void create(
            flecs::world_t *world,
            flecs::entity_t entity,
            Func&& f)
        {
            using Delegate = _::entity_payload_observer_delegate<Func, Evt>;
            auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(f));
            entity_observer_create(world, _::cpp_type<Evt>::id(world), entity, Delegate::run, ctx, 
                reinterpret_cast<ecs_ctx_free_t>(_::free_obj<Delegate>));
        }
    };
}

template <typename Self>
template <typename Func>
inline Self& entity_builder<Self>::observe(flecs::entity_t evt, Func&& f) {
    using Delegate = _::entity_observer_delegate<Func>;
    auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(f));

    _::entity_observer_create(m_world, evt, m_id, Delegate::run, ctx,
        reinterpret_cast<ecs_ctx_free_t>(_::free_obj<Delegate>));

    return to_base();
}

template <typename Self>
template <typename Evt, typename Func>
inline Self& entity_builder<Self>::observe(Func&& f) {
    _::entity_observer_factory<Func>::template create<Evt>(
        m_world, m_id, FLECS_FWD(f));
    return to_base();
}

template <typename Self>
template <typename Func>
inline Self& entity_builder<Self>::observe(Func&& f) {
    return this->observe<_::event_from_func_t<Func>>(FLECS_FWD(f));
}

inline void entity_view::emit(flecs::entity evt) {
    this->emit(evt.id());
}

inline void entity_view::enqueue(flecs::entity evt) {
    this->enqueue(evt.id());
}

} // namespace flecs
