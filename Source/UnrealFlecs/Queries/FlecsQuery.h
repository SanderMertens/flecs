// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQueryDefinition.h"
#include "Entities/FlecsEntityHandle.h"
#include "SolidMacros/Macros.h"
#include "FlecsQuery.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct FFlecsQuery
{
    GENERATED_BODY()

    FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsQuery& InQuery)
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

    template<typename ...TArgs>
    FORCEINLINE FFlecsQuery(const flecs::world& InWorld, const char* InName)
    {
        Query = InWorld.query<TArgs...>(InName);
    }

    FFlecsQuery(UFlecsWorld* InFlecsWorld, const FString& InName, const FFlecsQueryDefinition& InDefinition);

    FORCEINLINE NO_DISCARD bool HasChanged() const
    {
        return Query.changed();
    }

    FORCEINLINE NO_DISCARD flecs::query<> GetQuery() const
    {
        return Query;
    }

    FORCEINLINE NO_DISCARD int32 GetCount() const
    {
        return ecs_query_match_count(Query);
    }

    FORCEINLINE NO_DISCARD int32 GetFieldCount()
    {
        return Query.field_count();
    }

    FORCEINLINE NO_DISCARD bool HasMatches() const
    {
        return ecs_query_is_true(Query);
    }

    FORCEINLINE NO_DISCARD FString ToString() const
    {
        return StringCast<TCHAR>(ecs_query_str(Query)).Get();
    }

    FORCEINLINE NO_DISCARD FFlecsEntityHandle GetEntity() const
    {
        return FFlecsEntityHandle(ecs_get_entity(Query));
    }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsQuery& Other) const
    {
        return GetEntity() == Other.GetEntity();
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsQuery& Other) const
    {
        return !(*this == Other);
    }

    FORCEINLINE NO_DISCARD bool operator==(const flecs::query<>& Other) const
    {
        return GetEntity() == FFlecsEntityHandle(ecs_get_entity(Other));
    }
    
    flecs::query<> Query;
}; // struct FFlecsQuery


