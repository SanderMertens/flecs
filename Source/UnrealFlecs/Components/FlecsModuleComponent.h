// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsModuleComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleComponent
{
	GENERATED_BODY()

	FORCEINLINE FFlecsModuleComponent() = default;

	FORCEINLINE FFlecsModuleComponent(const TSubclassOf<UObject>& InModuleClass)
		: ModuleClass(InModuleClass)
	{
	}

	FORCEINLINE NO_DISCARD TSubclassOf<UObject> GetModuleClass() const { return ModuleClass; }

	FORCEINLINE FFlecsModuleComponent& operator=(const TSubclassOf<UObject>& InModuleClass)
	{
		ModuleClass = InModuleClass;
		return *this;
	}

	FORCEINLINE FFlecsModuleComponent& operator=(const FFlecsModuleComponent& InComponent)
	{
		ModuleClass = InComponent.ModuleClass;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsModuleComponent& InComponent) const
	{
		return ModuleClass == InComponent.ModuleClass;
	}

	FORCEINLINE bool operator!=(const FFlecsModuleComponent& InComponent) const
	{
		return !(*this == InComponent);
	}

	UPROPERTY()
	TSubclassOf<UObject> ModuleClass;

}; // struct FFlecsModuleComponent