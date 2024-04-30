/**
 * @file addons/cpp/mixins/observer/builder_i.hpp
 * @brief Observer builder interface.
 */

#pragma once

#include "../query/builder_i.hpp"

namespace flecs {

/** Observer builder interface.
 * 
 * @ingroup cpp_observers
 */
template<typename Base, typename ... Components>
struct observer_builder_i : query_builder_i<Base, Components ...> {
    using BaseClass = query_builder_i<Base, Components ...>;
    observer_builder_i()
        : BaseClass(nullptr)
        , desc_(nullptr)
        , event_count_(0) { }

    observer_builder_i(ecs_observer_desc_t *desc) 
        : BaseClass(&desc->query)
        , desc_(desc)
        , event_count_(0) { }

    /** Specify the event(s) for when the observer should run.
     * @param evt The event.
     */
    Base& event(entity_t evt) {
        desc_->events[event_count_ ++] = evt;
        return *this;
    }

    /** Specify the event(s) for when the observer should run.
     * @tparam E The event.
     */
    template <typename E>
    Base& event() {
        desc_->events[event_count_ ++] = _::type<E>().id(world_v());
        return *this;
    }

    /** Invoke observer for anything that matches its query on creation */
    Base& yield_existing(bool value = true) {
        desc_->yield_existing = value;
        return *this;
    }

    /** Set observer context */
    Base& ctx(void *ptr) {
        desc_->ctx = ptr;
        return *this;
    }

    /** Set observer run callback */
    Base& run(ecs_iter_action_t action) {
        desc_->run = action;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() override = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_observer_desc_t *desc_;
    int32_t event_count_;
};

}
