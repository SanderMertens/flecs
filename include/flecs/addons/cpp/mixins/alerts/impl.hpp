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
        id_ = 0;
        world_ = nullptr;
    }

    explicit alert(flecs::world_t *world, ecs_alert_desc_t *desc) {
        world_ = world;
        id_ = ecs_alert_init(world, desc);
    }
};

inline alerts::alerts(flecs::world& world) {
    world.import<metrics>();

    /* Import C module  */
    FlecsAlertsImport(world);

    world.component<AlertsActive>();
    world.component<Instance>();

    world.entity<alerts::Alert>("::flecs::alerts::Alert");
    world.entity<alerts::Info>("::flecs::alerts::Info");
    world.entity<alerts::Warning>("::flecs::alerts::Warning");
    world.entity<alerts::Error>("::flecs::alerts::Error");
}

template <typename... Comps, typename... Args>
inline flecs::alert_builder<Comps...> world::alert(Args &&... args) const {
    return flecs::alert_builder<Comps...>(world_, FLECS_FWD(args)...);
}

}
