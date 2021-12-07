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

template <typename ... Components>
struct filter_builder final : _::filter_builder_base<Components...> {
    filter_builder(flecs::world_t* world)
        : _::filter_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}
