// Solstice Games © 2024. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#ifndef SOLID_FOR_H
#define SOLID_FOR_H

#include "CoreMinimal.h"

namespace Solid
{
    template <typename MaxType, typename FunctionType>
    FORCEINLINE constexpr void For(const MaxType Max, FunctionType Function)
    {
        static_assert(std::is_integral_v<MaxType>, "MaxType must be an integral type.");
        
        for (MaxType Index = 0; Index < Max; ++Index)
        {
            Function(Index);
        }
    }

    template <typename MinType, typename MaxType, typename FunctionType>
    FORCEINLINE constexpr void For(const MinType Min, const MaxType Max, FunctionType Function)
    {
        static_assert(std::is_integral_v<MinType> && std::is_integral_v<MaxType>, "MinType and MaxType must be integral types.");
        
        for (MinType Index = Min; Index < Max; ++Index)
        {
            Function(Index);
        }
    }

    template <typename T>
    concept THasSizeConcept = requires(T InType)
    {
        InType.size();
    }; // concept THasSizeConcept

    template <typename T>
    concept THasNumConcept = requires(T InType)
    {
        InType.Num();
    }; // concept THasNumConcept

    template <THasSizeConcept ContainerType, typename FunctionType>
    FORCEINLINE constexpr void For(const ContainerType& Container, FunctionType Function)
    {
        constexpr auto Size = Container.size();
        
        for (decltype(Size) Index = 0; Index < Size; ++Index)
        {
            Function(Index, Container[Index]);
        }
    }

    template <THasNumConcept ContainerType, typename FunctionType>
    FORCEINLINE constexpr void For(const ContainerType& Container, FunctionType Function)
    {
        constexpr auto Size = Container.Num();
        
        for (decltype(Size) Index = 0; Index < Size; ++Index)
        {
            Function(Index, Container[Index]);
        }
    }

    template <typename FunctionType, std::size_t ...Indices>
    FORCEINLINE constexpr void ForEachImpl(FunctionType Function, std::index_sequence<Indices...>)
    {
        (Function(std::integral_constant<std::size_t, Indices>{}), ...);
    }

    template <std::size_t Num, typename FunctionType>
    FORCEINLINE constexpr void ForEach(FunctionType Function)
    {
        ForEachImpl(Function, std::make_index_sequence<Num>{});
    }

    template <typename Tuple, typename FunctionType, std::size_t... Indices>
    FORCEINLINE constexpr void ForEachTupleImpl(FunctionType Function, std::index_sequence<Indices...>)
    {
        (Function(std::get<Indices>(std::forward<Tuple>(Tuple{}))), ...);
    }

    template <typename Tuple, typename FunctionType, std::size_t... Indices>
    FORCEINLINE constexpr void ForEachTupleImpl(Tuple&& InTuple, FunctionType Function, std::index_sequence<Indices...>)
    {
        (Function(std::get<Indices>(std::forward<Tuple>(InTuple))), ...);
    }

    template <uint32 Start, uint32 End, typename Tuple, typename FunctionType, std::size_t... Indices>
    FORCEINLINE constexpr void ForEachTupleImpl(FunctionType Function, std::index_sequence<Indices...>)
    {
        (Function(std::integral_constant<uint32, Start + Indices>{}), ...);
    }

    template <typename Tuple, typename FunctionType>
    FORCEINLINE constexpr void ForEachTuple(Tuple&& InTuple, FunctionType Function)
    {
        ForEachTupleImpl<decltype(InTuple), FunctionType>(std::forward<Tuple>(InTuple),
            Function, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }

    template <uint32 Start, uint32 End, typename Tuple, typename FunctionType>
    FORCEINLINE constexpr void ForEachTuple(FunctionType Function)
    {
        ForEachTupleImpl<Start, End, Tuple, FunctionType>(
            Function, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }
    
} // namespace Solid

#endif // SOLID_FOR_H
