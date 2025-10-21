// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsId.h"
#include "SolidMacros/Macros.h"
#include "FlecsArchetype.generated.h"

/**
 * @brief A wrapper for a flecs archetype (flecs::type)
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

    NO_DISCARD FORCEINLINE int32 GetComponentCount() const
    {
        return Type.count();
    }

    const FFlecsId* begin() const
    {
        return reinterpret_cast<const FFlecsId*>(Type.begin());
    }

    const FFlecsId* end() const
    {
        return reinterpret_cast<const FFlecsId*>(Type.end());
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

    /**
     * @brief Get the id at the given index.
     * @param InIndex The index of the id.
     * @return The id at the given index.
     */
    NO_DISCARD FORCEINLINE FFlecsId GetAtIndex(const int32 InIndex) const
    {
        solid_checkf(InIndex >= 0 && InIndex < Type.count(),
            TEXT("Index %d is out of bounds for archetype with %d components"), InIndex, Type.count());
        return FFlecsId(Type.get(InIndex));
    }
    
    flecs::type Type;
    
}; // struct FFlecsArchetype
