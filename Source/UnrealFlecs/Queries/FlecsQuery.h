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

    FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsQuery& InQuery)
    {
        return GetTypeHash(InQuery.GetQuery().entity().id());
    }

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

    FORCEINLINE NO_DISCARD bool IsOrphaned() const
    {
        return Query.orphaned();
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

    FORCEINLINE NO_DISCARD FString ToString() const
    {
        return static_cast<FString>(Query.str());
    }

    FORCEINLINE NO_DISCARD FFlecsEntityHandle GetEntity() const
    {
        return FFlecsEntityHandle(Query.entity());
    }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsQuery& Other) const
    {
        return Query == Other.Query;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsQuery& Other) const
    {
        return Query != Other.Query;
    }

    FORCEINLINE NO_DISCARD bool operator==(const flecs::query<>& Other) const
    {
        return Query == Other;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const flecs::query<>& Other) const
    {
        return Query != Other;
    }

private:
    flecs::query<> Query;
}; // struct FFlecsQuery
