// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "UObject/Interface.h"
#include "FlecsEntityInterface.generated.h"

UINTERFACE(BlueprintType)
class UNREALFLECS_API UFlecsEntityInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsEntityInterface

class UNREALFLECS_API IFlecsEntityInterface
{
	GENERATED_BODY()

public:

	NO_DISCARD virtual FFlecsEntityHandle GetEntityHandle() const
		PURE_VIRTUAL(IFlecsEntityInterface::GetEntityHandle, return FFlecsEntityHandle(););

	NO_DISCARD FORCEINLINE bool IsValidEntity() const
	{
		return GetEntityHandle().IsValid();
	}

}; // class IFlecsEntityInterface
