/**
 * @file addons/cpp/entity_component_tuple.hpp
 * @brief Utilities to fetch component as tuples from entities.
 * 
 */

#pragma once

/**
 * @ingroup cpp_entities
 * @{
 */

namespace flecs
{

    template<std::size_t size, typename... Args>
    struct tuple_builder {};

    // Size 2

    template<typename T, typename U>
    struct tuple_2 {
        T val1; U val2;
    };

    template<typename... Args>
    struct tuple_builder<2, Args...> {
        using type = tuple_2<Args&...>;
        using type_ptr = tuple_2<Args*...>;
        using type_const = tuple_2<const Args&...>;
        using type_const_ptr = tuple_2<const Args*...>;
    };

    // Size 3

    template<typename T, typename U, typename V>
    struct tuple_3 {
        T val1; U val2; V val3;
    };

    template<typename... Args>
    struct tuple_builder<3, Args...> {
        using type = tuple_3<Args&...>;
        using type_ptr = tuple_3<Args*...>;
        using type_const = tuple_3<const Args&...>;
        using type_const_ptr = tuple_3<const Args*...>;
    };

    // Size 4

    template<typename T, typename U, typename V, typename W>
    struct tuple_4 {
        T val1; U val2; V val3; W val4;
    };

    template<typename... Args>
    struct tuple_builder<4, Args...> {
        using type = tuple_4<Args&...>;
        using type_ptr = tuple_4<Args*...>;
        using type_const = tuple_4<const Args&...>;
        using type_const_ptr = tuple_4<const Args*...>;
    };

    // Size 5

    template<typename T, typename U, typename V, typename W, typename X>
    struct tuple_5 {
        T val1; U val2; V val3; W val4; X val5;
    };

    template<typename... Args>
    struct tuple_builder<5, Args...> {
        using type = tuple_5<Args&...>;
        using type_ptr = tuple_5<Args*...>;
        using type_const = tuple_5<const Args&...>;
        using type_const_ptr = tuple_5<const Args*...>;
    };

    // Size 6

    template<typename T, typename U, typename V, typename W, typename X, typename Y>
    struct tuple_6 {
        T val1; U val2; V val3; W val4; X val5; Y val6;
    };

    template<typename... Args>
    struct tuple_builder<6, Args...> {
        using type = tuple_6<Args&...>;
        using type_ptr = tuple_6<Args*...>;
        using type_const = tuple_6<const Args&...>;
        using type_const_ptr = tuple_6<const Args*...>;
    };

    // Size 7

    template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z>
    struct tuple_7 {
        T val1; U val2; V val3; W val4; X val5; Y val6; Z val7;
    };

    template<typename... Args>
    struct tuple_builder<7, Args...> {
        using type = tuple_7<Args&...>;
        using type_ptr = tuple_7<Args*...>;
        using type_const = tuple_7<const Args&...>;
        using type_const_ptr = tuple_7<const Args*...>;
    };

    // Size 8

    template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z, typename A>
    struct tuple_8 {
        T val1; U val2; V val3; W val4; X val5; Y val6; Z val7; A val8;
    };

    template<typename... Args>
    struct tuple_builder<8, Args...> {
        using type = tuple_8<Args&...>;
        using type_ptr = tuple_8<Args*...>;
        using type_const = tuple_8<const Args&...>;
        using type_const_ptr = tuple_8<const Args*...>;
    };

}

/** @} */
