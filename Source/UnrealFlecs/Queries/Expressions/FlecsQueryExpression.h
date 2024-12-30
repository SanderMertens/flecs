// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "StructUtils/InstancedStruct.h"
#include "FlecsQueryExpression.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryExpression();
	FORCEINLINE FFlecsQueryExpression(const bool bInAllowsChildExpressions)
	#if WITH_EDITORONLY_DATA
		: bAllowsChildExpressions(bInAllowsChildExpressions)
	#endif // WITH_EDITORONLY_DATA
	{}
	
	virtual ~FFlecsQueryExpression() = default;
	
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const;

	#if WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere, Category = "Flecs | Query")
	bool bAllowsChildExpressions = false;

	#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query",
		meta = (EditCondition = "bAllowsChildExpressions", EditConditionHides,
			AllowEditInlineCustomization, ShowOnlyInnerProperties, ExcludeBaseStruct))
	TArray<TInstancedStruct<FFlecsQueryExpression>> Children;
	
}; // struct FFlecsQueryExpression

template<>
struct TStructOpsTypeTraits<FFlecsQueryExpression> : public TStructOpsTypeTraitsBase2<FFlecsQueryExpression>
{
	enum
	{
		WithPureVirtual = true
	};
}; // struct TStructOpsTypeTraits<FFlecsQueryExpression>
