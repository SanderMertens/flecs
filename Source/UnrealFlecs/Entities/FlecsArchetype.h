// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsId.h"
#include "SolidMacros/Macros.h"
#include "FlecsArchetype.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsArchetype
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsArchetype() = default;
    FORCEINLINE FFlecsArchetype(const flecs::type& InType) : Type(InType) {}
    FORCEINLINE FFlecsArchetype(const flecs::type* InType) : Type(*InType) {}

    FORCEINLINE void SetType(const flecs::type& InType) { Type = InType; }
    FORCEINLINE void SetType(const flecs::type* InType) { Type = *InType; }

    FORCEINLINE flecs::type& GetFlecsType() { return Type; }
    FORCEINLINE const flecs::type& GetFlecsType() const { return Type; }

    FORCEINLINE operator flecs::type&() { return GetFlecsType(); }
    FORCEINLINE operator const flecs::type&() const { return GetFlecsType(); }

    FORCEINLINE flecs::type* operator->() { return &Type; }
    FORCEINLINE const flecs::type* operator->() const { return &Type; }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsArchetype& Other) const
    {
        return Type == Other.Type;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsArchetype& Other) const
    {
        return Type != Other.Type;
    }

    /**
     * @brief Iterates over each component in the archetype.
     * 
     * @tparam FunctionType The type of the function to call for each component.
     * @param InFunction The function to call for each component.
     * Returns false to continue iteration, true to break.
     */
    template <typename FunctionType>
    FORCEINLINE void ForEach(FunctionType&& InFunction) const
    {
        using ReturnType = std::invoke_result_t<FunctionType, FFlecsId>;
        
        for (const FFlecsId Component : Type)
        {
            if constexpr (std::is_same_v<ReturnType, bool>)
            {
                if (InFunction(Component))
                {
                    break;
                }
            }
            else
            {
                InFunction(Component);
            }
        }
    }
    
    flecs::type Type;
}; // struct FFlecsArchetype
