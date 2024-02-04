// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsType.generated.h"

USTRUCT(BlueprintType)
struct FFlecsType
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsType() = default;
    FORCEINLINE FFlecsType(const flecs::type& InType) : Type(InType) {}
    FORCEINLINE FFlecsType(const flecs::type* InType) : Type(*InType) {}

    FORCEINLINE void SetType(const flecs::type& InType) { Type = InType; }
    FORCEINLINE void SetType(const flecs::type* InType) { Type = *InType; }

    FORCEINLINE flecs::type& GetFlecsType() { return Type; }
    FORCEINLINE const flecs::type& GetFlecsType() const { return Type; }

    FORCEINLINE operator flecs::type&() { return GetFlecsType(); }
    FORCEINLINE operator const flecs::type&() const { return GetFlecsType(); }

    FORCEINLINE flecs::type* operator->() { return &Type; }
    FORCEINLINE const flecs::type* operator->() const { return &Type; }

    FORCEINLINE bool operator==(const FFlecsType& Other) const
    {
        return Type == Other.Type;
    }

    FORCEINLINE bool operator!=(const FFlecsType& Other) const
    {
        return Type != Other.Type;
    }

private:
    flecs::type Type;
}; // struct FFlecsType
