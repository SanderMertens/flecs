#pragma once

#include "../filter/builder_i.hpp"

namespace flecs {

// Observer builder interface
template<typename Base, typename ... Components>
struct observer_builder_i : filter_builder_i<Base, Components ...> {
    using BaseClass = filter_builder_i<Base, Components ...>;
    observer_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_event_count(0) { }

    observer_builder_i(ecs_observer_desc_t *desc) 
        : BaseClass(&desc->filter)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify the event(s) for when the observer should run.
     * @param evt The event.
     */
    Base& event(entity_t evt) {
        m_desc->events[m_event_count ++] = evt;
        return *this;
    }

    /** Specify the event(s) for when the observer should run.
     * @tparam E The event.
     */
    template <typename E>
    Base& event() {
        m_desc->events[m_event_count ++] = _::cpp_type<E>().id(world_v());
        return *this;
    }

    /** Invoke observer for anything that matches its filter on creation */
    Base& yield_existing(bool value = true) {
        m_desc->yield_existing = value;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }    

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_observer_desc_t *m_desc;
    int32_t m_event_count;
};

}
