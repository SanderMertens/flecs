// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsOperQueryExpression.h"
#include "FlecsQueryExpression.h"
#include "Queries/FlecsQueryInputType.h"
#include "FlecsQueryTermExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Term Query Expression"))
struct UNREALFLECS_API FFlecsQueryTermExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryTermExpression();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query | Term")
	FFlecsQueryInput InputType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query | Term")
	bool bWithout = false;

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryTermExpression
