// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQuery.generated.h"

USTRUCT(BlueprintType)
struct FFlecsQuery
{
    GENERATED_BODY()

public:
    FORCEINLINE FFlecsQuery() = default;
    FORCEINLINE FFlecsQuery(const flecs::query_base& InQuery) : Query(InQuery) {}
    FORCEINLINE FFlecsQuery(const flecs::query_base* InQuery) : Query(*InQuery) {}

    FORCEINLINE void SetQuery(const flecs::query_base& InQuery) { Query = InQuery; }
    FORCEINLINE void SetQuery(const flecs::query_base* InQuery) { Query = *InQuery; }

    FORCEINLINE flecs::query_base& GetFlecsQuery() { return Query; }
    FORCEINLINE const flecs::query_base& GetFlecsQuery() const { return Query; }

    FORCEINLINE operator flecs::query_base&() { return GetFlecsQuery(); }
    FORCEINLINE operator const flecs::query_base&() const { return GetFlecsQuery(); }

private:
    flecs::query_base Query;
}; // struct FFlecsQuery