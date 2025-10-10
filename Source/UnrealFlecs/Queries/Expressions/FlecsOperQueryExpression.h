// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "FlecsQueryExpression.h"
#include "Queries/Enums/FlecsQueryOperators.h"
#include "Types/SolidNotNull.h"

#include "FlecsOperQueryExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Operator Expression"))
struct UNREALFLECS_API FFlecsOperQueryExpression final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsOperQueryExpression();
	
	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	EFlecsQueryOperator Operator = EFlecsQueryOperator::Default;
	
}; // struct FFlecsOperQueryExpression