// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryFlags.h"
#include "Enums/FlecsQueryCache.h"
#include "Expressions/FlecsQueryTermExpression.h"
#include "FlecsQueryDefinition.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryDefinition
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryDefinition() = default;

	FORCEINLINE FFlecsQueryDefinition& AddQueryTerm(const FFlecsQueryTermExpression& InTerm)
	{
		Terms.Add(InTerm);
		return *this;
	}

	template <TQueryExpressionConcept TExpression>
	FORCEINLINE FFlecsQueryDefinition& AddExpression(const TExpression& InExpression)
	{
		OtherExpressions.Add(InExpression);
		return *this;
	}

	FORCEINLINE void Apply(const TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
	{
		InQueryBuilder.cache_kind(static_cast<flecs::query_cache_kind_t>(CacheType));
		InQueryBuilder.query_flags(Flags);

		if (bDetectChanges)
		{
			InQueryBuilder.detect_changes();
		}
		
		for (const FFlecsQueryTermExpression& Term : Terms)
		{
			Term.Apply(InWorld, InQueryBuilder);
		}

		for (const TInstancedStruct<FFlecsQueryExpression>& OtherExpression : OtherExpressions)
		{
			OtherExpression.Get<FFlecsQueryExpression>().Apply(InWorld, InQueryBuilder);
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	TArray<FFlecsQueryTermExpression> Terms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	EFlecsQueryCacheType CacheType = EFlecsQueryCacheType::Default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query", meta = (Bitmask, BitmaskEnum = "EFlecsQueryFlags"))
	uint8 Flags = static_cast<uint8>(EFlecsQueryFlags::None);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	bool bDetectChanges = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFlecsQueryExpression>> OtherExpressions;
	
}; // struct FFlecsQueryDefinition
