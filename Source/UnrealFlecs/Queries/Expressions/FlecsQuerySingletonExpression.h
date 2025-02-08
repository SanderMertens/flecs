// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQuerySingletonExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQuerySingletonExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQuerySingletonExpression();
	
	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQuerySingletonExpression
