// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystem.h"
#include "FlecsSystemSettingsInfo.h"
#include "Components/FlecsUObjectComponent.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "FlecsSystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsSystemInterface : public UFlecsEntityInterface
{
	GENERATED_BODY()
}; // class UFlecsSystemInterface

class UNREALFLECS_API IFlecsSystemInterface : public IFlecsEntityInterface
{
	GENERATED_BODY()

public:

	void InitializeSystem_Internal(const flecs::world& InWorld);

	FORCEINLINE virtual void InitializeSystem()
	{
	}

	virtual FFlecsEntityHandle GetEntityHandle() const override final;

	virtual void BuildSystem(flecs::system_builder<>& Builder)
		PURE_VIRTUAL(IFlecsSystemInterface::BuildSystem, return;);

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Initialize System"))
	void BP_InitializeSystem();

	NO_DISCARD FORCEINLINE FFlecsSystem GetSystem() const
	{
		return System;
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Systems")
	FFlecsSystemSettingsInfo GetSystemSettings() const;
	
	FFlecsSystem System;
	
}; // class IFlecsSystemInterface

