// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Macros.h"
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

    FORCEINLINE NO_DISCARD bool HasChanged() const
    {
        return Query.changed();
    }

    FORCEINLINE NO_DISCARD flecs::world GetWorld() const
    {
        return Query.world();
    }

    FORCEINLINE NO_DISCARD flecs::query<> GetQuery() const
    {
        return Query;
    }

    FORCEINLINE NO_DISCARD int32 GetCount() const
    {
        return Query.count();
    }

    FORCEINLINE NO_DISCARD int32 GetFieldCount() const
    {
        return Query.field_count();
    }

    FORCEINLINE NO_DISCARD bool HasMatches() const
    {
        return Query.is_true();
    }

private:
    flecs::query<> Query;
}; // struct FFlecsQuery
