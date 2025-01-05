// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComponentTestStructs.generated.h"

struct FTestComponent_RegisterComponentTest
{
	int32 Value;
}; // struct FTestComponent

USTRUCT()
struct FUStructTestComponent_RegisterComponentTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value;
}; // struct FUStructTestComponent



