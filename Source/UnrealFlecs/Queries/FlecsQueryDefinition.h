// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryFlags.h"
#include "Expressions/FlecsQueryTermExpression.h"
#include "FlecsQueryDefinition.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryDefinition
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryDefinition() = default;

	template <TQueryExpressionConcept TExpression>
	FORCEINLINE void AddExpression(const TExpression& InExpression)
	{
		OtherExpressions.Emplace(InExpression);
	}

	FORCEINLINE void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
	{
		solid_check(InWorld != nullptr);
		
		InQueryBuilder.flags(Flags);
		
		for (const FFlecsQueryTermExpression& Term : Terms)
		{
			Term.Apply(InWorld, InQueryBuilder);
		}

		for (const TInstancedStruct<FFlecsQueryExpression>& OtherExpression : OtherExpressions)
		{
			OtherExpression.Get<FFlecsQueryExpression>().Apply(InWorld, InQueryBuilder);
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	TArray<FFlecsQueryTermExpression> Terms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query", meta = (Bitmask, BitmaskEnum = "EFlecsQueryFlags"))
	uint8 Flags = static_cast<uint8>(EFlecsQueryFlags::None);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFlecsQueryExpression>> OtherExpressions;
	
}; // struct FFlecsQueryDefinition
