// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"

#include "Entities/FlecsEntityHandle.h"
#include "FlecsQueryDefinition.h"

#include "FlecsQuery.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct FFlecsQuery
{
    GENERATED_BODY()

    NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsQuery& InQuery)
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

    FFlecsQuery(const TSolidNotNull<UFlecsWorld*> InFlecsWorld,
                const FString& InName,
                const FFlecsQueryDefinition& InDefinition);

    NO_DISCARD FORCEINLINE bool HasChanged() const
    {
        return Query.changed();
    }

    NO_DISCARD FORCEINLINE flecs::query<> GetQuery() const
    {
        return Query;
    }

    NO_DISCARD FORCEINLINE int32 GetCount() const
    {
        return ecs_query_match_count(Query);
    }

    NO_DISCARD FORCEINLINE int32 GetFieldCount()
    {
        return Query.field_count();
    }

    NO_DISCARD FORCEINLINE bool HasMatches() const
    {
        return ecs_query_is_true(Query);
    }

    NO_DISCARD FORCEINLINE FString ToString() const
    {
        return StringCast<TCHAR>(ecs_query_str(Query)).Get();
    }

    NO_DISCARD FORCEINLINE FFlecsEntityHandle GetEntity() const
    {
        return FFlecsEntityHandle(ecs_get_entity(Query));
    }

    NO_DISCARD FORCEINLINE bool operator==(const FFlecsQuery& Other) const
    {
        return Query.c_ptr() == Other.Query.c_ptr();
    }

    NO_DISCARD FORCEINLINE bool operator!=(const FFlecsQuery& Other) const
    {
        return !(*this == Other);
    }

    NO_DISCARD FORCEINLINE bool operator==(const flecs::query<>& Other) const
    {
        return GetEntity() == FFlecsEntityHandle(ecs_get_entity(Other));
    }
    
    flecs::query<> Query;
    
}; // struct FFlecsQuery

/*template <typename ...TArgs>
struct TFlecsQuery
{
}; // struct TFlecsQuery*/


