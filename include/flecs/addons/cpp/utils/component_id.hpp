#pragma once

namespace flecs {
namespace _ {

template <typename T = void, bool Sparse = false, typename Owner = void>
struct component_id {
    using type = T;
    static constexpr bool sparse = Sparse;
    flecs::id_t id;
    flecs::entity_t entity;

    flecs::entity_t owner(world_t *world) const {
        if constexpr (std::is_void_v<Owner>) {
            (void)world;
            return entity;
        } else {
            return _::type<Owner>::id(world);
        }
    }
};

template <typename T>
flecs::entity_t entity_id(world_t *world, T value) {
    if constexpr (is_enum_v<T>) {
        return flecs::enum_type<T>(world).entity(value);
    } else {
        (void)world;
        return value;
    }
}

template <typename T>
auto resolve_id(world_t *world, arg_list<T>) {
    using A = actual_type_t<T>;
    auto id = _::type<T>::id(world);
    if constexpr (is_pair_v<T>) {
        return component_id<A, false, pair_first_t<T>>{id, 0};
    } else {
        return component_id<A, true>{id, id};
    }
}

template <typename First, typename Second>
auto resolve_id(world_t *world, arg_list<First, Second>) {
    return resolve_id(world, arg_list<flecs::pair<First, Second>>{});
}

template <typename First, typename Second>
auto resolve_id(world_t *world, arg_list<First>, Second second) {
    auto first = _::type<First>::id(world);
    return component_id<First>{ecs_pair(first, entity_id(world, second)), first};
}

template <typename T>
auto resolve_id(world_t *world, arg_list<>, T value) {
    if constexpr (is_enum_v<T>) {
        return resolve_id(world, arg_list<T>{}, value);
    } else {
        (void)world;
        return component_id<>{value, value};
    }
}

inline auto resolve_id(world_t*, arg_list<>, flecs::entity_t first, flecs::entity_t second) {
    return component_id<>{ecs_pair(first, second), first};
}

template <typename... T, typename... Args>
auto make_id(world_t *world, Args... args) {
    return resolve_id(world, arg_list<T...>{}, args...);
}

template <typename Second>
auto second_id(world_t *world, flecs::entity_t first) {
    auto second = _::type<Second>::id(world);
    auto id = ecs_pair(first, second);
    ecs_assert(ecs_get_type_info(world, id) &&
        ecs_get_type_info(world, id)->component == second, ECS_INVALID_PARAMETER,
        "type of pair is not Second");
    return component_id<Second>{id, first};
}

template <typename First, typename Second>
auto second_id(world_t *world) {
    static_assert(is_empty_v<First>, "first element of pair must be a tag");
    return make_id<First, Second>(world);
}

template <bool Mutable, bool Required, typename Id>
decltype(auto) get_component(world_t *world, flecs::entity_t entity, Id id) {
    using T = typename Id::type;
    using A = conditional_t<Mutable, T, const T>;
    auto get = [&]() {
        if constexpr (Id::sparse && !std::is_void_v<T>) {
            if constexpr (Mutable) {
                return _::get_mut_ptr<T>(world, entity, id.id);
            } else {
                return _::get_ptr<T>(world, entity, id.id);
            }
        } else if constexpr (Mutable) {
            return ecs_get_mut_id(world, entity, id.id);
        } else {
            return ecs_get_id(world, entity, id.id);
        }
    };
    if constexpr (!std::is_void_v<T>) {
        ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
    }
    A *ptr = static_cast<A*>(get());
    if constexpr (Required) {
        ecs_assert(ptr != nullptr, ECS_INVALID_OPERATION,
            "entity does not have component (use try_get or try_get_mut)");
    }
    if constexpr (Required && !std::is_void_v<T>) {
        return *ptr;
    } else {
        return ptr;
    }
}

}
}
