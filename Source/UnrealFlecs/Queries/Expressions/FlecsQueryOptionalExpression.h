// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryOptionalExpression.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Optional Expression"))
struct UNREALFLECS_API FFlecsQueryOptionalExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryOptionalExpression();

	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryOptionalExpression