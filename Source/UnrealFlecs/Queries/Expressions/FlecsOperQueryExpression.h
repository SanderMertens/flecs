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
	FORCEINLINE FFlecsOperQueryExpression() = default;
	
	FORCEINLINE virtual void Apply(flecs::query_builder<>& InQueryBuilder) override
	{
		InQueryBuilder.oper(static_cast<flecs::oper_kind_t>(Operator));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flecs | Query | Operator")
	EFlecsQueryOperator Operator = EFlecsQueryOperator::Default;
	
}; // struct FFlecsOperQueryExpression