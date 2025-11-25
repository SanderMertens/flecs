// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"

#include "Templates/SubclassOf.h"

#include "SolidMacros/Macros.h"

#include "FlecsModuleComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleComponent
{
	GENERATED_BODY()

	FORCEINLINE FFlecsModuleComponent() = default;

	FORCEINLINE FFlecsModuleComponent(const TSubclassOf<UObject> InModuleClass)
		: ModuleClass(InModuleClass)
	{
	}

	NO_DISCARD FORCEINLINE TSubclassOf<UObject> GetModuleClass() const
	{
		return ModuleClass;
	}

	FORCEINLINE FFlecsModuleComponent& operator=(UClass* InModuleClass)
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

	UPROPERTY(EditAnywhere, Category = "Flecs | Module")
	TSubclassOf<UObject> ModuleClass;

}; // struct FFlecsModuleComponent

REGISTER_FLECS_COMPONENT(FFlecsModuleComponent);