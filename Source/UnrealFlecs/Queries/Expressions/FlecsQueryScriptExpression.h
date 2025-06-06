// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "Queries/FlecsQueryScriptExpr.h"
#include "FlecsQueryScriptExpression.generated.h"

USTRUCT(BlueprintType, DisplayName = "Script Expression")
struct UNREALFLECS_API FFlecsQueryScriptExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryScriptExpression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query", meta = (ShowOnlyInnerProperties))
	FFlecsQueryScriptExpr ScriptExpr;

	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryScriptExpression
