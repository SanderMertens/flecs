/**
 * @file addons/cpp/mixins/pipeline/builder.hpp
 * @brief Pipeline builder.
 */

#pragma once

#include "../../utils/builder.hpp"
#include "builder_i.hpp"

namespace flecs {
namespace _ {
    template <typename ... Components>
    using pipeline_builder_base = builder<
        pipeline, ecs_pipeline_desc_t, pipeline_builder<Components...>, 
        pipeline_builder_i, Components ...>;
}

/** Pipeline builder.
 * 
 * @ingroup cpp_pipelines
 */
template <typename ... Components>
struct pipeline_builder final : _::pipeline_builder_base<Components...> {
    /** Construct a pipeline builder. */
    pipeline_builder(flecs::world_t* world, flecs::entity_t id = 0)
        : _::pipeline_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        this->desc_.entity = id;
    }

    /** Construct a named pipeline builder. */
    pipeline_builder(flecs::world_t* world, const char *name)
        : _::pipeline_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->desc_.entity = ecs_entity_init(world, &entity_desc);
        }
    }
};

}
