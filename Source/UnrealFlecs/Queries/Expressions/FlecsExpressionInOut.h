// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "Queries/Enums/FlecsQueryInOut.h"
#include "FlecsExpressionInOut.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsExpressionInOut final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsExpressionInOut() = default;
	
	FORCEINLINE virtual void Apply(flecs::query_builder<>& InQueryBuilder) override
	{
		InQueryBuilder.inout(static_cast<flecs::inout_kind_t>(InOut));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flecs | Query | Expression")
	EFlecsQueryInOut InOut = EFlecsQueryInOut::Default;
	
}; // struct FFlecsExpressionInOut
