/**
 * @file addons/cpp/utils/signature.hpp
 * @brief Compile-time utilities for deriving query attributes from a parameter pack.
 */

#pragma once

namespace flecs {
namespace _ {

    template <typename T>
    constexpr flecs::inout_kind_t type_to_inout() {
        return is_const_p<T>::value ? flecs::In :
            (is_reference<T>::value ? flecs::InOut : flecs::InOutDefault);
    }

    template <typename T>
    constexpr flecs::oper_kind_t type_to_oper() {
        return is_pointer<T>::value ? flecs::Optional : flecs::And;
    }

    template <typename... Components, typename Builder>
    void populate_signature(flecs::world_t *world, Builder *builder) {
        (void)world;
        (void)builder;
        (builder->with(_::type<remove_pointer_t<Components>>::id(world))
            .inout(type_to_inout<Components>()).oper(type_to_oper<Components>()), ...);
    }

} // namespace _
} // namespace flecs
