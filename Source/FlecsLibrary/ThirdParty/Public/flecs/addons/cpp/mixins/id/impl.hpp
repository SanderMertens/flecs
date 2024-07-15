/**
 * @file addons/cpp/mixins/id/impl.hpp
 * @brief Id class implementation.
 */

#pragma once

namespace flecs {

inline flecs::entity id::entity() const {
    ecs_assert(!is_pair(), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!flags(), ECS_INVALID_OPERATION, NULL);
    return flecs::entity(world_, id_);
}

inline flecs::entity id::flags() const {
    return flecs::entity(world_, id_ & ECS_ID_FLAGS_MASK);
}

inline flecs::entity id::first() const {
    ecs_assert(is_pair(), ECS_INVALID_OPERATION, NULL);

    flecs::entity_t e = ECS_PAIR_FIRST(id_);
    if (world_) {
        return flecs::entity(world_, ecs_get_alive(world_, e));
    } else {
        return flecs::entity(e);
    }
}

inline flecs::entity id::second() const {
    flecs::entity_t e = ECS_PAIR_SECOND(id_);
    if (world_) {
        return flecs::entity(world_, ecs_get_alive(world_, e));
    } else {
        return flecs::entity(e);
    }
}

inline flecs::entity id::add_flags(flecs::id_t flags) const {
    return flecs::entity(world_, id_ | flags);
}

inline flecs::entity id::remove_flags(flecs::id_t flags) const {
    (void)flags;
    ecs_assert((id_ & ECS_ID_FLAGS_MASK) == flags, ECS_INVALID_PARAMETER, NULL);
    return flecs::entity(world_, id_ & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_flags() const {
    return flecs::entity(world_, id_ & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_generation() const {
    return flecs::entity(world_, static_cast<uint32_t>(id_));
}

inline flecs::world id::world() const {
    return flecs::world(world_);
}

inline flecs::entity id::type_id() const {
    return flecs::entity(world_, ecs_get_typeid(world_, id_));
}


// Id mixin implementation

template <typename T>
inline flecs::id world::id() const {
    return flecs::id(world_, _::type<T>::id(world_));
}

template <typename ... Args>
inline flecs::id world::id(Args&&... args) const {
    return flecs::id(world_, FLECS_FWD(args)...);
}

template <typename First, typename Second>
inline flecs::id world::pair() const {
    return flecs::id(
        world_, 
        ecs_pair(
            _::type<First>::id(world_), 
            _::type<Second>::id(world_)));
}

template <typename First>
inline flecs::id world::pair(entity_t o) const {
    ecs_assert(!ECS_IS_PAIR(o), ECS_INVALID_PARAMETER, 
        "cannot create nested pairs");

    return flecs::id(
        world_,
        ecs_pair(
            _::type<First>::id(world_), 
            o));
}

inline flecs::id world::pair(entity_t r, entity_t o) const {
    ecs_assert(!ECS_IS_PAIR(r) && !ECS_IS_PAIR(o), ECS_INVALID_PARAMETER, 
        "cannot create nested pairs");

    return flecs::id(
        world_,
        ecs_pair(r, o));
}

}
