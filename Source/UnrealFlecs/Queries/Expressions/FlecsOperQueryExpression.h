// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "flecs.h"
#include "Queries/Enums/FlecsQueryOperators.h"
#include "FlecsOperQueryExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Operator Query Expression"))
struct UNREALFLECS_API FFlecsOperQueryExpression final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsOperQueryExpression();
	
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flecs | Query")
	EFlecsQueryOperator Operator = EFlecsQueryOperator::Default;
	
}; // struct FFlecsOperQueryExpression