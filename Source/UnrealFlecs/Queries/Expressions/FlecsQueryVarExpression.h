// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryVarExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryVarExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryVarExpression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	FString VarName;

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryVarExpression