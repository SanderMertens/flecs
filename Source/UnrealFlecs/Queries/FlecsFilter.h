// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsFilter.generated.h"

USTRUCT(BlueprintType)
struct FFlecsFilter
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsFilter() = default;

    FORCEINLINE FFlecsFilter(const flecs::filter<>& InFilter) : Filter(InFilter) {}
    FORCEINLINE FFlecsFilter(const flecs::filter<>* InFilter) : Filter(*InFilter) {}

    FORCEINLINE NO_DISCARD flecs::filter<>& GetFlecsFilter() { return Filter; }
    FORCEINLINE NO_DISCARD const flecs::filter<>& GetFlecsFilter() const { return Filter; }

    FORCEINLINE operator flecs::filter<>&() { return GetFlecsFilter(); }
    FORCEINLINE operator const flecs::filter<>&() const { return GetFlecsFilter(); }

    FORCEINLINE NO_DISCARD flecs::filter<>* operator->() { return &Filter; }
    FORCEINLINE NO_DISCARD const flecs::filter<>* operator->() const { return &Filter; }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsFilter& Other) const
    {
        return Filter == Other.Filter;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsFilter& Other) const
    {
        return Filter != Other.Filter;
    }

private:
    flecs::filter<> Filter;
}; // struct FFlecsFilter