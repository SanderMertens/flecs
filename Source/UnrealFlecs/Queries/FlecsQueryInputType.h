// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Entities/FlecsEntityHandle.h"
#include "FlecsQueryScriptExpr.h"

#include "FlecsQueryInputType.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryInputType : uint8
{
	ScriptStruct,
	Entity,
	String,
	GameplayTag,
}; // enum class EFlecsQueryInputType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryInput
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	EFlecsQueryInputType Type = EFlecsQueryInputType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::ScriptStruct", EditConditionHides))
	TObjectPtr<UScriptStruct> ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::Entity", EditConditionHides))
	FFlecsId Entity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::String", EditConditionHides))
	FFlecsQueryScriptExpr Expr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::GameplayTag", EditConditionHides))
	FGameplayTag Tag;
	
}; // struct FFlecsQueryInput
