// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntityRecordTestStructs.generated.h"

USTRUCT()
struct FTestStruct_EntityRecord
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 IntegerValue;
}; // struct FTestStruct_EntityRecord
