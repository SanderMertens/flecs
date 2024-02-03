// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityFlags.generated.h"

UENUM(BlueprintType)
enum class EFlecsEntityFlags : uint8
{
    None = 0,
}; // enum class EFlecsEntityFlags
ENUM_CLASS_FLAGS(EFlecsEntityFlags);
