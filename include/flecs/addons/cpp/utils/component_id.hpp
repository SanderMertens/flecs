#pragma once

namespace flecs {
namespace _ {

template <typename T = void, bool Sparse = false, typename Owner = void, typename Construct = T>
struct component_id {
    using type = T;
    using construct_type = Construct;
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
        auto entity = flecs::enum_type<T>(world).entity(value);
        ecs_assert(entity, ECS_INVALID_PARAMETER, "enum constant was not found");
        return entity;
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
    using T = flecs::pair<First, Second>;
    return component_id<actual_type_t<T>, false, First, First>{
        _::type<T>::id(world), 0};
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

template <typename T = void, typename Second = void>
struct value_type : actual_type<conditional_t<std::is_void_v<Second>, T, flecs::pair<T, Second>>> {};

template <typename... T>
using value_type_t = typename value_type<T...>::type;

template <typename... T, typename A>
auto value_id(world_t *world, const A&) {
    if constexpr (sizeof...(T)) {
        return make_id<T...>(world);
    } else {
        return make_id<A>(world);
    }
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

template <typename Id>
void add_component(world_t *world, flecs::entity_t entity, Id id) {
    flecs_static_assert(std::is_void_v<typename Id::construct_type> ||
        is_flecs_constructible<typename Id::construct_type>::value,
        "cannot default construct component: use emplace<T>()");
    ecs_add_id(world, entity, id.id);
}

template <typename Id>
bool has_component(world_t *world, flecs::entity_t entity, Id id) {
    if (ecs_has_id(world, entity, id.id)) {
        return true;
    }
    if constexpr (Id::sparse && is_enum_v<typename Id::type>) {
        return ecs_has_pair(world, entity, id.id, flecs::Wildcard);
    }
    return false;
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
