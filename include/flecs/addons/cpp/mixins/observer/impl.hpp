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
    observer(flecs::world_t *world, const ecs_observer_desc_t *desc) {
        world_ = world;
        id_ = ecs_observer_init(world, desc);
    }

    /** Set the observer context. */
    observer& ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.ctx = ctx;
        ecs_observer_update(world_, id_, &desc);
        return *this;
    }

    /** Get the observer context. */
    void* ctx() const {
        return ecs_observer_get(world_, id_)->ctx;
    }

    /** Replace the observer's run callback. */
    template <typename Func>
    observer& run(Func&& func) {
        using Delegate = typename _::run_delegate<
            typename std::decay<Func>::type>;
        ecs_observer_desc_t desc = {};
        _::set_callback<Delegate, Delegate::run, true>(desc, FLECS_FWD(func));
        ecs_observer_update(world_, id_, &desc);
        return *this;
    }

    /** Replace the observer's each callback. */
    template <typename Func>
    observer& each(Func&& func) {
        using CallbackComponents =
            typename _::each_callback_args<arg_list_t<Func>>::type;
        ecs_observer_desc_t desc = {};
        _::set_each_callback<false>(desc, FLECS_FWD(func), CallbackComponents{});
        ecs_observer_update(world_, id_, &desc);
        return *this;
    }

    /** Replace the observer's run callback and use an each callback for
     * iteration. */
    template <typename Func>
    observer& run_each(Func&& func) {
        using CallbackComponents =
            typename _::each_callback_args<arg_list_t<Func>>::type;
        ecs_observer_desc_t desc = {};
        _::set_each_callback<true>(desc, FLECS_FWD(func), CallbackComponents{});
        ecs_observer_update(world_, id_, &desc);
        return *this;
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
