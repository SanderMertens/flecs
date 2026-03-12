/**
 * @file addons/cpp/mixins/observer/impl.hpp
 * @brief Observer implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

/** Observer.
 *
 * @ingroup cpp_observers
 */
struct observer final : entity
{
    using entity::entity;

    /** Default constructor. */
    explicit observer() : entity() { }

    /** Construct from a world and an observer descriptor. */
    observer(flecs::world_t *world, ecs_observer_desc_t *desc) {
        world_ = world;
        id_ = ecs_observer_init(world, desc);
    }

    /** Set the observer context. */
    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity = id_;
        desc.ctx = ctx;
        ecs_observer_init(world_, &desc);
    }

    /** Get the observer context. */
    void* ctx() const {
        return ecs_observer_get(world_, id_)->ctx;
    }

    /** Get the query for this observer. */
    flecs::query<> query() const {
        return flecs::query<>(ecs_observer_get(world_, id_)->query);
    }
};

/** Mixin implementation. */
inline observer world::observer(flecs::entity e) const {
    return flecs::observer(world_, e);
}

template <typename... Comps, typename... Args>
inline observer_builder<Comps...> world::observer(Args &&... args) const {
    return flecs::observer_builder<Comps...>(world_, FLECS_FWD(args)...);
}

} // namespace flecs
