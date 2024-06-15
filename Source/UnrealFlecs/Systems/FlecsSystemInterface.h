// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystem.h"
#include "UObject/Interface.h"
#include "FlecsSystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UFlecsSystemInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsSystemInterface

class UNREALFLECS_API IFlecsSystemInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE void InitializeSystem_Internal(const FFlecsSystem& InSystem)
	{
		System = InSystem;
		solid_checkf(System.IsValid(), TEXT("Invalid system"));
		
		InitializeSystem();
		BP_InitializeSystem();
	}

	FORCEINLINE virtual void InitializeSystem()
	{
	}

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Initialize System"))
	void BP_InitializeSystem();

private:
	
	FFlecsSystem System;

	
}; // class IFlecsSystemInterface
