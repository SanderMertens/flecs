/**
 * @file addons/cpp/mixins/script/impl.hpp
 * @brief Script implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

inline flecs::entity script_builder::run() const {
    ecs_entity_t e = ecs_script_init(world_, &desc_);
    return flecs::entity(world_, e);
}

}
