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
    inline void entity_observer_create(
        flecs::world_t *world,
        flecs::entity_t event,
        flecs::entity_t entity,
        ecs_iter_action_t callback,
        void *callback_ctx,
        ecs_ctx_free_t callback_ctx_free) 
    {
        ecs_observer_desc_t desc = {};
        desc.events[0] = event;
        desc.query.terms[0].id = EcsAny;
        desc.query.terms[0].src.id = entity;
        desc.callback = callback;
        desc.callback_ctx = callback_ctx;
        desc.callback_ctx_free = callback_ctx_free;

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
            entity_observer_create(world, _::type<Evt>::id(world), entity, Delegate::run, ctx, 
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
            entity_observer_create(world, _::type<Evt>::id(world), entity, Delegate::run, ctx, 
                reinterpret_cast<ecs_ctx_free_t>(_::free_obj<Delegate>));
        }
    };
}

template <typename Self>
template <typename Func>
inline const Self& entity_builder<Self>::observe(flecs::entity_t evt, Func&& f) const {
    using Delegate = _::entity_observer_delegate<Func>;
    auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(f));

    _::entity_observer_create(world_, evt, id_, Delegate::run, ctx,
        reinterpret_cast<ecs_ctx_free_t>(_::free_obj<Delegate>));

    return to_base();
}

template <typename Self>
template <typename Evt, typename Func>
inline const Self& entity_builder<Self>::observe(Func&& f) const {
    _::entity_observer_factory<Func>::template create<Evt>(
        world_, id_, FLECS_FWD(f));
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
