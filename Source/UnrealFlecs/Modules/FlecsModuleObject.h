// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsModuleInterface.h"
#include "UObject/Object.h"
#include "FlecsModuleObject.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, ClassGroup = (Flecs))
class UNREALFLECS_API UFlecsModuleObject : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()
}; // class UFlecsModuleObject
