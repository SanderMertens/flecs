/**
 * @file addons/cpp/entity_component_tuple.hpp
 * @brief Utilities to fetch component as tuples from entities.
 */

#pragma once

/**
 * @ingroup cpp_entities
 * @{
 */

namespace flecs
{

    /** Builds tuple types for a given number of component types.
     *
     * @tparam size The number of components.
     * @tparam Args The component types.
     */
    template<std::size_t size, typename... Args>
    struct tuple_builder {};

    /** Tuple of 2 elements. */
    template<typename T, typename U>
    struct tuple_2 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
    };

    /** Tuple builder specialization for 2 elements. */
    template<typename... Args>
    struct tuple_builder<2, Args...> {
        using type = tuple_2<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_2<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_2<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_2<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 3 elements. */
    template<typename T, typename U, typename V>
    struct tuple_3 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
    };

    /** Tuple builder specialization for 3 elements. */
    template<typename... Args>
    struct tuple_builder<3, Args...> {
        using type = tuple_3<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_3<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_3<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_3<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 4 elements. */
    template<typename T, typename U, typename V, typename W>
    struct tuple_4 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
        W val4; /**< Fourth element. */
    };

    /** Tuple builder specialization for 4 elements. */
    template<typename... Args>
    struct tuple_builder<4, Args...> {
        using type = tuple_4<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_4<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_4<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_4<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 5 elements. */
    template<typename T, typename U, typename V, typename W, typename X>
    struct tuple_5 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
        W val4; /**< Fourth element. */
        X val5; /**< Fifth element. */
    };

    /** Tuple builder specialization for 5 elements. */
    template<typename... Args>
    struct tuple_builder<5, Args...> {
        using type = tuple_5<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_5<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_5<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_5<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 6 elements. */
    template<typename T, typename U, typename V, typename W, typename X, typename Y>
    struct tuple_6 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
        W val4; /**< Fourth element. */
        X val5; /**< Fifth element. */
        Y val6; /**< Sixth element. */
    };

    /** Tuple builder specialization for 6 elements. */
    template<typename... Args>
    struct tuple_builder<6, Args...> {
        using type = tuple_6<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_6<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_6<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_6<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 7 elements. */
    template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z>
    struct tuple_7 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
        W val4; /**< Fourth element. */
        X val5; /**< Fifth element. */
        Y val6; /**< Sixth element. */
        Z val7; /**< Seventh element. */
    };

    /** Tuple builder specialization for 7 elements. */
    template<typename... Args>
    struct tuple_builder<7, Args...> {
        using type = tuple_7<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_7<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_7<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_7<const Args*...>; /**< Const pointer tuple type. */
    };

    /** Tuple of 8 elements. */
    template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z, typename A>
    struct tuple_8 {
        T val1; /**< First element. */
        U val2; /**< Second element. */
        V val3; /**< Third element. */
        W val4; /**< Fourth element. */
        X val5; /**< Fifth element. */
        Y val6; /**< Sixth element. */
        Z val7; /**< Seventh element. */
        A val8; /**< Eighth element. */
    };

    /** Tuple builder specialization for 8 elements. */
    template<typename... Args>
    struct tuple_builder<8, Args...> {
        using type = tuple_8<Args&...>; /**< Reference tuple type. */
        using type_ptr = tuple_8<Args*...>; /**< Pointer tuple type. */
        using type_const = tuple_8<const Args&...>; /**< Const reference tuple type. */
        using type_const_ptr = tuple_8<const Args*...>; /**< Const pointer tuple type. */
    };

}

/** @} */
