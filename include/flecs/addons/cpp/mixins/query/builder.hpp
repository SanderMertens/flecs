#pragma once

#include "../../utils/builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using query_builder_base = builder<
        query, ecs_query_desc_t, query_builder<Components...>, 
        query_builder_i, Components ...>;
}

template <typename ... Components>
struct query_builder final : _::query_builder_base<Components...> {
    query_builder(flecs::world_t* world)
        : _::query_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}
