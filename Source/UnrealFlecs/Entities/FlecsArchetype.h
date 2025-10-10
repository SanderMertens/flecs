// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsId.h"
#include "SolidMacros/Macros.h"
#include "FlecsArchetype.generated.h"

/**
 * @brief A wrapper for a flecs archetype, flecs::type in C++.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsArchetype
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsArchetype() = default;
    
    FORCEINLINE FFlecsArchetype(const flecs::type& InType) : Type(InType)
    {
    }
    
    FORCEINLINE FFlecsArchetype(const flecs::type* InType)
        : Type(*InType)
    {
    }

    void SetType(const flecs::type& InType);
    void SetType(const flecs::type* InType);

    NO_DISCARD FORCEINLINE flecs::type& GetFlecsType() { return Type; }
    NO_DISCARD FORCEINLINE const flecs::type& GetFlecsType() const { return Type; }

    FORCEINLINE operator flecs::type&() { return GetFlecsType(); }
    FORCEINLINE operator const flecs::type&() const { return GetFlecsType(); }

    FORCEINLINE flecs::type* operator->() { return &Type; }
    FORCEINLINE const flecs::type* operator->() const { return &Type; }

    NO_DISCARD FORCEINLINE bool operator==(const FFlecsArchetype& Other) const
    {
        return Type == Other.Type;
    }

    NO_DISCARD FORCEINLINE bool operator!=(const FFlecsArchetype& Other) const
    {
        return Type != Other.Type;
    }

    /**
     * @brief Iterates over each component in the archetype.
     * @tparam FunctionType The type of the function to call for each component.
     * @param InFunction The function to call for each component.
     * @return false to continue iteration, true to break.
     */
    template <typename FunctionType>
    FORCEINLINE_DEBUGGABLE void ForEach(FunctionType&& InFunction) const
    {
        using ReturnType = std::invoke_result_t<FunctionType, FFlecsId>;
        
        for (const FFlecsId Component : Type)
        {
            if constexpr (std::is_same_v<ReturnType, bool>)
            {
                if (std::invoke(std::forward<FunctionType>(InFunction), Component))
                {
                    break;
                }
            }
            else
            {
                std::invoke(std::forward<FunctionType>(InFunction), Component);
            }
        }
    }
    
    flecs::type Type;
    
}; // struct FFlecsArchetype
