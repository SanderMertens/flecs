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
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ecs_observer_desc_t desc = {};
        desc.run = Delegate::run;
        desc.run_ctx = ctx;
        desc.run_ctx_free = _::free_obj<Delegate>;
        ecs_observer_update(world_, id_, &desc);
        return *this;
    }

    /** Replace the observer's each callback. */
    template <typename Func>
    observer& each(Func&& func) {
        using CallbackComponents =
            typename _::each_callback_args<arg_list_t<Func>>::type;
        return each_callback(CallbackComponents{}, FLECS_FWD(func));
    }

    /** Replace the observer's run callback and use an each callback for
     * iteration. */
    template <typename Func>
    observer& run_each(Func&& func) {
        using CallbackComponents =
            typename _::each_callback_args<arg_list_t<Func>>::type;
        return run_each_callback(CallbackComponents{}, FLECS_FWD(func));
    }

    /** Get the query for this observer. */
    flecs::query<> query() const {
        return flecs::query<>(ecs_observer_get(world_, id_)->query);
    }

private:
    template <typename ... CallbackComponents, typename Func>
    observer& each_callback(_::arg_list<CallbackComponents...>, Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, CallbackComponents...>;
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ecs_observer_desc_t desc = {};
        desc.callback = Delegate::run;
        desc.callback_ctx = ctx;
        desc.callback_ctx_free = _::free_obj<Delegate>;
        ecs_observer_update(world_, id_, &desc);
        return *this;
    }

    template <typename ... CallbackComponents, typename Func>
    observer& run_each_callback(_::arg_list<CallbackComponents...>, Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, CallbackComponents...>;
        auto ctx = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ecs_observer_desc_t desc = {};
        desc.run = Delegate::run_each;
        desc.run_ctx = ctx;
        desc.run_ctx_free = _::free_obj<Delegate>;
        ecs_observer_update(world_, id_, &desc);
        return *this;
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
