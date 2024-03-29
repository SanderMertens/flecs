// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "UObject/Interface.h"
#include "FlecsEntityInterface.generated.h"

UINTERFACE(BlueprintType)
class UFlecsEntityInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsEntityInterface

class UNREALFLECS_API IFlecsEntityInterface
{
	GENERATED_BODY()

public:

	virtual FFlecsEntityHandle GetEntityHandle() const
		PURE_VIRTUAL(IFlecsEntityInterface::GetEntityHandle, return FFlecsEntityHandle(););

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entity", meta = (DisplayName = "Get Entity Handle"))
	FORCEINLINE FFlecsEntityHandle K2_GetEntityHandle() const
	{
		return GetEntityHandle();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
	FORCEINLINE bool IsValid() const
	{
		return GetEntityHandle().IsValid();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
	FORCEINLINE bool IsAlive() const
	{
		return GetEntityHandle().IsAlive();
	}

}; // class IFlecsEntityInterface
