#pragma once

#include "../query/builder_i.hpp"

namespace flecs {

// Query builder interface
template<typename Base>
struct pipeline_builder_i : query_builder_i<Base> {
    pipeline_builder_i(ecs_pipeline_desc_t *desc, int32_t term_index = 0) 
        : query_builder_i<Base>(&desc->query, term_index)
        , m_desc(desc) { }

private:
    ecs_pipeline_desc_t *m_desc;
};

}
