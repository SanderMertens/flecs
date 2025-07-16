/**
 * @file addons/cpp/mixins/enum/impl.hpp
 * @brief Enum implementation.
 */

#pragma once

namespace flecs {

template <typename E>
inline E entity_view::to_constant() const {
#ifdef FLECS_META
    using U = typename std::underlying_type<E>::type;
    const E* ptr = static_cast<const E*>(ecs_get_id(world_, id_, 
        ecs_pair(flecs::Constant, _::type<U>::id(world_))));
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, "entity is not a constant");
    return ptr[0];
#else
    ecs_assert(false, ECS_UNSUPPORTED,
        "operation not supported without FLECS_META addon");
    return E();
#endif
}

template <typename TInt>
TInt entity_view::to_constant(flecs::entity_t type_id) const
{
    #ifdef FLECS_META
    const TInt* ptr = static_cast<const TInt*>(ecs_get_id(world_, id_, 
        ecs_pair(flecs::Constant, _::type<TInt>::id(world_))));
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, "entity is not a constant");
    return static_cast<TInt>(ptr[0]);
    #else
    ecs_assert(false, ECS_UNSUPPORTED,
        "operation not supported without FLECS_META addon");
    return TInt();
    #endif
}

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::to_entity(E constant) const {
    const auto& et = enum_type<E>(world_);
    return flecs::entity(world_, et.entity(constant));
}

}