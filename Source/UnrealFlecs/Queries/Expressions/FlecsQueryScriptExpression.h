// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "Queries/FlecsQueryScriptExpr.h"
#include "FlecsQueryScriptExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryScriptExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryScriptExpression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	FFlecsQueryScriptExpr ScriptExpr;

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryScriptExpression
