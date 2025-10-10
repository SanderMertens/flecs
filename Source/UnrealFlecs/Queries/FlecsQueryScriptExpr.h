// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryScriptExpr.generated.h"

/**
 * @TODO: We need this type to have a custom PropertyEditor for the FlecsQueryScriptExpr type.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryScriptExpr
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Query")
	FString Expr;
	
}; // struct FFlecsQueryScriptExpr
