// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryScriptExpr.generated.h"

/**
 * We need this type to have a custom PropertyEditor for the FlecsQueryScriptExpr type.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryScriptExpr
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Flecs | Query")
	FString Expr;
}; // struct FFlecsQueryScriptExpr
