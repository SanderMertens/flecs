// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryOptionalExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryOptionalExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryOptionalExpression();

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryOptionalExpression