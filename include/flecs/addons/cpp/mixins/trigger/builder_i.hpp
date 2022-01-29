#pragma once

#include "../term/builder_i.hpp"

namespace flecs 
{

// Trigger builder interface
template<typename Base, typename ... Components>
struct trigger_builder_i : term_builder_i<Base> {
    using Class = trigger_builder_i<Base, Components...>;
    using BaseClass = term_builder_i<Base>;

    trigger_builder_i(ecs_trigger_desc_t *desc) 
        : BaseClass(&desc->term)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify the event(s) for when the trigger should run.
     * @param evt The event.
     */
    Base& event(entity_t evt) {
        m_desc->events[m_event_count ++] = evt;
        return *this;
    }

    /** Specify the event(s) for when the trigger should run.
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

    /** Associate trigger with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
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

    ecs_trigger_desc_t *m_desc;
    int32_t m_event_count;
};

}
