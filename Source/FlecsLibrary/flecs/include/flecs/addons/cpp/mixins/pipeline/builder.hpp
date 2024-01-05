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
 * \ingroup cpp_pipelines
 */
template <typename ... Components>
struct pipeline_builder final : _::pipeline_builder_base<Components...> {
    pipeline_builder(flecs::world_t* world, flecs::entity_t id = 0)
        : _::pipeline_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        this->m_desc.entity = id;
    }
};

}
