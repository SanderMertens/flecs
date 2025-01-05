// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystemInterface.h"
#include "GameFramework/Actor.h"
#include "FlecsSystemActor.generated.h"

UCLASS(Abstract)
class UNREALFLECS_API AFlecsSystemActor : public AActor, public IFlecsSystemInterface
{
	GENERATED_BODY()

public:

}; // class AFlecsSystemActor
