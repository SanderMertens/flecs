// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsQueryExpression.h"
#include "FlecsQueryNameExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryNameExpression : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsQueryNameExpression();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Query")
	FString Name;

	virtual void Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const override;
	
}; // struct FFlecsQueryNameExpression