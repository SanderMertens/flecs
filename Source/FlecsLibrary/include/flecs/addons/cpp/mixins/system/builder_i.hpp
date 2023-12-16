/**
 * @file addons/cpp/mixins/system/builder_i.hpp
 * @brief System builder interface.
 */

#pragma once

#include "../query/builder_i.hpp"

namespace flecs 
{

/** System builder interface.
 * 
 * \ingroup cpp_addons_systems
 */
template<typename Base, typename ... Components>
struct system_builder_i : query_builder_i<Base, Components ...> {
private:
    using BaseClass = query_builder_i<Base, Components ...>;

public:
    system_builder_i(ecs_system_desc_t *desc) 
        : BaseClass(&desc->query)
        , m_desc(desc) { }

    /** Specify in which phase the system should run.
     *
     * @param phase The phase.
     */
    Base& kind(entity_t phase) {
        flecs::entity_t cur_phase = ecs_get_target(
            world_v(), m_desc->entity, EcsDependsOn, 0);
        if (cur_phase) {
            ecs_remove_id(world_v(), m_desc->entity, ecs_dependson(cur_phase));
            ecs_remove_id(world_v(), m_desc->entity, cur_phase);
        }
        if (phase) {
            ecs_add_id(world_v(), m_desc->entity, ecs_dependson(phase));
            ecs_add_id(world_v(), m_desc->entity, phase);
        }
        return *this;
    }

    /** Specify in which phase the system should run.
     *
     * @tparam Phase The phase.
     */
    template <typename Phase>
    Base& kind() {
        return this->kind(_::cpp_type<Phase>::id(world_v()));
    }

    /** Specify whether system can run on multiple threads.
     *
     * @param value If false system will always run on a single thread.
     */
    Base& multi_threaded(bool value = true) {
        m_desc->multi_threaded = value;
        return *this;
    }

    /** Specify whether system should be ran in staged context.
     *
     * @param value If false system will always run staged.
     */
    Base& no_readonly(bool value = true) {
        m_desc->no_readonly = value;
        return *this;
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    Base& interval(ecs_ftime_t interval) {
        m_desc->interval = interval;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    Base& rate(const entity_t tick_source, int32_t rate) {
        m_desc->rate = rate;
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    Base& rate(int32_t rate) {
        m_desc->rate = rate;
        return *this;
    }

    /** Set tick source.
     * This operation sets a shared tick source for the system.
     *
     * @param tick_source The tick source to use for the system.
     */
    Base& tick_source(flecs::entity_t tick_source) {
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

    /** Set system run callback */
    Base& run(ecs_iter_action_t action) {
        m_desc->run = action;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_system_desc_t *m_desc;
};

/** @} */

}
