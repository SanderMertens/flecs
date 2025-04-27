// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "StructUtils/InstancedStruct.h"
#include "FlecsQueryExpression.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintInternalUseOnly)
struct UNREALFLECS_API FFlecsQueryExpression
{
	GENERATED_BODY()

protected:
	FORCEINLINE explicit FFlecsQueryExpression(const bool bInAllowsChildExpressions)
	#if WITH_EDITORONLY_DATA
		: bAllowsChildExpressions(bInAllowsChildExpressions)
	#endif // WITH_EDITORONLY_DATA
	{}

public:
	FFlecsQueryExpression() = default;
	virtual ~FFlecsQueryExpression() = default;

	// We should only call the Super when we have child expressions
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const;

	#if WITH_EDITORONLY_DATA

	//UPROPERTY(VisibleAnywhere, Category = "Flecs | Query")
	UPROPERTY()
	bool bAllowsChildExpressions = false;

	#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "bAllowsChildExpressions", EditConditionHides, ShowOnlyInnerProperties, ExcludeBaseStruct))
	TArray<TInstancedStruct<FFlecsQueryExpression>> Children;
	
}; // struct FFlecsQueryExpression

template<>
struct TStructOpsTypeTraits<FFlecsQueryExpression> : public TStructOpsTypeTraitsBase2<FFlecsQueryExpression>
{
	enum
	{
		//WithPureVirtual = true
	};
}; // struct TStructOpsTypeTraits<FFlecsQueryExpression>

template <typename T>
concept TQueryExpressionConcept = std::is_base_of<FFlecsQueryExpression, T>::value;
