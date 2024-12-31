// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryScriptExpr.h"
#include "Entities/FlecsEntityHandle.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	EFlecsQueryInputType Type = EFlecsQueryInputType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::ScriptStruct", EditConditionHides))
	TObjectPtr<UScriptStruct> ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::Entity", EditConditionHides))
	FFlecsEntityHandle Entity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::String", EditConditionHides))
	FFlecsQueryScriptExpr Expr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query",
		meta = (EditCondition = "Type == EFlecsQueryInputType::GameplayTag", EditConditionHides))
	FGameplayTag Tag;
}; // struct FFlecsQueryInput
