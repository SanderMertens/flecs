/**
 * @file addons/cpp/utils/signature.hpp
 * @brief Compile-time utilities for deriving query attributes from a parameter pack.
 */

#pragma once

namespace flecs {
namespace _ {

    template <typename T, if_t< is_const_p<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::In;
    }

    template <typename T, if_t< is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOut;
    }

    template <typename T, if_not_t< 
        is_const_p<T>::value || is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOutDefault;
    }

    template <typename T, if_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::Optional;
    }

    template <typename T, if_not_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::And;
    }

    // Type-erased term population shared by all query/system/observer builders.
    // Kept out-of-line so the per-signature builder code is reduced to building
    // the id/inout/oper arrays plus a single call.
    FLECS_NOINLINE
    inline int32_t populate_query_terms(
        ecs_query_desc_t *desc,
        int32_t term_index,
        const flecs::id_t *ids,
        const flecs::inout_kind_t *inout,
        const flecs::oper_kind_t *oper,
        int32_t count)
    {
        ecs_assert((term_index + count) <= FLECS_TERM_COUNT_MAX,
            ECS_INVALID_PARAMETER,
            "query exceeds maximum number of terms (%d)", FLECS_TERM_COUNT_MAX);

        for (int32_t i = 0; i < count; i ++) {
            ecs_term_t *term = &desc->terms[term_index + i];
            *term = ecs_term_t{};
            if (ids[i] & ECS_ID_FLAGS_MASK) {
                term->id = ids[i];
            } else {
                term->first.id = ids[i];
            }
            term->inout = static_cast<int16_t>(inout[i]);
            term->oper = static_cast<int16_t>(oper[i]);
        }
        return term_index + count;
    }

    template <typename ... Components>
    struct sig {
        sig(flecs::world_t *world)
            : world_(world)
            , ids({ (_::type<remove_pointer_t<Components>>::id(world))... })
            , inout ({ (type_to_inout<Components>())... })
            , oper ({ (type_to_oper<Components>())... })
        { }

        flecs::world_t *world_;
        flecs::array<flecs::id_t, sizeof...(Components)> ids;
        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout;
        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper;

        template <typename Builder>
        void populate(const Builder& b) {
            b->populate_terms(ids.ptr(), inout.ptr(), oper.ptr(),
                static_cast<int32_t>(sizeof...(Components)));
        }
    };

} // namespace _
} // namespace flecs
