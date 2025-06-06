// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryFilterExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Filter Expression"))
struct UNREALFLECS_API FFlecsQueryFilterExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryFilterExpression();

	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;
};
