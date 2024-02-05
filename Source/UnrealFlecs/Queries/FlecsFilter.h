// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsEntityHandle.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorld.h"
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
    
    FORCEINLINE void Set(const flecs::filter<>& InFilter)
    {
        Filter = InFilter;
    }

    FORCEINLINE NO_DISCARD int32 GetCount() const
    {
        return Filter.count();
    }

    template <typename FunctionType>
    FORCEINLINE void ForEach(FunctionType Function) const
    {
        Filter.each(Function);
    }

    template <typename FunctionType>
    FORCEINLINE void ForEach(FFlecsWorld& InWorld, FunctionType Function) const
    {
        Filter.each(InWorld, Function);
    }

    template <typename FunctionType>
    FORCEINLINE void ForEach(const FFlecsEntityHandle& InEntity, FunctionType Function) const
    {
        Filter.each(InEntity, Function);
    }

    template <typename FunctionType>
    FORCEINLINE void Iterate(FunctionType Function) const
    {
        Filter.iter(Function);
    }

    template <typename FunctionType>
    FORCEINLINE void Iterate(FFlecsWorld& InWorld, FunctionType Function) const
    {
        Filter.iter(InWorld, Function);
    }

    template <typename FunctionType>
    FORCEINLINE void Iterate(const FFlecsEntityHandle& InEntity, FunctionType Function) const
    {
        Filter.iter(InEntity, Function);
    }

    template <typename FunctionType>
    FORCEINLINE NO_DISCARD FFlecsEntityHandle Find(FunctionType Function) const
    {
        return Filter.find(Function);
    }

    FORCEINLINE NO_DISCARD FFlecsEntityHandle First() const
    {
        return Filter.first();
    }

private:
    flecs::filter<> Filter;
}; // struct FFlecsFilter