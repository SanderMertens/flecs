/**
 * @file addons/cpp/mixins/filter/builder.hpp
 * @brief Filter builder.
 */

#pragma once

#include "../../utils/builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using filter_builder_base = builder<
        filter, ecs_filter_desc_t, filter_builder<Components...>, 
        filter_builder_i, Components ...>;
}

/** Filter builder.
 * 
 * \ingroup cpp_filters
 */
template <typename ... Components>
struct filter_builder final : _::filter_builder_base<Components...> {
    filter_builder(flecs::world_t* world, const char *name = nullptr)
        : _::filter_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::",
            entity_desc.root_sep = "::",
            this->m_desc.entity = ecs_entity_init(world, &entity_desc);
        }
    }
};

}
