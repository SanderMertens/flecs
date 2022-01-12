#pragma once

#include "../filter/builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using rule_builder_base = builder<
        rule, ecs_filter_desc_t, rule_builder<Components...>, 
        filter_builder_i, Components ...>;
}

template <typename ... Components>
struct rule_builder final : _::rule_builder_base<Components...> {
    rule_builder(flecs::world_t* world)
        : _::rule_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}
