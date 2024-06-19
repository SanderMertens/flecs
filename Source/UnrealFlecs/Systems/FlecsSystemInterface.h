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

	FORCEINLINE void InitializeSystem_Internal(const flecs::world& InWorld)
	{
		flecs::system_builder<> Builder(InWorld, StringCast<ANSICHAR>(*GetName()).Get());
		BuildSystem(Builder);
		
		InitializeSystem();
		BP_InitializeSystem();
	}

	FORCEINLINE virtual void InitializeSystem() 
	{
	}

	FORCEINLINE virtual void BuildSystem(flecs::system_builder<>& Builder)
		PURE_VIRTUAL(IFlecsSystemInterface::BuildSystem, return;);

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Initialize System"))
	void BP_InitializeSystem();

	FORCEINLINE NO_DISCARD FFlecsSystem GetSystem() const
	{
		return System;
	}

	FORCEINLINE NO_DISCARD FString GetName() const;
	
	FFlecsSystem System;
	
}; // class IFlecsSystemInterface
