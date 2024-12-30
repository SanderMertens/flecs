// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	TArray<FFlecsQueryTermExpression> Terms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	EFlecsQueryFlags Flags = EFlecsQueryFlags::None;
	
}; // struct FFlecsQueryDefinition
