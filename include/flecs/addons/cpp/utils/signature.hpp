/**
 * @file addons/cpp/utils/signature.hpp
 * @brief Compile time utilities for deriving query attributes from param pack.
 */

#pragma once

#include <stdio.h>

namespace flecs {
namespace _ {

    template <typename T, if_t< is_const_p<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::In;
    }

    template <typename T, if_t< is_reference<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOut;
    }

    template <typename T, if_not_t< 
        is_const_p<T>::value || is_reference<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOutDefault;
    }

    template <typename T, if_t< is_pointer<T>::value > = 0>
    static constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::Optional;
    }

    template <typename T, if_not_t< is_pointer<T>::value > = 0>
    static constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::And;
    }

    template <typename ... Components>
    struct sig {
        sig(flecs::world_t *world) 
            : m_world(world)
            , ids({ (_::type<Components>::id(world))... })
            , inout ({ (type_to_inout<Components>())... })
            , oper ({ (type_to_oper<Components>())... }) 
        { }

        flecs::world_t *m_world;
        flecs::array<flecs::id_t, sizeof...(Components)> ids;
        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout;
        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper;

        template <typename Builder>
        void populate(const Builder& b) {
            size_t i = 0;
            for (auto id : ids) {
                b->with(id).inout(inout[i]).oper(oper[i]);
                i ++;
            }
        }
    };

} // namespace _
} // namespace flecs
