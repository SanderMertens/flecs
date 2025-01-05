// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryTraitExpression.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryTraitInputType : uint8
{
	LastTerm,
	ScriptStruct,
	String
}; // enum class EFlecsQueryTraitInputType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryTraitExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryTraitExpression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	EFlecsQueryTraitInputType ComponentInputType = EFlecsQueryTraitInputType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs",
		meta = (EditCondition = "TraitType == EFlecsQueryTraitInputType::String", EditConditionHides))
	FString ComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs",
		meta = (EditCondition = "TraitType == EFlecsQueryTraitInputType::ScriptStruct", EditConditionHides))
	TObjectPtr<UScriptStruct> ComponentStruct;

	/*
	* If true, the trait holding entity will be marked as optional to allow for
	* entities that do not have the trait to be included in the query results.
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	bool bIsExclusionary = false;

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryTraitExpression
