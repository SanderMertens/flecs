// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "flecs.h"
#include "Queries/Enums/FlecsQueryOperators.h"
#include "FlecsOperQueryExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Operator Query Expression"))
struct UNREALFLECS_API FFlecsOperQueryExpression final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsOperQueryExpression();
	
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	EFlecsQueryOperator Operator = EFlecsQueryOperator::Default;
	
}; // struct FFlecsOperQueryExpression