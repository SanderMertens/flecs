// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SolidNotNull.h"
#include "UObject/Interface.h"
#include "FlecsObjectRegistrationInterface.generated.h"

class UFlecsWorld;

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UFlecsObjectRegistrationInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsObjectRegistrationInterface

class UNREALFLECS_API IFlecsObjectRegistrationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RegisterObject(const TSolidNotNull<UFlecsWorld*> InFlecsWorld) = 0;

}; // class IFlecsObjectRegistrationInterface
