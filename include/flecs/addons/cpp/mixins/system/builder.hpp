/**
 * @file addons/cpp/mixins/system/builder.hpp
 * @brief System builder.
 */

#pragma once

#include "../../utils/node_builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using system_builder_base = node_builder<
        system, ecs_system_desc_t, system_builder<Components...>, 
        system_builder_i, Components ...>;
}

/** System builder.
 * 
 * \ingroup cpp_addons_systems
 */
template <typename ... Components>
struct system_builder final : _::system_builder_base<Components...> {
    system_builder(flecs::world_t* world, const char *name = nullptr)
        : _::system_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);

#ifdef FLECS_PIPELINE
        ecs_add_id(world, this->m_desc.entity, ecs_dependson(flecs::OnUpdate));
        ecs_add_id(world, this->m_desc.entity, flecs::OnUpdate);
#endif
    }
};

}
