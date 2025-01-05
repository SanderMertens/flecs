// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsArchetype.generated.h"

USTRUCT(BlueprintType)
struct FFlecsArchetype
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
    
    flecs::type Type;
}; // struct FFlecsArchetype
