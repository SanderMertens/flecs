// Fill out your copyright notice in the Description page of Project Settings.

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
	

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override
	{
		return ObjectEntityHandle;
	}
	
private:
	UPROPERTY()
	FFlecsEntityHandle ObjectEntityHandle;

}; // class UUnrealFlecsObject
