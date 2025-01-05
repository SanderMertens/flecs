// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystemInterface.h"
#include "UObject/Object.h"
#include "FlecsSystemObject.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsSystemObject : public UObject, public IFlecsSystemInterface
{
	GENERATED_BODY()

public:

private:
	FORCEINLINE virtual void BuildSystem(flecs::system_builder<>& Builder) override final
	{
		
	}


}; // class UFlecsSystemObject
