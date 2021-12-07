#pragma once

#include "../../utils/node_builder.hpp"
#include "builder_i.hpp"

namespace flecs
{

namespace _ {
    template <typename ... Components>
    using system_builder_base = node_builder<
        system, 
        ecs_system_desc_t, 
        system_builder<Components...>, 
        system_builder_i, 
        Components ...>;
}

template <typename ... Components>
struct system_builder final : _::system_builder_base<Components...> {
    using _::system_builder_base<Components...>::system_builder_base;
};

}
