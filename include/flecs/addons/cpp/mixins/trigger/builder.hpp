#pragma once

#include "../../utils/node_builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using trigger_builder_base = node_builder<
        trigger, 
        ecs_trigger_desc_t, 
        trigger_builder<Components...>, 
        trigger_builder_i, 
        Components ...>;
}

template <typename ... Components>
struct trigger_builder final : _::trigger_builder_base<Components...> {
    trigger_builder(flecs::world_t* world, const char *name = nullptr)
        : _::trigger_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}
