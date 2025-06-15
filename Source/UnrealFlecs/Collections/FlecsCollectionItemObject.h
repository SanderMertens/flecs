// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsCollectionRegistrationInterface.h"
#include "UObject/Object.h"
#include "FlecsCollectionItemObject.generated.h"

UCLASS(Abstract, BlueprintType, EditInlineNew)
class UNREALFLECS_API UFlecsCollectionItemObject : public UObject, public IFlecsCollectionRegistrationInterface
{
	GENERATED_BODY()

public:
	virtual void RegisterCollection(
		const TSolidNotNull<UFlecsWorld*> InFlecsWorld, const FFlecsEntityHandle& InCollectionEntity) override;
	
}; // class UFlecsCollectionItemObject
