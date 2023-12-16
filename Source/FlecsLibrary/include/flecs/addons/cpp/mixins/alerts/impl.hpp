/**
 * @file addons/cpp/mixins/alerts/impl.hpp
 * @brief Alerts module implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

template <typename ... Components>
struct alert final : entity
{
    using entity::entity;

    explicit alert() {
        m_id = 0;
        m_world = nullptr;
    }

    explicit alert(flecs::world_t *world, ecs_alert_desc_t *desc) 
    {
        m_world = world;
        m_id = ecs_alert_init(world, desc);

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }
};

inline alerts::alerts(flecs::world& world) {
    /* Import C module  */
    FlecsAlertsImport(world);

    world.entity<alerts::Alert>("::flecs::alerts::Alert");
    world.entity<alerts::Info>("::flecs::alerts::Info");
    world.entity<alerts::Warning>("::flecs::alerts::Warning");
    world.entity<alerts::Error>("::flecs::alerts::Error");
}

template <typename... Comps, typename... Args>
inline flecs::alert_builder<Comps...> world::alert(Args &&... args) const {
    return flecs::alert_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

}
