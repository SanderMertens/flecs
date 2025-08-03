// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "General/FlecsObjectRegistrationInterface.h"
#include "UObject/Interface.h"
#include "FlecsCollectionRegistrationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UNREALFLECS_API UFlecsCollectionRegistrationInterface : public UFlecsObjectRegistrationInterface
{
	GENERATED_BODY()
}; // class UFlecsCollectionRegistrationInterface

class UNREALFLECS_API IFlecsCollectionRegistrationInterface : public IFlecsObjectRegistrationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RegisterObject(const TSolidNotNull<UFlecsWorld*> InFlecsWorld) override final;

	virtual void RegisterCollection(const TSolidNotNull<UFlecsWorld*> InFlecsWorld,
		const FFlecsEntityHandle& InCollectionEntity) = 0;

	NO_DISCARD FORCEINLINE FFlecsEntityHandle GetCollectionEntity() const
	{
		return CollectionEntity;
	}

	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorld.Get();
	}

protected:
	FFlecsEntityHandle CollectionEntity;
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;

}; // class IFlecsCollectionRegistrationInterface
