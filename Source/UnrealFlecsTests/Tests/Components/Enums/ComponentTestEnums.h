// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComponentTestEnums.generated.h"

enum class ETestEnum : uint8
{
	None,
	One,
	Two,
	Three
}; // enum class ETestEnum

UENUM()
enum class ETestEnum_UENUM : uint8
{
	None,
	One,
	Two,
	Three
}; // enum class ETestEnum
