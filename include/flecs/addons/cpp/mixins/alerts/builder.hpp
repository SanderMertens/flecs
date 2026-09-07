/**
 * @file addons/cpp/mixins/alerts/builder.hpp
 * @brief Alert builder.
 */

#pragma once

#include "../../utils/builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using alert_builder_base = builder<
        alert<Components...>, ecs_alert_desc_t, alert_builder<Components...>,
        alert_builder_i, Components ...>;
}

/** Alert builder.
 * 
 * @ingroup cpp_addons_alerts
 */
template <typename ... Components>
struct alert_builder final : _::alert_builder_base<Components...> {
    alert_builder(flecs::world_t* world, const char *name = nullptr)
        : _::alert_builder_base<Components...>(world)
    {
        _::populate_signature<Components...>(world, this);
        this->set_name(name);
    }
};

}
