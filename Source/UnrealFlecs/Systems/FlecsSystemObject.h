// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystemInterface.h"
#include "FlecsSystemSettingsInfo.h"
#include "UObject/Object.h"
#include "FlecsSystemObject.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsSystemObject : public UObject, public IFlecsSystemInterface
{
	GENERATED_BODY()

public:
	UFlecsSystemObject(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs", meta = (ExposeOnSpawn = true))
	FFlecsSystemSettingsInfo SystemSettings;

private:

}; // class UFlecsSystemObject
