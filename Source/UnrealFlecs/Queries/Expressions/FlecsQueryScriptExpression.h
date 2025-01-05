// Elie Wiese-Namir © 2024. All Rights Reserved.

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
