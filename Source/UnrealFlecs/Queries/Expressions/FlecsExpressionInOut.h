// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "Queries/Enums/FlecsQueryInOut.h"
#include "FlecsExpressionInOut.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "In Out Expression"))
struct UNREALFLECS_API FFlecsExpressionInOut final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsExpressionInOut();
	
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flecs | Query")
	EFlecsQueryInOut InOut = EFlecsQueryInOut::Default;
	
}; // struct FFlecsExpressionInOut
