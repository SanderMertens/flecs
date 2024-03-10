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
    
    FORCEINLINE FFlecsQuery(const flecs::query<>& InQuery) : Query(InQuery) {}
    FORCEINLINE FFlecsQuery(const flecs::query<>* InQuery) : Query(*InQuery) {}

    FORCEINLINE FFlecsQuery(flecs::query_builder<>& InQueryBuilder)
    {
        Query = InQueryBuilder.build();
    }

private:
    flecs::query<> Query;
}; // struct FFlecsQuery
