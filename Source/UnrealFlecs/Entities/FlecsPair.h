// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsPair.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPair
{
	GENERATED_BODY()

public:

private:
	flecs::entity PairId;
}; // struct FFlecsPair
