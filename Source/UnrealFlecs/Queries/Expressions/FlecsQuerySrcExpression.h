// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsQuerySrcExpression.generated.h"

UENUM(BlueprintType)
enum class EFlecsQuerySrcType : uint8
{
	Entity,
	String
}; // enum class EFlecsQuerySrcType

USTRUCT(BlueprintType, meta = (DisplayName = "Source Expression"))
struct UNREALFLECS_API FFlecsQuerySrcExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQuerySrcExpression();

	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	EFlecsQuerySrcType SrcType = EFlecsQuerySrcType::Entity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "SrcType == EFlecsQuerySrcType::Entity", EditConditionHides))
	FFlecsEntityHandle Entity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query", 
		meta = (EditCondition = "SrcType == EFlecsQuerySrcType::String", EditConditionHides))
	FString Src;
	
}; // struct FFlecsQuerySrcExpression
