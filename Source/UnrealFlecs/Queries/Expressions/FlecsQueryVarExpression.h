// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryVarExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName="Var Expression"))
struct UNREALFLECS_API FFlecsQueryVarExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryVarExpression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	FString VarName;

	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryVarExpression