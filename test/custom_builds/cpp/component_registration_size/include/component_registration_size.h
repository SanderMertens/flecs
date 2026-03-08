#ifndef COMPONENT_REGISTRATION_SIZE_H
#define COMPONENT_REGISTRATION_SIZE_H

/* This generated file contains includes for project dependencies */
#include "component_registration_size/bake_config.h"

#include <initializer_list>
#include <utility>

template <size_t I>
struct component_registration_size_component {
    int32_t value;
};

template <size_t... I>
inline void component_registration_size_register_components(
    flecs::world& world,
    std::index_sequence<I...>)
{
    (void)std::initializer_list<int>{
        (world.component<component_registration_size_component<I>>(), 0)...
    };
}

inline void component_registration_size_register_300_components(
    flecs::world& world)
{
    component_registration_size_register_components(
        world, std::make_index_sequence<300>{});
}

#endif
