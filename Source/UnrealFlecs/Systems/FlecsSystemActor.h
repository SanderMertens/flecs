// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystemInterface.h"
#include "FlecsSystemSettingsInfo.h"
#include "GameFramework/Actor.h"
#include "FlecsSystemActor.generated.h"

UCLASS(Abstract)
class UNREALFLECS_API AFlecsSystemActor : public AActor, public IFlecsSystemInterface
{
	GENERATED_BODY()

public:
	AFlecsSystemActor();
	AFlecsSystemActor(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs", meta = (ExposeOnSpawn = true))
	FFlecsSystemSettingsInfo SystemSettings;

}; // class AFlecsSystemActor
