#pragma once

#include "filter_i.hpp"

namespace flecs 
{

// Observer builder interface
template<typename Base, typename ... Components>
class observer_builder_i : public filter_builder_i<Base, Components ...> {
    using BaseClass = filter_builder_i<Base, Components ...>;
public:
    observer_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_event_count(0) { }

    observer_builder_i(ecs_observer_desc_t *desc) 
        : BaseClass(&desc->filter)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify when the event(s) for which the trigger run.
     * @param kind The kind that specifies when the system should be ran.
     */
    Base& event(entity_t kind) {
        m_desc->events[m_event_count ++] = kind;
        return *this;
    }

    /** Associate observer with entity */
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

    ecs_observer_desc_t *m_desc;
    int32_t m_event_count;
};

}
