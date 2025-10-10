// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsGameLoopInterface.h"
#include "UObject/Object.h"
#include "FlecsGameLoopObject.generated.h"

UCLASS(Abstract, EditInlineNew, BlueprintType, NotBlueprintable, Category = "Flecs|GameLoop")
class UNREALFLECS_API UFlecsGameLoopObject : public UObject, public IFlecsGameLoopInterface
{
	GENERATED_BODY()

public:
	UFlecsGameLoopObject();
	UFlecsGameLoopObject(const FObjectInitializer& ObjectInitializer);
	
}; // class UFlecsGameLoopObject
