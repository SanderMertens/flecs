// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryOptionalExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryOptionalExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryOptionalExpression();

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryOptionalExpression