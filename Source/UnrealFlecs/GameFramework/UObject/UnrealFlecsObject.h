// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "UObject/Object.h"
#include "UnrealFlecsObject.generated.h"

UCLASS(Abstract, BlueprintType)
class UNREALFLECS_API UUnrealFlecsObject : public UObject, public IFlecsEntityInterface
{
	GENERATED_BODY()

public:
	UUnrealFlecsObject();
	UUnrealFlecsObject(const FObjectInitializer& ObjectInitializer);
	

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override
	{
		return ObjectEntityHandle;
	}
	
protected:
	UPROPERTY()
	FFlecsEntityHandle ObjectEntityHandle;
}; // class UUnrealFlecsObject
