/**
 * @file addons/cpp/mixins/entity_ranges/impl.hpp
 * @brief Entity ranges implementation.
 */

#pragma once

namespace flecs {

inline const ecs_entity_range_t* world::range_new(
    uint32_t min,
    uint32_t max) const
{
    return ecs_entity_range_new(world_, min, max);
}

inline void world::range_set(const ecs_entity_range_t *range) const {
    ecs_entity_range_set(world_, range);
}

inline const ecs_entity_range_t* world::range_get() const {
    return ecs_entity_range_get(world_);
}

}
