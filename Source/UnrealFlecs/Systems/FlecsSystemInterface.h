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

	FORCEINLINE void InitializeSystem_Internal(const flecs::world& InWorld)
	{
		const FFlecsSystemSettingsInfo& Settings = GetSystemSettings();
		
		flecs::system_builder Builder(InWorld, StringCast<char>(*Settings.Name).Get());

		if (Settings.Kind != FFlecsEntityHandle::GetNullHandle())
		{
			Builder.kind(Settings.Kind);
		}

		switch (Settings.TimerKind)
		{
		case EFlecsSystemTimerKind::None:
			break;
		case EFlecsSystemTimerKind::Interval:
			Builder.interval(Settings.Interval);
			break;
		case EFlecsSystemTimerKind::Rate:
			Builder.rate(Settings.Rate);
			break;
		};

		if (Settings.TickSource != FFlecsEntityHandle::GetNullHandle())
		{
			Builder.tick_source(Settings.TickSource);
		}

		Builder.priority(Settings.Priority);
		
		BuildSystem(Builder);

		System = FFlecsSystem(Builder); // Builder.build();
		
		Settings.SystemRecord.ApplyRecordToEntity(System.GetEntity());
		//@TODO: Add FlecsSystemObject Component Target Type
		//System.GetEntity().Set<FFlecsUObjectComponent>({ _getUObject() });
		
		InitializeSystem();
		BP_InitializeSystem();
	}

	FORCEINLINE virtual void InitializeSystem()
	{
	}

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override final
	{
		return System.GetEntity();
	}

	virtual void BuildSystem(flecs::system_builder<>& Builder)
		PURE_VIRTUAL(IFlecsSystemInterface::BuildSystem, return;);

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Initialize System"))
	void BP_InitializeSystem();

	FORCEINLINE NO_DISCARD FFlecsSystem GetSystem() const
	{
		return System;
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Systems")
	FFlecsSystemSettingsInfo GetSystemSettings() const;
	
	FFlecsSystem System;
	
}; // class IFlecsSystemInterface

