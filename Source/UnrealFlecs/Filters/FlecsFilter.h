// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsFilter.generated.h"

USTRUCT(BlueprintType)
struct FFlecsFilter
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsFilter() = default;
    FORCEINLINE FFlecsFilter(const flecs::filter_base& InFilter) : Filter(InFilter) {}

    FORCEINLINE void SetFilter(const flecs::filter_base& InFilter) { Filter = InFilter; }

    FORCEINLINE flecs::filter_base& GetFlecsFilter() { return Filter; }
    FORCEINLINE const flecs::filter_base& GetFlecsFilter() const { return Filter; }

    FORCEINLINE operator flecs::filter_base&() { return GetFlecsFilter(); }
    FORCEINLINE operator const flecs::filter_base&() const { return GetFlecsFilter(); }
    FORCEINLINE operator flecs::filter_base*() { return &Filter; }

    FORCEINLINE bool operator==(const FFlecsFilter& Other) const
    {
        return Filter == Other.Filter;
    }

    FORCEINLINE bool operator!=(const FFlecsFilter& Other) const
    {
        return Filter != Other.Filter;
    }

private:
    flecs::filter_base Filter;
}; // struct FFlecsFilter
