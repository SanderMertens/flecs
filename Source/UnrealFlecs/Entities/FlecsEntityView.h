// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityHandleTypes.h"
#include "Types/SolidNotNull.h"
#include "FlecsEntityView.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityView : public FFlecsCommonHandle
{
	GENERATED_BODY()

public:
	SOLID_INLINE FFlecsEntityView() = default;

protected:
	
}; // struct FFlecsEntityView
