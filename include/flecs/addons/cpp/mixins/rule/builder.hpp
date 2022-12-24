/**
 * @file addons/cpp/mixins/rule/builder.hpp
 * @brief Rule builder.
 */

#pragma once

#include "../filter/builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using rule_builder_base = builder<
        rule, ecs_filter_desc_t, rule_builder<Components...>, 
        filter_builder_i, Components ...>;
}

/** Rule builder.
 * 
 * \ingroup cpp_addons_rules
 */
template <typename ... Components>
struct rule_builder final : _::rule_builder_base<Components...> {
    rule_builder(flecs::world_t* world, const char *name = nullptr)
        : _::rule_builder_base<Components...>(world)
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
